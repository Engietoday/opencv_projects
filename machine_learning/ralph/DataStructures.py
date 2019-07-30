#!/usr/bin/env python

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

class ObjectList():
	def __init__(self):
		self.list = []
		
		self.listmember = DetectedObjects()

	def append_object(self, listmember):
		self.list.append(listmember)

	def delete_object(self, id):
		for i in range(0,len(self.list)):
			if id == self.list[i].objectheader.id:
				del self.list[i]
				break
			else:
				pass

	def get_list(self):
		return self.list

class DetectedObjects():
	def __init__(self):
		self.objectheader = objectheader()
		self.bbox3d = bbox3d()
		self.bbox2d = bbox2d()
		self.indicator = indicator()
		self.behavior = behavior()

class bbox3d():
	def __init__(self):
		self.space_frame = ''
		self.pose = np.zeros(7) #[position, quaternion]
		self.dimensions = np.zeros(3) #[length_x, length_y, length_z]
		self.variance = np.zeros(3) #[x,y,z]
		self.velocity = np.zeros(6) #[linear_velocity, angular_velocity]
		self.acceleration = np.zeros(6) #[linear_acceleration, angular_accleration]

		self.pointcloud = np.zeros(2,3) #[x,y,z;...]

		self.convex_hull = np.zeros(2,3) #[x,y,z;...]
		self.candidate_trajectory = np.zeros(2,3) #[x,y,z;...]
		self.pose_reliable = True
		self.velocity_reliable = True
		self.acceleration_reliable = True

class bbox2d():
	def __init__(self):
		self.image_frame = ''
		self.x = 0 # X coord in image space(pixel) of the initial point of the Rect
		self.y = 0 # Y coord in image space(pixel) of the initial point of the Rect
		self.width = 0 # Width of the Rect in pixels
		self.height = 0 # Height of the Rect in pixels
		self.angle = 0.0 # Angle [0 to 2*PI), allow rotated rects
		self.image = [] # ROI of image inside the bounding box.

def objectheader():
	def __init__(self):
		self.id = 0
		self.label = ''
		self.score = 0.0
		self.color = [0, 0, 0, 0] #Define this object with a color. [R, B, G, A]
		self.valid = True

def indicator():
	def __init__(self):
		self.indiator_state = 0

def behavior():
	def __init__(self):
		self.behavior_state = 0
		self.user_defined_info = ''
