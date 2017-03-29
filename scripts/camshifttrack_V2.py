#!/usr/bin/env python
# coding=utf-8
import numpy as np
import cv2
import rospy
from std_msgs.msg import Float64

def distance_to_camera(knownWidth, focalLength, perWidth):
    # compute and return the distance from the maker to the camera
    return (knownWidth * focalLength) / perWidth


cap = cv2.VideoCapture(0)
# take first frame of the video
ret,frame = cap.read()
# setup initial location of window
r,h,c,w = 0,480,0,640  # simply hardcoded the values
track_window = (c,r,w,h)
# set up the ROI for tracking

hsv_roi =  cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
mask = cv2.inRange(hsv_roi, np.array((95., 100.,100.)), np.array((130.,255.,255.)))
roi = frame[r:r+h, c:c+w]
roi_hist = cv2.calcHist([hsv_roi],[0],mask,[180],[0,180])
print(roi_hist)
cv2.normalize(roi_hist,roi_hist,0,255,cv2.NORM_MINMAX)
# Setup the termination criteria, either 10 iteration or move by atleast 1 pt
term_crit = ( cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1 )

KNOWN_DISTANCE = 1
KNOWN_HEIGHT = 0.21
FOCAL_LENGTH = 700
ANGLE_OF_VIEW = 68
PIXELS = (640 ** 2 + 480 ** 2) ** 0.5
PIXEL_DEGREES = ANGLE_OF_VIEW / PIXELS
CENTER_SCREEN_X = 640 / 2
CENTER_SCREEN_Y = 480 / 2

dist_Pub = rospy.Publisher('dist_camshifttrack', Float64, queue_size=10)
rospy.init_node('camshifttrack', anonymous=True)
rate = rospy.Rate(10) # 10hz

while(1):
    ret ,frame = cap.read()
    if ret == True:
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        dst = cv2.calcBackProject([hsv],[0],roi_hist,[0,180],1)
        # apply meanshift to get the new location
        ret, track_window = cv2.CamShift(dst, track_window, term_crit)
        # Draw it on image
        pts = cv2.boxPoints(ret)
        pts = np.int0(pts)
        img2 = cv2.polylines(frame,[pts],True, 255,2)
        cv2.imshow('img2',img2)

        distance = distance_to_camera(KNOWN_HEIGHT, FOCAL_LENGTH, track_window[3])
        x_center = track_window[0] + track_window[2] / 2
        y_center = track_window[1] + track_window[3] / 2
        center_distance = ((CENTER_SCREEN_X - x_center) ** 2 + (CENTER_SCREEN_Y - y_center) ** 2) ** 0.5
        angle = center_distance * PIXEL_DEGREES
        print('Distance: ' + str(distance) + '\n'+ 'Angle: ' + str(angle))
        print(PIXEL_DEGREES)
        D=distance / np.tan(angle)

        dist_Pub.publish(D)
        rate.sleep()


        if cv2.waitKey(1) == 13:  # 13 is the Enter Key
            break

    else:
            break

cv2.destroyAllWindows()
cap.release()