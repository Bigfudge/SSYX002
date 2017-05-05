#!/usr/bin/env python
# 

import rospy
from std_msgs.msg import Float64

def callback_V(data):
    rospy.loginfo('Vanster motor:  %f', data.data)

def callback_H(data):
    rospy.loginfo('Hoger motor:  %f', data.data)

def callback_Head(data):
    rospy.loginfo('XXXXXXXXXXXXXXXXXXXXXXXXX:  %f', data.data)

def listener():

    rospy.init_node('listener', anonymous=True)

    rospy.Subscriber('motor_V', Float64, callback_V)
    rospy.Subscriber('motor_H', Float64, callback_H)
    rospy.Subscriber('heading', Float64, callback_Head)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
