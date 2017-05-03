#!/usr/bin/env python
# 

import rospy
from std_msgs.msg import Float64

def callback_V(data):
    rospy.loginfo('Vanster motor:  %f', data.data)

def callback_H(data):
    rospy.loginfo('Hoger motor:  %f', data.data)

#def callback_Head(data):
#    rospy.loginfo(rospy.get_caller_id() + 'Heading:  %i', data.data)

def listener():

    rospy.init_node('listener', anonymous=True)

    rospy.Subscriber('speedV', Float64, callback_V)
    rospy.Subscriber('speedH', Float64, callback_H)
    #rospy.Subscriber('heading', Float64, callback_Head)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
