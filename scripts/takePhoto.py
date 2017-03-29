#!/usr/bin/env python
# coding=utf-8
import numpy as np
import cv2

cap = cv2.VideoCapture(0)
# take first frame of the video
ret,frame = cap.read()

cv2.imwrite("test.jpg", frame)

cv2.destroyAllWindows()
cap.release()
