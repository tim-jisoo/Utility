import cv2
import random

orig  = cv2.imread("../image/kor.jpg")
height, width, channel = orig.shape

cv2.imshow("original", orig)
cv2.moveWindow("original", 0,0)

for i in range(0, 50):
	mode = random.randrange(0,3)

	if mode == 0:
		ksize = (random.randrange(1,6) * 2 + 1, random.randrange(1,6) * 2 + 1)
		blur = cv2.blur(orig, ksize)

	elif mode == 1:
		ksize = (random.randrange(1,6) * 2 + 1, random.randrange(1,6) * 2 + 1)
		blur = cv2.GaussianBlur(orig, ksize, 0)

	else:
		msize = random.randrange(1, 6) * 2 + 1
		blur = cv2.medianBlur(orig, msize)

	cv2.imshow("blur", blur)
	cv2.moveWindow("blur", width << 1, 0);
	cv2.waitKey(50)
