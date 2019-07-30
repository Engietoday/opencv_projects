#!/usr/bin/env python

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function


from DataLoaderStrategy import DataLoaderStrategy
import cv2
import numpy as np

class DataLoaderMapillary(DataLoaderStrategy):
	def _print(self):
		print("Hello from Map")

	def get_training_by_id(self, id = ''):
		_trainingpath = self.training_path
		if _trainingpath == '':
			_trainingpath = self.parent_path + 'training/'

		if id == '':
			return 0, 0, 0, 0
		else: 
			_tempid = id
			if self.with_format(id):
				_tempid = id.split('.')[0]
			_images_format = '.jpg'
			_instances_format = '.png'
			_labels_format = '.png'
			_panoptic_format = '.png'
			_image = cv2.imread(_trainingpath + 'images/' +  _tempid + _images_format)
			_instance = cv2.imread(_trainingpath + 'instances/' + _tempid + _instances_format)
			_label = cv2.imread(_trainingpath + 'labels/' + _tempid + _labels_format)
			_panoptic = cv2.imread(_trainingpath + 'panoptic/' + _tempid + _panoptic_format)

			return _image, _instance, _label, _panoptic
	
	def get_testing_by_id(self, id = ''):
		_testingpath = self.testing_path
		if _testingpath == '':
			_testingpath = self.parent_path + 'testing/'
		if id == '':
			return 0, 0, 0, 0
		else: 
			_tempid = id
			if self.with_format(id):
				_tempid = id.split('.')[0]
			_images_format = '.jpg'
			_image = cv2.imread(_testingpath + 'images/' +  _tempid + _images_format)
		return _image

	def get_validation_by_id(self, id = ''):
		_validationpath = self.validation_path
		if _validationpath == '':
			_validationpath = self.parent_path + 'validation/'
		if id == '':
			return 0, 0, 0, 0
		else: 
			_tempid = id
			if self.with_format(id):
				_tempid = id.split('.')[0]
			_images_format = '.jpg'
			_instances_format = '.png'
			_labels_format = '.png'
			_panoptic_format = '.png'
			_image = cv2.imread(_validationpath + 'images/' +  _tempid + _images_format)
			_instance = cv2.imread(_validationpath + 'instances/' + _tempid + _instances_format)
			_label = cv2.imread(_validationpath + 'labels/' + _tempid + _labels_format)
			_panoptic = cv2.imread(_validationpath + 'panoptic/' + _tempid + _panoptic_format)
			return _image, _instance, _label, _panoptic
	

	def show_training(self, id = ''):
		_image, _instance, _label, _panoptic = self.get_training_by_id(id)
		cv2.namedWindow('Training Image',cv2.WINDOW_NORMAL)
		cv2.namedWindow('Training Instance',cv2.WINDOW_NORMAL)
		cv2.namedWindow('Training Label',cv2.WINDOW_NORMAL)
		cv2.namedWindow('Training Panoptic',cv2.WINDOW_NORMAL)
		cv2.imshow('Training Image', _image)
		cv2.imshow('Training Instance', _instance)
		cv2.imshow('Training Label', _label)
		cv2.imshow('Training Panoptic', _panoptic)
		while(1):
			k = cv2.waitKey(100)
			if k == 27:
				break
			else:
				continue

	def show_validation(self, id = ''):
		_image, _instance, _label, _panoptic = self.get_validation_by_id(id)
		cv2.namedWindow('Training Image',cv2.WINDOW_NORMAL)
		cv2.namedWindow('Training Instance',cv2.WINDOW_NORMAL)
		cv2.namedWindow('Training Label',cv2.WINDOW_NORMAL)
		cv2.namedWindow('Training Panoptic',cv2.WINDOW_NORMAL)
		cv2.imshow('Training Image', _image)
		cv2.imshow('Training Instance', _instance)
		cv2.imshow('Training Label', _label)
		cv2.imshow('Training Panoptic', _panoptic)
		while(1):
			k = cv2.waitKey(100)
			if k == 27:
				break
			else:
				continue
	def show_testing(self, id = ''):
		_image = self.get_testing_by_id(id)
		cv2.namedWindow('Testing Image',cv2.WINDOW_NORMAL)
		cv2.imshow('Testing Image', _image)
		while(1):
			k = cv2.waitKey(100)
			if k == 27:
				break
			else:
				continue

def main():
	myobj = DataLoaderMapillary()
	myobj.set_parent_path('/home/prismadynamics/Desktop/machine_learning/datasets/mapillary/')
	myobj.set_training_path(myobj.parent_path + 'training/')
	myobj.show_training(id = '_0A_W6lEi-7W0RvVEiKkyQ.asdfa')

if __name__ == "__main__":
	main()