import cv2
import random

orig  = cv2.imread("../image/kor.jpg")
height, width, channel = orig.shape

cv2.imshow("original", orig)
cv2.moveWindow("original", 0,0)

for i in range(0, 50):
	factor_w = random.uniform(0.1, 1.0)
	factor_h = random.uniform(0.1, 1.0)
	
	restore = cv2.resize(cv2.resize(orig, None, fx = factor_w, fy = factor_h), (width, height))

	cv2.imshow("resize", restore)
	cv2.moveWindow("resize", width << 1, 0);
	cv2.waitKey(50)
