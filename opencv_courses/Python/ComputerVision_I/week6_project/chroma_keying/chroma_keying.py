import cv2
import numpy as np
import sys

class ChromaKeying:
	def __init__(self, path, bkg_path=None, windowname='Video'):
		self.vid_path = path
		self.windowname = windowname
		self.clicked_points = (0, 0)
		if bkg_path == None:
			self.background_img = []
		else:
			self.background_img = cv2.imread(bkg_path)

		self.cap = cv2.VideoCapture(self.vid_path)
		# Check if camera opened successfully
		if (self.cap.isOpened() == False): 
			sys.exit('Error opening video stream or file')
		cv2.namedWindow(self.windowname, cv2.WINDOW_NORMAL)
		cv2.setMouseCallback(self.windowname, self.on_mouse)
		cv2.createTrackbar('Tolerance', self.windowname, 15, 30, self.on_trackbar_tolerance)
		cv2.createTrackbar('Softness', self.windowname, 15, 30, self.on_trackbar_softness)
		cv2.createTrackbar('Color Cast', self.windowname, 15, 30, self.on_trackbar_color_cast)
		cv2.createTrackbar('Playback FPS', self.windowname, 15, 60, self.on_trackbar_playback)
		self.fps = 30

	def on_mouse(self, event, x, y, flags, param):
		pt = (x, y)
		if event == cv2.EVENT_LBUTTONDOWN:
			pass
		elif event == cv2.EVENT_LBUTTONUP:
			self.clicked_points = pt

	def on_trackbar_tolerance(self, *args):
		pass
	def on_trackbar_softness(self, *args):
		pass	
	def on_trackbar_color_cast(self, *args):
		pass
	def on_trackbar_playback(self, *args):
		self.fps = args[0]

	def set_background(self, bkg_path):
		self.background_img = cv2.imread(bkg_path)

	def run(self):
		# Read until video is completed
		while(self.cap.isOpened()):
		  # Capture frame-by-frame
		  ret, frame = self.cap.read()

		  if ret == True:
		    cv2.imshow(self.windowname, frame)    
		    ch = cv2.waitKey(int(1.0/self.fps *1000.0))
		  else: 
		    break
def main():
	vid_path = './greenscreen-demo.mp4'
	chromakeying = ChromaKeying(vid_path)
	chromakeying.run()

if __name__ == "__main__":
	main()