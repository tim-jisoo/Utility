import numpy as np
import cv2
import random
import sys

if len(sys.argv) != 3:
	print("usage : python3 src dst\n")
	termination
	
for i in range(10):
	orig_path = sys.argv[1] + str(i) + ".jpg"
	orig = cv2.imread(orig_path)
	print("image : [%s] is loaded...\n" % orig_path)
	
	dummy, invs = cv2.threshold(cv2.resize(cv2.cvtColor(orig, cv2.COLOR_BGR2GRAY),(28,28)),	
							0xFF>>1, 0xFF, cv2.THRESH_BINARY_INV)

	height, width = invs.shape
	cv2.imshow("original", invs)
	cv2.moveWindow("original", 0, 0)

	for j in range(1000):
		tgt = invs
		#blur distortion
		mode = random.randrange(0,3)
		if mode == 0:		
			ksize = (random.randrange(1,3) * 2 + 1, random.randrange(1,3) * 2 + 1)
			tgt = cv2.blur(tgt, ksize)
		elif mode == 1:
			ksize = (random.randrange(1,3) * 2 + 1, random.randrange(1,3) * 2 + 1)
			tgt = cv2.GaussianBlur(tgt, ksize, 0)
		else:
			msize = random.randrange(1, 3) * 2 + 1
			tgt = cv2.medianBlur(tgt, msize)

		#rotate distortion
		angle = random.uniform(-5.0, 5.0) 
		Matrix = cv2.getRotationMatrix2D((width>>1, height>>1), angle, 1)
		tgt = cv2.warpAffine(tgt, Matrix, (width, height))

		#shift distortion	
		if random.randrange(0, 2) == 0:
			direction = 1
		else:
			direction = -1
		dx = random.randrange(0, width >> 3)
		dy = random.randrange(0, height >> 3)
		Matrix = np.float32([[1, 0, direction * dx], [0, 1, direction * dy]])
		tgt = cv2.warpAffine(tgt, Matrix, (width, height))
	
		#scale distortion
		factor_w = random.uniform(0.25, 1.0)
		factor_h = random.uniform(0.25, 1.0)
		tgt = cv2.resize(cv2.resize(tgt, None, fx=factor_w, fy=factor_h), (width, height))		

		#save distortion image
		#dummy, tgt = cv2.threshold(tgt, 0xFF>>1, 0xFF, cv2.THRESH_BINARY)	
		cv2.imshow("tgt", tgt)
		cv2.moveWindow("tgt",0,300)
		cv2.waitKey(5)
		cv2.imwrite(sys.argv[2] + str(i) + "/" + str(j) + ".bmp", tgt)
