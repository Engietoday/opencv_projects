import cv2 as cv
import numpy as np

class BlemishRemover:
	def __init__(self, windowname, src, patch_radius=15):
		self.windowname = windowname
		self.src = src
		self.current_image = src.copy()
		self.patch_radius = patch_radius
		self.clicked_points = (0, 0)

		self.show()
		cv.setMouseCallback(self.windowname, self.on_mouse)
		cv.createTrackbar('Patch Size', self.windowname, 15, 30, self.on_trackbar)

	def on_trackbar(self, *args):
		self.patch_radius = args[0]

	def set_patch_radius(self, r):
		self.patch_radius = r

	def show(self):
		cv.imshow(self.windowname, self.current_image)

	def on_mouse(self, event, x, y, flags, param):
		pt = (x, y)
		if event == cv.EVENT_LBUTTONDOWN:
			pass
		elif event == cv.EVENT_LBUTTONUP:
			self.clicked_points = pt
			self.remove_blemish()

	def remove_blemish(self):
		surround_patch = []
		h = self.clicked_points[1]
		w = self.clicked_points[0]
		r = self.patch_radius

		patch = self.get_patch(self.current_image, w, h)
		surround_patch.append(self.get_patch(self.current_image, w-2*r, h)) # This gets the left side patch
		surround_patch.append(self.get_patch(self.current_image, w+2*r, h)) # This gets the right side patch
		surround_patch.append(self.get_patch(self.current_image, w, h+2*r)) # This gets the upper side patch
		surround_patch.append(self.get_patch(self.current_image, w, h-2*r)) # This gets the lower side patch
		# The best patch will be selected based on the lowest gradient norm average.
		lowest_grad_indx = 0
		lowest_gradient = float('inf')
		for i in range(0, len(surround_patch)):
			gradient_x_avg, gradient_y_avg = self.gradient_means(surround_patch[i])
			_temp_gradient_avg = np.sqrt(gradient_x_avg*gradient_x_avg + gradient_y_avg*gradient_y_avg)
			if(_temp_gradient_avg < lowest_gradient):
				lowest_grad_indx = i
				lowest_gradient = _temp_gradient_avg
		#self.current_image[h-r:h+r, w-r:w+r] = 0
		#self.current_image[h-r:h+r, w-r:w+r] = surround_patch[lowest_grad_indx]
		# Create an all white mask
		mask = 255*np.ones(surround_patch[lowest_grad_indx].shape, surround_patch[lowest_grad_indx].dtype)
		self.current_image = cv.seamlessClone(surround_patch[lowest_grad_indx], self.current_image, mask, (w, h), cv.NORMAL_CLONE)

		self.show()

	def gradient_means(self, img, blur=False, render=True):
		_temp_img = img.copy()
		if blur:
			_temp_img = cv.GaussianBlur(_temp_img,(3,3),0,0)
		else:
			pass
		# Apply sobel filter along x direction
		sobelx = cv.Sobel(_temp_img, cv.CV_32F, 1, 0)
		# Apply sobel filter along y direction
		sobely = cv.Sobel(_temp_img,cv.CV_32F,0,1)
		if render:
			cv.normalize(sobelx,
			                dst = sobelx, 
			                alpha = 0, 
			                beta = 1, 
			                norm_type = cv.NORM_MINMAX, 
			                dtype = cv.CV_32F)
			cv.normalize(sobely, 
			                dst = sobely, 
			                alpha = 0, 
			                beta = 1, 
			                norm_type = cv.NORM_MINMAX, 
			                dtype = cv.CV_32F)
		avg_x = np.average(np.absolute(sobelx))
		avg_y = np.average(np.absolute(sobelx))
		return avg_x, avg_y



	def run(self):
		while True:
			ch = cv.waitKey(50)
			if ch == 27:
				cv.destroyAllWindows()
				break
			if ch == ord('t'):
				h = self.clicked_points[1]
				w = self.clicked_points[0]
				patch = self.get_patch(self.current_image, w, h)
				cv.imshow('Selected Patch', patch)
			if ch == ord('r'):
				self.current_image = src.copy()
			if ch == ord('s'):
				self.save_image()
				break

	def get_patch(self, img, x_c, y_c):
		r = self.patch_radius
		ret = img[y_c-r:y_c+r, x_c-r:x_c+r]
		return ret

	def save_image(self):
		cv.imwrite('./blemish_removed.png', self.current_image)
		print('Image saved in: ./blemish_removed.png')


def main():
	img = cv.imread('./blemish.png', cv.IMREAD_COLOR)
	blemishremoval = BlemishRemover('image', img)
	blemishremoval.set_patch_radius(10)
	blemishremoval.run()

if __name__ == "__main__":
	main()