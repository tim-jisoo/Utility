import cv2

###################### 1. image read ########################################################################
orig  = cv2.imread("../image/lena.jpg")
print("image data type => %s" % type(orig))

###################### 2. image down-scaling ################################################################
#small = cv2.resize(orig, (250, 250) )
small = cv2.resize(orig, None, fx = 0.25, fy = 0.25)
height, width, channel = small.shape
print("h[%d], w[%d], ch[%d]" %(height, width, channel))

###################### 3. Convert color image to 1-channel gray image #######################################
gray = cv2.cvtColor(small, cv2.COLOR_BGR2GRAY)

###################### 4. image thresholding - global, adaptive(mean or gaussian) ###########################
neighbor, const = 11, 2
retval, bnr = cv2.threshold(gray, 0xFF >> 2, 0xFF, cv2.THRESH_BINARY)
Abnr = cv2.adaptiveThreshold(gray, 0xFF, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, neighbor, const )
#Abnr = cv2.adaptiveThreshold(gray, 0xFF, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, neightbor, const)
print("result of threshold [%d]" %(retval))

##################### 5. image blurring (mean, Gaussian, median) ############################################
ksize = (5,5)
msize = 5;
blur = cv2.blur(small, ksize)
#blur = cv2.GaussianBlur(small, ksize, 0)
#blur = cv2.medianBlur(small, msize)
print("kernel size type => %s" % type(ksize))

#################### 6. draw rectangle to color image  ###################################
rect = small
start = ((width >> 1) - (width >> 2), (height >> 1) - (height >> 2))
end = ((width >> 1) + (width >> 2), (height >> 1) + (height >> 2))
color = (0xFF, 0xFF, 0x00)
thickness = 2
cv2.rectangle(rect, start, end, color, thickness)

#################### 7. print image to console out. #########################################################
cv2.imshow("small", small)
cv2.moveWindow("small", 0, 0)
cv2.imshow("gray", gray)
cv2.moveWindow("gray", width, 0)
cv2.imshow("bnr", bnr)
cv2.moveWindow("bnr", width << 1, 0)
cv2.imshow("Abnr", Abnr)
cv2.moveWindow("Abnr", width * 3, 0)
cv2.imshow("blur", blur)
cv2.moveWindow("blur", 0, height);
cv2.imshow("rect", rect)
cv2.moveWindow("rect", width, height);

#################### 8. write image to file system ###################################
cv2.imwrite("../result/gray.jpg", gray)
cv2.waitKey()
#cv2.waitKey(3000)
