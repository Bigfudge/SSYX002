import numpy as np
import cv2
import time

lower_limit = np.array([90, 100, 100])
upper_limit = np.array([130, 255, 255])

def find_marker(frame):
    # Convert image from RBG/BGR to HSV so we easily filter
    hsv_img = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Use inRange to capture only the values between lower & upper_blue
    mask = cv2.inRange(hsv_img, lower_limit, upper_limit)

    # Perform Bitwise AND on mask and our original frame
    res = cv2.bitwise_and(frame, frame, mask=mask)

    gray = cv2.cvtColor(res, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (5, 5), 0)
    edged = cv2.Canny(gray, 35, 125)
    (ret, cnts, _) = cv2.findContours(edged.copy(), cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
    c = max(cnts, key = cv2.contourArea)


    # compute the bounding box of the of the paper region and return it
    return cv2.minAreaRect(c)

def distance_to_camera(knownWidth, focalLength, perWidth):
	# compute and return the distance from the maker to the camera
	return (knownWidth * focalLength) / perWidth

# initialize the known distance from the camera to the object, which
# in this case is 24 inches
KNOWN_DISTANCE = 1

# initialize the known object width, which in this case, the piece of
# paper is 12 inches wide
KNOWN_WIDTH = 0.08

image = cv2.imread("test.jpg")
marker = find_marker(image)
focalLength = (marker[1][0] * KNOWN_DISTANCE) / KNOWN_WIDTH
cap = cv2.VideoCapture(1)
print(focalLength)

cap.release()
cv2.destroyAllWindows()