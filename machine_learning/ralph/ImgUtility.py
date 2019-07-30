#!/usr/bin/env python

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import cv2
import numpy as np
import matplotlib.pyplot as plt
from os import listdir
from os.path import isfile, join

from DataStructures import ObjectList

class Frame():
	def __init__(self):
		## metadata ##
		self.id = 0 #frame id
		self.pose = np.zeros(7) #pose of the camera relative to a frame [position, quaternion]
		self.variance = np.zeros(3) #variance of the camera relative to a frame[x,y,z]
		self.velocity = np.zeros(6) #velocity of the camera relative to a frame [linear_velocity, angular_velocity]
		self.acceleration = np.zeros(6) #acceleration of the camera relative to a frame [linear_acceleration, angular_accleration]
		
		## Data ##
		self.image = []
		self.objectlist = []
		self.segmentationmask = []

	## setter ##
	def set_image(self, img):
		self.image = img

	def set_objectlist(self, list):
		self.objectlist = list

	def set_segmentationmask(self, mask):
		self.segmentationmask = mask

	def set_id(self, id):
		self.id = id

	def set_pose(self, pose):
		self.pose = pose

	def set_variance(self, var):
		self.variance = var

	def set_velocity(self, velocity):
		self.velocity = velocity

	def set_acceleration(self, acceleration):
		self.acceleration = acceleration

	## getter ##
	def get_image(self):
		return self.image

	def get_objectlist(self):
		return self.objectlist

	def get_segmentationmask(self):
		return self.segmentationmask

	def get_id(self):
		return self.id

	def get_pose(self, pose):
		return self.pose

	def get_variance(self, var):
		return self.variance

	def get_velocity(self, velocity):
		return self.velocity

	def get_acceleration(self, acceleration):
		return self.acceleration
	
	## class methods ##
	def show_image(self):
		plt.imshow(self.image)
		plt.show()

def _configure_optimizer(learning_rate):
	"""Configures the optimizer used for training.

	Args:
	learning_rate: A scalar or `Tensor` learning rate.

	Returns:
	An instance of an optimizer.

	Raises:
	ValueError: if FLAGS.optimizer is not recognized.
	"""
	if FLAGS.optimizer == 'adadelta':
		optimizer = tf.keras.optimizers.Adadelta(learning_rate, rho, epsilon)
	elif FLAGS.optimizer == 'adagrad':
		optimizer = tf.keras.optimizers.Adagrad(learning_rate, initial_accumulator_value, epsilon)
	elif FLAGS.optimizer == 'adam':
		optimizer = tf.keras.optimizers.Adam(learning_rate, beta_1, beta_2, epsilon)
	elif FLAGS.optimizer == 'ftrl':
		optimizer = tf.keras.optimizers.Ftrl(learning_rate, learning_rate_power, initial_accumulator_value,
			l1_regularization_strength, l2_regularization_strength)
	elif FLAGS.optimizer == 'adamax':
		optimizer = tf.keras.optimizers.Adamax(learning_rate, beta_1, beta_2, epsilon)
	elif FLAGS.optimizer == 'rmsprop':
		optimizer = tf.keras.optimizers.RMSprop(learning_rate, rho, momentum, epsilon, centered)
	elif FLAGS.optimizer == 'sgd':
		optimizer = tf.keras.optimizers.SGD(learning_rate, momentum, nesterov)
	else:
		raise ValueError('Optimizer [%s] was not recognized', FLAGS.optimizer)
    return optimizer

if __name__ == '__main__':
	path_im1 = r'../datasets/data_road/training/image_2/um_000000.png'
	path_im2 = r'../datasets/data_road/training/gt_image_2/um_lane_000000.png'
	path = path_im1
	file_format = path.split('.')[1]
	onlyfiles = [f for f in listdir(path[0:-13]) if isfile(join(path[0:-13], f))]
	for i in onlyfiles:
		filemember = i.split('.')
		filename = filemember[0].split('_')
		fileformat = filemember[1]
		print(filename)