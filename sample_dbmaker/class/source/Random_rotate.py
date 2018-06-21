import cv2
import random

orig  = cv2.imread("../image/kor.jpg")
height, width, channel = orig.shape

dummy, orig = cv2.threshold(cv2.cvtColor(orig, cv2.COLOR_BGR2GRAY), 0xFF >> 1, 0xFF, cv2.THRESH_BINARY_INV)

cv2.imshow("original", orig)
cv2.moveWindow("original", 0,0)

str_base = "../result/DB/"

for i in range(0, 50):
	angle = random.uniform(-5.0, 5.0) 
	Matrix = cv2.getRotationMatrix2D((width>>1, height>>1), angle, 1)
	rotate = cv2.warpAffine(orig, Matrix, (width, height))
	cv2.imshow("rotate", rotate)
	cv2.moveWindow("rotate", 0, 300)

	str_file = str(i) + ".bmp"
	str_path = str_base + str_file
	cv2.imwrite(str_path, rotate)

	cv2.waitKey(50)


