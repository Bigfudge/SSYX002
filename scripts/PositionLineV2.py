#!/usr/bin/env python
#coding=utf-8
#
# license removed for brevity
import numpy as np
import argparse
import cv2
import copy
import matplotlib.pyplot as plt
import math
import rospy
from std_msgs.msg import Float64

cap = cv2.VideoCapture(0) #change to cap = cv2.VideoCapture(0) for the camera instead (file: cv2.VideoCapture('video/30-rak.mp4'))
win = 0
fail = 0

# intial Kalman parameters
#Process noise covariance
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

#Measurement noise covariance
R = 0.1 ** 3  # estimate of measurement variance, change to see effect
#Default value: 0.1 ** 2
Rm = 0.1 ** 3  # estimate of measurement variance, change to see effect (m)

# intial guesses
xhat.insert(0, 0.1)
P.insert(0, 1.0)
mhat.insert(0, 25)
Pm.insert(0, 1.0)
# End of Initial Kalman

# Calc-initial
mcalc = 0
count = 0
xcalc = 0
# Calc-End

#Initiating ROS-stuff
dist_Pub = rospy.Publisher('distance', Float64, queue_size=10)
rot_Pub = rospy.Publisher('rotationDeviation', Float64, queue_size=10)
rospy.init_node('positionLinje', anonymous=True)
rate = rospy.Rate(10) # 10hz
#End of ROS-stuff

k = 1
while cap.isOpened():
    ret, frame = cap.read()
    frame = cv2.resize(frame, None, fx=0.5, fy=0.5, interpolation=cv2.INTER_LINEAR)
    height, width = frame.shape[:2]

    # cropping
    start_row, start_col = int(height * .7), int(width * .5)
    end_row, end_col = int(height), int(width)
    cropped = frame[start_row:end_row, start_col:end_col]
    w, h = cropped.shape[:2]

    #Dilation/Closing kernel
    kernel = np.ones((5,5), np.uint8)

    # Defining the list of boundaries
    boundaries = [
        # ([0, 100, 200], [7, 160, 255]) orange
        #([45, 40, 160], [95, 70, 200])  #Red
        ([4, 14, 120], [95, 85, 200])  # RedExtended
    ]

    # loop over the boundaries
    for (lower, upper) in boundaries:
        # create NumPy arrays from the boundaries
        lower = np.array(lower, dtype="uint8")
        upper = np.array(upper, dtype="uint8")

        # find the colors within the specified boundaries and apply
        # the mask
        mask = cv2.inRange(frame, lower, upper)
        cv2.imshow('red', mask)
        # output = cv2.bitwise_and(image, image, mask = mask)
        #cv2.imshow('bild', mask)
        dillad = cv2.dilate(mask, kernel, iterations=1)
        closing = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
        #cv2.imshow('dilated', dillad)
        #cv2.imshow('closed', closing)
        # cv2.waitKey(0)
        #jAG LUCKTAR BAJS


        # cv2.destroyAllWindows()

    #Find all lines in the picture
    lines = cv2.HoughLinesP(closing, cv2.HOUGH_PROBABILISTIC, np.pi / 180, threshold=20, minLineLength=1, maxLineGap=50)

    if lines is None:
        lines = cv2.HoughLinesP(dillad, cv2.HOUGH_PROBABILISTIC, np.pi / 180, threshold=20, minLineLength=1,
                                maxLineGap=50)

    #Find the correct line
    if lines is not None:
        for line in lines:
            for x1, y1, x2, y2 in line:
                pts = np.array([[x1, y1], [x2, y2]], np.int32)
                points = np.vstack([pts])
        vx, vy, x0, y0 = cv2.fitLine(np.float32(points), cv2.DIST_L2, 0, 0.01, 0.01)
        cv2.line(frame, (int(x0 - vx * width), int(y0 - vy * width)), (int(x0 + vx * width), int(y0 + vy * width)),(0, 0, 255), 1) #Alla linjer typ (RÖD)
        lutning = (vy/vx)
        #print('vx: ', vx, 'vy: ', vy, 'x0: ', x0, 'y0: ', y0)
        #m = (y0 - x0 * lutning) #Denna uträkning kan behöva ses över! Nu kollar den vart linjen slutar i vänstra kant, men högra hade eventuellt varit bättre? ev. y0 + (width-x0)*lutning
        m = (y0 + (width - x0) * lutning)
        #print('m: ',m, ' + lutning: ', lutning, 'punkt1: ',int(lutning*20+m), 'punkt2: ',int(lutning*300+m), 'width: ', width, 'height: ', height)
        #cv2.line(frame, (width, int(m)), (0, int(m-lutning*width)),(255,0,0),2) #Alla fitLines (BLÅ)
        win = win + 1

        # Kalman: time updateD
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
        cv2.line(frame, (width, int(mhat[0])), (0, int(mhat[0]-xhat[0]*width)),(0,255,0),2) #Rätta linjen
        #cv2.imshow('fotot', cropped)
        k=0
    else:
        if k < 20:
            cv2.line(frame, (20, int(xhat[0] * 20 + mhat[0])), (width, int(xhat[0] * width + mhat[0])), (0, 255, 0), 2)
            #cv2.imshow('fotot', cropped)
        k = k+1
        if k > 20:
            print ('Error occurred')
            #break #Bör eventuellt kommenteras bort vid reell körning.


    #Output: (Optimerad för 30cm från bandet)
    xCorrect = 2.0452 #Önskat värde av xhat[0] vid 30 cm avstånd
    distance = -0.331*mhat[0]+98
    xhatModify = ((xCorrect - (-0.019321*distance+2.6249)) + xhat[0]) #Vinkeln omräknad med avseende på avståndet. Formel: (Önskad vinkel vid 30 cm - Önskad vinkel vid specifikt avstånd) + nuvarande mätvärde för vinkeln
    #print('Rätt: ', -0.01603*distance+2.83285,'xCorrect: ', xCorrect, 'xhat[0]: ', xhat[0], 'xhatModify: ', xhatModify)
    if xhatModify<2.0452:
        vinkel = 16*math.log(2*math.pow(xhatModify,2))-33.9862
    else:
         vinkel = 10*math.log(math.pow(xhatModify,2))-15.7409#14,25*xhatModify-29.15 #100/math.exp(xhatModify)-9.7 #Vinkeluträkning
    print('Avstånd från band: ', distance, ' centimeter. | Grader från optimal kurs: ', vinkel, ' grader. (pos = pekar bort från bandet, neg = pekar mot bandet)')

    vink = "%.2f" % vinkel
    dist = "%.2f" % distance #"{:.5f}".format(distance)
    #cv2.putText(frame, ("Avstånd: ", distance, "cm. Vinkelställning: ", vinkel, "grader"), (20, 250), cv2.FONT_HERSHEY_SIMPLEX, 2, (0,255,0), 2)
    cv2.putText(frame, ("Distance: " + dist +"cm."), (20, 210), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)
    cv2.putText(frame, ("Angular position: " + vink + "degrees."), (20, 230), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)
    cv2.imshow('full', frame)
    radianer = vinkel*math.pi/180

    dist_Pub.publish((distance-40)/100)
    rot_Pub.publish(radianer)
    rate.sleep()

    mcalc = mcalc + mhat[0]
    count = count + 1
    xcalc = xcalc + xhat[0]

    if cv2.waitKey(1) == 13:  # 13 is the Enter Key ; Ett annat krav för avbrott kan vara rekommenderat  vid reell körning.
        print('mTot: ',mcalc/count,'xTot: ',xcalc/count)
        break
#print(win*100/(win+fail), 'procent funnet!')
cap.release()
cv2.destroyAllWindows()
