#!/usr/bin/env python

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function


class DataLoaderStrategy(object):

	def __init__(self):
		self.parent_path = ""
		self.training_path = ""
		self.testing_path = ""
		self.validation_path = ""
		self.data = [] # List of objects representing the data.
		self.training_data = []
		self.testing_data = []
		self.validation_data = []

	def set_parent_path(self, _parent):
		if _parent[-1] != r'/':
			_parent = _parent + r'/'
		self.parent_path = _parent

	def set_training_path(self, _training):
		if _training[-1] != r'/':
			_training = _training + r'/'
		self.training_path = _training

	def set_testing_path(self, _testing):
		if _testing[-1] != r'/':
			_testing = _testing + r'/'
		self.testing_path = _testing

	def set_validation_path(self, _valiation):
		if _valiation[-1] != r'/':
			_valiation = _valiation + r'/'
		self.validation_path = _valiation

	def get_data(self, indx = -1):
		if indx < 0:
			return self.data
		else:
			return self.data[indx]

	def get_training_data(self):
		return self.training_data

	def get_testing_data(self):
		return self.testing_data

	def get_validation_data(self):
		return self.validation_data

	def with_format(self, id = ""):
		_tempid = id.split('.')
		if len(_tempid) == 2:
			return True
		else:
			return False