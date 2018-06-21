import numpy as np
import cv2
import random

orig  = cv2.imread("../image/kor.jpg")
height, width, channel = orig.shape

cv2.imshow("original", orig)
cv2.moveWindow("original", 0,0)

Matrix = np.float32([[1, 0, 100], [0, 1, 50]])
shift = cv2.warpAffine(orig, M, (width, height))

cv2.imshow("shift", shift)
cv2.waitKey()


