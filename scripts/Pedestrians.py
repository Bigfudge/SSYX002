import cv2
import rospy
from std_msgs.msg import Bool

body_classifier = cv2.CascadeClassifier('./Haarcascades/haarcascade_lowerbody.xml')
cap = cv2.VideoCapture(0)
pub2 = rospy.Publisher('stop', Bool, queue_size=10)
rospy.init_node('pedestrianStop', anonymous=True)
rate = rospy.Rate(10) # 10hz


class Stack:
     def __init__(self):
         self.items = []
         for i in range(10):
            self.items.append(0)

     def __str__(self):
         return str(self.items)

     def isEmpty(self):
         return self.items == []

     def push(self, item):
         self.items.append(item)

     def pop(self):
         return self.items.pop(0)

     def peek(self):
         return self.items[len(self.items)-1]

     def size(self):
         return len(self.items)
     
     def sum(self):
         return sum(self.items)


count = Stack()
fourcc = cv2.VideoWriter_fourcc(*'XVID')
out = cv2.VideoWriter('output.avi', fourcc, 20.0, (640,480))

while True:
    ret, frame = cap.read()
    frame = cv2.resize(frame, None, fx=1, fy=1, interpolation=cv2.INTER_LINEAR)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    bodies = body_classifier.detectMultiScale(gray, 1.2, 4)
    if len(bodies) > 0:
        count.push(1)
        count.pop()
    else:
        count.push(0)
        count.pop()

    if count.sum() >= 3:
        pub2.publish(True)
        cv2.putText(frame, "stop", (50,50), cv2.FONT_HERSHEY_SIMPLEX, 2.0, (0,255,0),3)
    
       



    for (x, y, w, h) in bodies:
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 2)

    cv2.imshow('Pedestrians', frame)
    out.write(frame)
    # print(stop)
    
    rate.sleep()
    if cv2.waitKey(1) == 13:
        break



cap.release()
cv2.destroyAllWindows()