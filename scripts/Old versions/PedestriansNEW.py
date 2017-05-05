#!/usr/bin/env python
# coding=utf-8
import numpy as np
import cv2
import rospy
from std_msgs.msg import Float32

body_classifier = cv2.CascadeClassifier('Haarcascades\haarcascade_upperbody.xml')
cap = cv2.VideoCapture('testBlue.mp4')

while True:

    ret, frame = cap.read()
    frame = cv2.resize(frame, None, fx=0.5, fy=0.5, interpolation=cv2.INTER_LINEAR)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    bodies = body_classifier.detectMultiScale(gray, 1.2, 2)
    if len(bodies) > 0:
        stop = True
    else:
        stop = False

    for (x, y, w, h) in bodies:
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 2)

    cv2.imshow('Pedestrians', frame)
    print(stop)
    if cv2.waitKey(1) == 13:
        break

cap.release()
cv2.destroyAllWindows()