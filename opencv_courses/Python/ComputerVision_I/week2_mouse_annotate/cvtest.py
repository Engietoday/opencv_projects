import cv2
import numpy as np

def resizeImage(action, x, y, flag, userdata):
  # Referencing global variables 
  global scalingFactor, scalingType
  print(['Action', action])
  print(['cv2.EVENT_FLAG_CTRLKEY', cv2.EVENT_FLAG_CTRLKEY])
  print(['cv2.EVENT_MOUSEWHEEL', cv2.EVENT_MOUSEWHEEL])
  print(['flag', flag])
  ctrl_pressed = (((np.absolute(flag) - cv2.EVENT_FLAG_CTRLKEY) == 65488) or (((np.absolute(flag) - cv2.EVENT_FLAG_CTRLKEY) == 65568)))
  print(['ctrl_pressed', np.absolute(flag) - cv2.EVENT_FLAG_CTRLKEY])
  # Action to be taken when ctrl key + mouse wheel scrolled forward
  if action == (not ctrl_pressed) + cv2.EVENT_MOUSEWHEEL:
    if (flag < 0):
      print('Ctrl + Wheel Forward.')
    if (flag > 0):
    	print('Ctrl + Wheel Backward.')

image_path = './data/images/sample.jpg'
source = cv2.imread(image_path)
dummy = source.copy()
cv2.namedWindow("Window")
cv2.setMouseCallback("Window", resizeImage)
k = 0
while k!=27 :
    cv2.imshow("Window", source)
    cv2.putText(source,'Choose ceaaaaaaaaaaaaaaaaaaaaaaanter, and drag, Press ESC to exit and c to clear', (10,30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255,255,255), 2 );
    k = cv2.waitKey(20) & 0xFF
    # Another way of cloning
    if k==99:
        source= dummy.copy()

cv2.destroyAllWindows()
