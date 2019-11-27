from __future__ import print_function

# Import libraries
import cv2
import numpy as np
#import matplotlib.pyplot as plt

def saveROI(src, p1, p2, path):
	y = np.sort(np.array([p1[0], p2[0]]))
	x = np.sort(np.array([p1[1], p2[1]]))
	_tempROI = src[x[0]:x[1], y[0]:y[1],:]
	if path != '':
		cv2.imwrite(path, _tempROI)
	else:
		print('Empty Path!')

def drawRectangle(action, x, y, flags, userdata):
	path = "./data/images/face.png";
	global anchor_1, anchor_2
	if action == cv2.EVENT_LBUTTONDOWN:
		anchor_1 = (x,y)
		cv2.circle(source, anchor_1, 1, (255,255,0), 2, cv2.LINE_AA );

	elif action == cv2.EVENT_LBUTTONUP:
		anchor_2 = (x,y)
		if anchor_1 == anchor_2:
			pass
		else:
			cv2.rectangle(source, anchor_1, anchor_2, (255, 255, 0), 3, cv2.LINE_AA)
			cv2.imshow("Window", source)
			saveROI(source, anchor_1, anchor_2, path)
			print('Image saved in: ' + path)

image_path = './data/images/sample.jpg'
source = cv2.imread(image_path)
dummy = source.copy()
cv2.namedWindow("Window")
cv2.setMouseCallback("Window", drawRectangle)
k = 0
while k!=27 :
    cv2.imshow("Window", source)
    cv2.putText(source,'Choose center, and drag, Press ESC to exit and c to clear', (10,30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255,255,255), 2 );
    k = cv2.waitKey(20) & 0xFF
    # Another way of cloning
    if k==99:
        source= dummy.copy()

cv2.destroyAllWindows()
