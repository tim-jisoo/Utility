import cv2
import random

orig  = cv2.imread("../image/kor.jpg")
height, width, channel = orig.shape

dummy, orig = cv2.threshold(cv2.cvtColor(orig, cv2.COLOR_BGR2GRAY), 0xFF >> 1, 0xFF, cv2.THRESH_BINARY_INV)

cv2.imshow("original", orig)
cv2.moveWindow("original", 0,0)

for i in range(0, 50):
	factor_w = random.uniform(0.1, 1.0)
	factor_h = random.uniform(0.1, 1.0)
	
	scaling = cv2.resize(cv2.resize(orig, None, fx = factor_w, fy = factor_h), (width, height))
	cv2.imshow("scaling", scaling)
	cv2.moveWindow("scaling", 0, 300)

	dummy, bnr = cv2.threshold(scaling, 0xFF >> 1, 0xFF, cv2.THRESH_BINARY)
	cv2.imshow("bnr", bnr)
	cv2.moveWindow("bnr", 0, 600)
	cv2.waitKey(50)
