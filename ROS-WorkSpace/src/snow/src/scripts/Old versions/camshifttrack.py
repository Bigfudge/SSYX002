#!/usr/bin/env python
# coding=utf-8
import numpy as np
import cv2
import rospy
from std_msgs.msg import Float32

def distance_to_camera(knownWidth, focalLength, perWidth):
    # compute and return the distance from the maker to the camera
    return (knownWidth * focalLength) / perWidth

# Initialize webcam
cap = cv2.VideoCapture('testBlue.mp4')

# take first frame of the video
ret, frame = cap.read()

# setup default location of window
r, h, c, w = 240, 100, 400, 160
track_window = (c, r, w, h)

# Crop region of interest for tracking
roi = frame[r:r + h, c:c + w]

# Convert cropped window to HSV color space
hsv_roi = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)

# Create a mask between the HSV bounds
lower_limit = np.array([95, 100, 100])
upper_limit = np.array([130, 255, 255])
#lower_limit2 = np.array([160, 100, 100])
#upper_limit2 = np.array([179, 255, 255])

mask = cv2.inRange(hsv_roi, lower_limit, upper_limit)
#mask2 = cv2.inRange(hsv_roi, lower_limit2, upper_limit2)
#mask = mask1 + mask2

# Obtain the color histogram of the ROI
roi_hist = cv2.calcHist([hsv_roi], [0], mask, [180], [0, 180])

# Normalize values to lie between the range 0, 255
cv2.normalize(roi_hist, roi_hist, 0, 255, cv2.NORM_MINMAX)

# Setup the termination criteria
# We stop calculating the centroid shift after ten iterations
# or if the centroid has moved at least 1 pixel
term_crit = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1)

KNOWN_DISTANCE = 1.69
KNOWN_WIDTH = 0.30
FOCAL_LENGTH = 884.4333
ANGLE_OF_VIEW = 78
PIXELS = (640 ** 2 + 480 ** 2) ** 0.5
PIXEL_DEGREES = ANGLE_OF_VIEW / PIXELS
CENTER_SCREEN_X = 640 / 2
CENTER_SCREEN_Y = 480 / 2

dist_Pub = rospy.Publisher('dist_camshifttrack', Float32, queue_size=10)
ang_Pub = rospy.Publisher('ang__camshifttrack', Float32, queue_size=10)
rospy.init_node('camshifttrack', anonymous=True)
rate = rospy.Rate(10) # 10hz


while True:

    # Read webcam frame
    ret, frame = cap.read()

    if ret == True:
        # Convert to HSV
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        # Calculate the histogram back projection
        # Each pixel's value is it's probability
        dst = cv2.calcBackProject([hsv], [0], roi_hist, [0, 180], 1)

        # apply Camshift to get the new location
        ret, track_window = cv2.CamShift(dst, track_window, term_crit)

        # Draw it on image
        # We use polylines to represent Adaptive box
        pts = cv2.boxPoints(ret)
        pts = np.int0(pts)
        img2 = cv2.polylines(frame, [pts], True, 255, 2)


        distance = distance_to_camera(KNOWN_WIDTH, FOCAL_LENGTH, track_window[2])
        x_center = track_window[0] + track_window[2] / 2
        y_center = track_window[1] + track_window[3] / 2
        center_distance = ((CENTER_SCREEN_X - x_center) ** 2 + (CENTER_SCREEN_Y - y_center) ** 2) ** 0.5
        angle = center_distance * PIXEL_DEGREES
        print('Distance: ' + str(distance) + '\n'+ 'Angle: ' + str(angle))

   

        dist_Pub.publish(float(distance))
        ang_Pub.publish(float(angle))
        rate.sleep()

        cv2.imshow('Camshift Tracking', img2)

        if cv2.waitKey(1) == 13:  # 13 is the Enter Key
            break

    else:
        break

cv2.destroyAllWindows()
cap.release()