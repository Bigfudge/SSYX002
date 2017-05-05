#!/usr/bin/env python
# coding=utf-8
# license removed for brevity
import numpy as np
import argparse
import cv2
import copy
import matplotlib.pyplot as plt
import math
import rospy
from std_msgs.msg import Float32


cap = cv2.VideoCapture("line.mp4") #change to cap = cv2.VideoCapture(0) for the camera instead
win = 0
fail = 0

# intial Kalman parameters
Q = 1e-5  # process variance Lutning(Lägre värde = prediktion av stor vikt | Högre värde = mätning av stor vikt)
#Default value: 1e-5
Qm = 1e-5  # process variance for m (Lägre värde = prediktion av stor vikt | Högre värde = mätning av stor vikt)

# initialize lists for "lutning"
xhat = []#np.zeros(sz)  # a posteri estimate of x
P = [] #np.zeros(sz)  # a posteri error estimate
xhatminus = [] #np.zeros(sz)  # a priori estimate of x
Pminus = [] #np.zeros(sz)  # a priori error estimate
K = [] #np.zeros(sz)  # gain or blending factor

# initialize lists for "m"
mhat = [] #np.zeros(sz)  # a posteri estimate of x
Pm = [] #np.zeros(sz)  # a posteri error estimate
mhatminus = [] #np.zeros(sz)  # a priori estimate of x
Pmminus = [] #np.zeros(sz)  # a priori error estimate
Km = [] #np.zeros(sz)  # gain or blending factor

R = 0.1 ** 3  # estimate of measurement variance, change to see effect
#Default value: 0.1 ** 2
Rm = 0.1 ** 3  # estimate of measurement variance, change to see effect (m)

# intial guesses
xhat.insert(0, 0.1)
P.insert(0, 1.0)
mhat.insert(0, 25)
Pm.insert(0, 1.0)
# End of Initial Kalman
k = 1

#Initiating ROS-stuff
dist_Pub = rospy.Publisher('distance', Float32, queue_size=10)
rot_Pub = rospy.Publisher('rotationDeviation', Float32, queue_size=10)
rospy.init_node('positionLinje', anonymous=True)
rate = rospy.Rate(10) # 10hz


while cap.isOpened() :
    ret, frame = cap.read()
    frame = cv2.resize(frame, None, fx=0.25, fy=0.25, interpolation=cv2.INTER_LINEAR)
    height, width = frame.shape[:2]

    # cropping
    start_row, start_col = int(height * .4), int(width * .5)
    end_row, end_col = int(height), int(width)
    cropped = frame[start_row:end_row, start_col:end_col]
    w, h = cropped.shape[:2]

    #Dilation/Closing kernel
    kernel = np.ones((5,5), np.uint8)

    # Defining the list of boundaries
    boundaries = [
        # ([0, 100, 200], [7, 160, 255]) orange
        ([45, 40, 160], [95, 70, 200])  #Red
    ]

    # loop over the boundaries
    for (lower, upper) in boundaries:
        # create NumPy arrays from the boundaries
        lower = np.array(lower, dtype="uint8")
        upper = np.array(upper, dtype="uint8")

        # find the colors within the specified boundaries and apply
        # the mask
        mask = cv2.inRange(frame, lower, upper)
        # output = cv2.bitwise_and(image, image, mask = mask)
        #cv2.imshow('bild', mask)
        dillad = cv2.dilate(mask, kernel, iterations=1)
        closing = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
        #cv2.imshow('dilated', dillad)
        #cv2.imshow('closed', closing)
        # cv2.waitKey(0)
        # cv2.destroyAllWindows()

        #Find all lines in the picture
        cv2.imshow('mask', mask)
        lines = cv2.HoughLinesP(closing, cv2.HOUGH_PROBABILISTIC, np.pi / 180, threshold=20, minLineLength=1, maxLineGap=50)

    if lines is None:
        lines = cv2.HoughLinesP(dillad, cv2.HOUGH_PROBABILISTIC, np.pi / 180, threshold=20, minLineLength=1, maxLineGap=50)

        #Find the correct line
    if lines is not None:
        for line in lines:
            for x1, y1, x2, y2 in line:
                pts = np.array([[x1, y1], [x2, y2]], np.int32)
                points = np.vstack([pts])
        vx, vy, x0, y0 = cv2.fitLine(np.float32(points), cv2.DIST_L2, 0, 0.01, 0.01)
        #cv2.line(frame, (int(x0 - vx * w), int(y0 - vy * w)), (int(x0 + vx * w), int(y0 + vy * w)),(0, 0, 255), 1) #Alla linjer typ (RÖD)
        lutning = (vy/vx)
        m = (y0 - x0 * lutning)
        cv2.line(frame, (20, lutning*20+m), (300, lutning*300+m),(255,0,0),2) #Alla fitLines (BLÅ)
        win = win + 1

        # Kalman: time update
        xhatminus.insert(0, xhat[0])
        Pminus.insert(0, P[0] + Q)
        mhatminus.insert(0, mhat[0])
        Pmminus.insert(0, Pm[0] + Qm)

        # measurement update
        K.insert(0, Pminus[0] / (Pminus[0] + R))
        xhat.insert(0, xhatminus[0] + K[0] * (lutning - xhatminus[0]))
        P.insert(0, (1 - K[0]) * Pminus[0])
        Km.insert(0, Pmminus[0] / (Pmminus[0] + Rm))
        mhat.insert(0, mhatminus[0] + Km[0] * (m - mhatminus[0]))
        Pm.insert(0, (1 - Km[0]) * Pmminus[0])
        cv2.line(frame, (20, int(xhat[0] * 20 + mhat[0])), (300, int(xhat[0] * 300 + mhat[0])),(0,255,0),2)
        #cv2.imshow('fotot', cropped)
        k=0
    else:
        if k < 20:
            cv2.line(frame, (20, int(xhat[0] * 20 + mhat[0])), (300, int(xhat[0] * 300 + mhat[0])), (0, 255, 0), 2)
            #cv2.imshow('fotot', cropped)
        k = k+1
        if k > 20:
            print ('Error occurred')
            
    cv2.imshow('full', frame)

    #Output:
    fakA = 50 #Avståndsfaktor
    fakKurs = 8 #Önskad lutning på bandet
    distance = float(mhat[0]/fakA)
    rotationDeviation =float(fakKurs - math.degrees(math.tan(xhat[0])))
    #print('Distance from line: ', distance, ' meter. | RotationDeviation: ', rotationDeviation )


    dist_Pub.publish(distance)
    rot_Pub.publish(rotationDeviation)
    rate.sleep()

    if cv2.waitKey(1) == 13:  # 13 is the Enter Key
        break
    
#print(win*100/(win+fail))
cap.release()
cv2.destroyAllWindows()
