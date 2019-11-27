/*-
 * Copyright (c) 2004
 *     Michael B. Durian
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 4. The name of the Author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "../libintrepidcs/intrepidcs.h"

#define TEXT_LEN 256

struct can_sniff_stat {
	int	delta;
	int	verbose;
	int	tx_message_valid[10];
	struct	ics_can_msg tx_message[10];
};

static void usage(void);
static int parse_tx_message(struct can_sniff_stat *css, int ch,
    const char *optarg, char *text, int text_len);

static void dev_stat_callback(struct ics_stream_state *ss,
    struct ics_dev_status *ds, void *client_data);
static void rx_overflow_callback(struct ics_stream_state *ss,
    struct ics_rx_overflow *rxo, void *client_data);
static void tx_complete_callback(struct ics_stream_state *ss,
    struct ics_tx_complete *txc, void *client_data);
static void can_error_callback(struct ics_stream_state *ss,
    struct ics_can_error *ce, void *client_data);
static void pic_overflow_callback(struct ics_stream_state *ss,
    void *client_data);
static void main51_overflow_callback(struct ics_stream_state *ss,
    struct ics_main51_overflow *mo, void *client_data);
static void hscan_callback(struct ics_stream_state *ss,
    struct ics_can_msg *msg, void *client_data);
static void swcan_callback(struct ics_stream_state *ss,
    struct ics_can_msg *msg, void *client_data);

int
main(int argc, char *argv[])
{
	struct ics_packet p;
	struct ics_interface *iface;
	struct ics_stream_state *ss;
	struct can_sniff_stat css;
	struct termios orig_t, t;
	fd_set rfds;
	uint32_t id, mask;
	int ch, done, i, is_valid, max_fd;
	char b, *chk_ptr, *filter_arg, *id_ptr, *mask_ptr;
	const char *pname;
	char text[TEXT_LEN];
	char bGuiFlag = 0;

	pname = NULL;
	css.verbose = 0;
	css.delta = 0;
	for (i = 0; i < 10; i++)
		css.tx_message_valid[i] = 0;
	while ((ch = getopt(argc, argv, "dgf:p:v0:1:2:3:4:5:6:7:8:9:")) != -1) {
		switch (ch) {
		case 'd':
			css.delta = 1;
			break;
		case 'g':
			  /*
			   *Don't assume stdin is a terminal
			   *added to facilitate Intrepid CS valuecan util
			   *g for GUI
			   */
			bGuiFlag = 1;
			break;
		case 'p':
			pname = optarg;
			break;
		case 'f':
			/*
			 * Don't do anything - we'll process this on
			 * the second pass.
			 */
			break;
		case 'v':
			css.verbose = 1;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (!parse_tx_message(&css, ch, optarg, text,
			    TEXT_LEN)) {
				fprintf(stderr, "Bad TX message: %s\n",
				    text);
				return (EXIT_FAILURE);
			}
			break;

		default:
			usage();
			return (EXIT_FAILURE);
		}
	}

	if (pname == NULL) {
		fprintf(stderr, "No CAN interface specified\n");
		usage();
		return (EXIT_FAILURE);
	}

	iface = ics_interface_init(pname, text, TEXT_LEN);
	if (iface == NULL) {
		fprintf(stderr, "ics_interface_init failed: %s\n", text);
		return (EXIT_FAILURE);
	}

	ss = ics_stream_init(iface, text, TEXT_LEN);
	if (ss == NULL) {
		fprintf(stderr, "ics_stream_init failed: %s\n", text);
		ics_interface_release(iface);
		return (EXIT_FAILURE);
	}


	/*
	 * Process command line arguments a second time to get
	 * filter values.  Since we've already verified that the
	 * command line only contains valid arguments, we don't
	 * need to worry about that here.
	 */
	#ifdef linux
	optind = 0;
	#else
	optind = 1;
	optreset = 1;
	#endif
	while ((ch = getopt(argc, argv, "dgf:p:v0:1:2:3:4:5:6:7:8:9:")) != -1) {
		switch (ch) {
		case 'd':
			break;
		case 'f':
			/*
			 * Make a copy of the filter argument so we
			 * can modify it.
			 */
			filter_arg = malloc(strlen(optarg) + 1);
			if (filter_arg == NULL) {
				fprintf(stderr, "Out of memory\n");
				ics_stream_release(ss);
				ics_interface_release(iface);
			}
			strcpy(filter_arg, optarg);
			/*
			 * The filter argument is in the form:
			 * id:mask
			 */
			id_ptr = filter_arg;

			/* skip ahead to : */
			mask_ptr = filter_arg;
			while (*mask_ptr != '\0' && *mask_ptr != ':')
				mask_ptr++;
			if (*mask_ptr == '\0') {
				fprintf(stderr, "Invalid filter argument '%s' "
				    "- must be in the form id:mask\n", optarg);
				free(filter_arg);
				ics_stream_release(ss);
				ics_interface_release(iface);
				return (EXIT_FAILURE);
			}
			/* put a '\0' at the end of the id */
			*mask_ptr = '\0';
			mask_ptr++;
			if (mask_ptr == '\0') {
				fprintf(stderr, "Invalid filter argument '%s' "
				    "- must be in the form id:mask\n", optarg);
				free(filter_arg);
				ics_stream_release(ss);
				ics_interface_release(iface);
				return (EXIT_FAILURE);
			}

			id = strtoul(id_ptr, &chk_ptr, 16);
			if (*chk_ptr != '\0') {
				fprintf(stderr, "Invalid filter ID value "
				    "'%s'\n", id_ptr);
				free(filter_arg);
				ics_stream_release(ss);
				ics_interface_release(iface);
				return (EXIT_FAILURE);
			}

			mask = strtoul(mask_ptr, &chk_ptr, 16);
			if (*chk_ptr != '\0') {
				fprintf(stderr, "Invalid filter mask value "
				    "'%s'\n", mask_ptr);
				free(filter_arg);
				ics_stream_release(ss);
				ics_interface_release(iface);
				return (EXIT_FAILURE);
			}
			free(filter_arg);

			/*
			 * We have a good filter value.
			 * Aside from checking for a NULL value, we don't
			 * care about the return value because we never
			 * remove filters.  If we wanted to remove a filter,
			 * we'd need to save this value so we could later
			 * pass it to stream_remove_filter.
			 */
			if (ics_stream_add_filter(ss, mask, id) == NULL) {
				fprintf(stderr, "Out of memory adding "
				    "filter\n");
				ics_stream_release(ss);
				ics_interface_release(iface);
				return (EXIT_FAILURE);
			}

			break;
		case 'p':
			break;
		case 'v':
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			break;
		case 'g':
			break;
		}
	}

	/*
	 * Let the stream processor know how we want to handle various
	 * messages.
	 */
	ics_stream_set_device_status_callback(ss, dev_stat_callback, &css);
	ics_stream_set_rx_overflow_callback(ss, rx_overflow_callback, &css);
	ics_stream_set_tx_complete_callback(ss, tx_complete_callback, &css);
	ics_stream_set_can_error_callback(ss, can_error_callback, &css);
	ics_stream_set_pic_overflow_callback(ss, pic_overflow_callback, &css);
	ics_stream_set_main51_overflow_callback(ss, main51_overflow_callback,
	    &css);
	ics_stream_set_hscan_callback(ss, hscan_callback, &css);
	ics_stream_set_swcan_callback(ss, swcan_callback, &css);

	/*
	 * The default a bit rate of 500kbs.  If we want a different CAN
	 * bitrate, we need to send a command which isn't yet documented.
	 */

	/*
	 * Since this program runs until it is killed, we never get a
	 * chance to properly cleanup when it exists.  Because of this,
	 * the vehicle interface is left in a peculiar state.  Just
	 * enabled CAN packet transmission isn't enough.  First we must
	 * disable CAN packet transmission and then re-enable it.
	 */
	ics_packet_create_can_disable(&p);
	if (!ics_interface_write_packet(iface, &p)) {
		fprintf(stderr, "Couldn't write disable packet: %s\n",
		    ics_interface_err_str(iface));
		ics_stream_release(ss);
		ics_interface_release(iface);
		return (EXIT_FAILURE);
	}

	/* Enable CAN message transmission */
	ics_packet_create_can_enable(&p);
	if (!ics_interface_write_packet(iface, &p)) {
		fprintf(stderr, "Couldn't write enable packet: %s\n",
		    ics_interface_err_str(iface));
		ics_stream_release(ss);
		ics_interface_release(iface);
		return (EXIT_FAILURE);
	}

	/*
	 * Put stdin into raw mode so we can detect a single keystroke
	 * before a newline is typed.
	 */
	if(!bGuiFlag) {
		if (tcgetattr(STDIN_FILENO, &orig_t) == -1) {
			fprintf(stderr, "Couldn't get termios for stdin: %s\n",
			    strerror(errno));
			ics_stream_release(ss);
			ics_interface_release(iface);
			return (EXIT_FAILURE);
		}
		memcpy(&t, &orig_t, sizeof(struct termios));
		t.c_lflag &= ~ICANON;
		t.c_cc[VMIN] = 0;
		t.c_cc[VTIME] = 0;
		/* don't echo stdin */
		t.c_lflag &= ~ECHO;
		if (tcsetattr(STDIN_FILENO, TCSANOW, &t) == -1) {
			fprintf(stderr, "Couldn't set termios for stdin: %s\n",
			    strerror(errno));
			ics_stream_release(ss);
			ics_interface_release(iface);
			return (EXIT_FAILURE);
		}
	}

	done = 0;
	while (!done) {
		/*
		 * Check for either data arriving from the interface or
		 * a character arriving from standard input.
		 */
		FD_ZERO(&rfds);
		FD_SET(ics_interface_fd(iface), &rfds);
		FD_SET(STDIN_FILENO, &rfds);
		if (ics_interface_fd(iface) > STDIN_FILENO)
			max_fd = ics_interface_fd(iface);
		else
			max_fd = STDIN_FILENO;
		if (select(max_fd + 1, &rfds, NULL, NULL, NULL) == -1) {
			fprintf(stderr, "select failed: %s\n",
			    strerror(errno));
			ics_stream_release(ss);
			ics_interface_release(iface);
			if(!bGuiFlag)
				(void)tcsetattr(STDIN_FILENO, TCSANOW, &orig_t);
			exit(EXIT_FAILURE);
		}
		if (FD_ISSET(ics_interface_fd(iface), &rfds)) {
			if (!ics_interface_read_packet(iface, &p, &is_valid)) {
				fprintf(stderr, "interface_read_packet "
				    "failed: %s\n",
				    ics_interface_err_str(iface));
				ics_stream_release(ss);
				ics_interface_release(iface);
				if(!bGuiFlag)
					(void)tcsetattr(STDIN_FILENO, TCSANOW, &orig_t);
				return (EXIT_FAILURE);
			}
			if (!is_valid)
				fprintf(stderr, "malformed packet\n");
			else {
				if (css.verbose) {
					ics_packet_format(text, TEXT_LEN, &p);
					printf("%s\n", text);
				}
				if (!ics_stream_process_packet(ss, &p)) {
					if (css.verbose) {
						ics_packet_format(text,
						    TEXT_LEN, &p);
						printf("no handler for"
						    "packet '%s'\n", text);
					}
				}
			}
		}
		if (FD_ISSET(STDIN_FILENO, &rfds)) {
			/*
			 * a keypress occurred - read it and if it is a
			 * 'q', then it's time for the program to exit.
			 */
			if (read(STDIN_FILENO, &b, 1) == -1) {
				fprintf(stderr, "stdin read failed: %s\n",
				    strerror(errno));
				ics_stream_release(ss);
				ics_interface_release(iface);
				if(!bGuiFlag)
					(void)tcsetattr(STDIN_FILENO, TCSANOW, &orig_t);
				return (EXIT_FAILURE);
			}
			/*
			 * if the key is '0' - '9', then send that TX
			 * message instead of exiting.
			 */
			if (b >= '0' && b <= '9') {
				if (css.tx_message_valid[b - '0']) {
					ics_packet_create_can_msg(&p,
					    &css.tx_message[b - '0']);
					if (!ics_interface_write_packet(iface,
					    &p)) {
						fprintf(stderr, "could not "
						    "write packet: %s\n",
						    ics_interface_err_str(
						    iface));
						ics_stream_release(ss);
						ics_interface_release(iface);
						if(!bGuiFlag)
							(void)tcsetattr(STDIN_FILENO,
						   	 TCSANOW, &orig_t);
						return(EXIT_FAILURE);
					}
				}
			} else {
				/* exit on any other keypress, not just 'q' */
				done = 1;
			}
		}
	}

	/* NOT REACHED */
	ics_stream_release(ss);
	ics_interface_release(iface);
	if(!bGuiFlag)
		(void)tcsetattr(STDIN_FILENO, TCSANOW, &orig_t);
	return (EXIT_SUCCESS);
}

static void
usage(void)
{

	fprintf(stderr, "can_sniff [-vd] -p port\n");
}

static int
parse_tx_message(struct can_sniff_stat *css, int ch, const char *optarg,
    char *text, int text_len)
{
	int ret;
	char *str, *sptr, *id_str, *chk_ptr, *data_str;
	struct ics_can_msg *msg;

	ret = 1;
	if (ch >= '0' && ch <= '9')
		msg = &css->tx_message[ch - '0'];
	else {
		snprintf(text, text_len, "Invalid TX number %c", ch);
		ret = 0;
	}

	str = NULL;
	if (ret) {
		str = malloc(strlen(optarg) + 1);
		if (str != NULL)
			strcpy(str, optarg);
		else {
			snprintf(text, text_len, "Out of memory");
			ret = 0;
		}
	}
	if (ret) {
		sptr = str;
		id_str = strsep(&sptr, ":");
		if (id_str == NULL) {
			snprintf(text, text_len, "No ID for TX message %c",
			    ch);
			ret = 0;
		} else {
			msg->can_id = strtoul(id_str, &chk_ptr, 16);
			if (*chk_ptr != '\0') {
				snprintf(text, text_len, "Invalid ID '%s' for "
				    "TX message %c", id_str, ch);
				ret = 0;
			}
		}
	}
	if (ret) {
		msg->data_len = 0;
		while (ret && (data_str = strsep(&sptr, " ")) != NULL) {
			if (*data_str != '\0') {
				msg->data[msg->data_len] = strtoul(data_str,
				    &chk_ptr, 16);
				if (*chk_ptr != '\0') {
					snprintf(text, text_len, "Invalid "
					    "data byte num %d, '%s' in "
					    "TX message %c", msg->data_len,
					    data_str, ch);
					ret = 0;
				} else if (msg->data_len >= ICS_MAX_CAN_DATA) {
					snprintf(text, text_len, "Too many "
					    "data bytes in TX message %c",
					    ch);
					ret = 0;
				} else {
					msg->data_len++;
				}
			}
		}
	}
	if (ret) {
		msg->netid = ICS_NETID_HSCAN;
		msg->timestamp = ch - '0';
		if (msg->can_id > 0x7ff)
			msg->is_extended_id = 1;
		else
			msg->is_extended_id = 0;
		msg->is_remote_frame = 0;

		css->tx_message_valid[ch - '0'] = 1;
	}
	if (str != NULL)
		free(str);
	return (ret);
}

static void
dev_stat_callback(struct ics_stream_state *ss, struct ics_dev_status *ds,
    void *client_data)
{
	struct can_sniff_stat *css;

	css = (struct can_sniff_stat *)client_data;

	ics_stream_update_device_status(ss, ds);
	if (css->verbose) {
		printf("Timestamp overflow\n");
		fflush(stdout);
	}
}

static void
rx_overflow_callback(struct ics_stream_state *ss, struct ics_rx_overflow *rxo,
    void *client_data)
{
	struct can_sniff_stat *css;

	css = (struct can_sniff_stat *)client_data;

	printf("RX Overflow\n");
	fflush(stdout);
}

static void
tx_complete_callback(struct ics_stream_state *ss, struct ics_tx_complete *txc,
    void *client_data)
{
	struct can_sniff_stat *css;
	struct ics_can_msg msg;
	char msg_text[256];

	css = (struct can_sniff_stat *)client_data;

	/*
	* Identify which message was sent based on the description
	* and copy it into a local structure so we can adjust the timestamp.
	*/
	memcpy(&msg, &css->tx_message[txc->desc], sizeof(struct ics_can_msg));

	/*
	 * Now replace the timestamp in our local copy (which is really
	 * the description ID with the correct timestamp.
	 */
	msg.timestamp = txc->timestamp;

	/* Format the message into something readable */
	ics_stream_format_can_msg(ss, msg_text, 256, &msg, css->delta);
	printf("%s (TX)\n", msg_text);

	fflush(stdout);
}

static void
can_error_callback(struct ics_stream_state *ss, struct ics_can_error *ce,
    void *client_data)
{
	struct can_sniff_stat *css;

	css = (struct can_sniff_stat *)client_data;

	if (ce->rx1ovr)
		printf("RX1OVR ");
	if (ce->rx0ovr)
		printf("RX0OVR ");
	if (ce->txbo)
		printf("TXBO ");
	if (ce->txep)
		printf("TXEP ");
	if (ce->rxep)
		printf("RXEP ");
	if (ce->txwar)
		printf("TXWAR ");
	if (ce->rxwar)
		printf("RXWAR ");
	if (ce->ewarn)
		printf("EWARN ");
	printf("\n");
	fflush(stdout);
}

static void
pic_overflow_callback(struct ics_stream_state *ss, void *client_data)
{
	struct can_sniff_stat *css;

	css = (struct can_sniff_stat *)client_data;

	printf("PIC Overflow\n");
	fflush(stdout);
}

static void
main51_overflow_callback(struct ics_stream_state *ss,
    struct ics_main51_overflow *mo, void *client_data)
{
	struct can_sniff_stat *css;

	css = (struct can_sniff_stat *)client_data;

	printf("Main51 Overflow\n");
	fflush(stdout);
}

static void
hscan_callback(struct ics_stream_state *ss, struct ics_can_msg *msg,
    void *client_data)
{
	struct can_sniff_stat *css;
	char msg_text[256];

	css = (struct can_sniff_stat *)client_data;

	if (msg == NULL)
		printf("invalid HSCAN packet\n");
	else {
		ics_stream_format_can_msg(ss, msg_text, 256, msg, css->delta);
		printf("HSCAN: %s\n", msg_text);
	}
	fflush(stdout);
}

static void
swcan_callback(struct ics_stream_state *ss, struct ics_can_msg *msg,
    void *client_data)
{
	struct can_sniff_stat *css;
	char msg_text[256];

	css = (struct can_sniff_stat *)client_data;

	if (msg == NULL)
		printf("invalid SWCAN packet\n");
	else {
		ics_stream_format_can_msg(ss, msg_text, 256, msg, css->delta);
		printf("SWCAN: %s\n", msg_text);
	}
	fflush(stdout);
}