import numpy as np
import cv2
import random

orig  = cv2.imread("../image/kor.jpg")
height, width, channel = orig.shape

dummy, orig = cv2.threshold(cv2.cvtColor(orig, cv2.COLOR_BGR2GRAY), 0xFF >> 1, 0xFF, cv2.THRESH_BINARY_INV)

cv2.imshow("original", orig)
cv2.moveWindow("original", 0,0)

for i in range(0, 50):
	
	if random.randrange(0, 2) == 0:
		direction = 1
	else:
		direction = -1

	dx = random.randrange(0, width >> 3)

	dy = random.randrange(0, height >> 3)

	Matrix = np.float32([[1, 0, direction * dx], [0, 1, direction * dy]])
	shift = cv2.warpAffine(orig, Matrix, (width, height))

	cv2.imshow("shift", shift)
	cv2.moveWindow("shift", 0, 300)
	cv2.waitKey(50)


