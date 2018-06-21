import cv2
import random

orig  = cv2.imread("../image/kor.jpg")
height, width, channel = orig.shape

dummy, orig = cv2.threshold(cv2.cvtColor(orig, cv2.COLOR_BGR2GRAY), 0xFF >> 1, 0xFF, cv2.THRESH_BINARY_INV)

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
	cv2.moveWindow("blur", 0, 300);

	dummy, bnr = cv2.threshold(blur, 0xFF >> 1, 0xFF, cv2.THRESH_BINARY)
	cv2.imshow("bnr", bnr)
	cv2.moveWindow("bnr", 0, 600);

	cv2.waitKey(50)
