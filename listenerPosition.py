#!/usr/bin/env python
# 

import rospy
from std_msgs.msg import Float32

def callback_V(data):
    rospy.loginfo(rospy.get_caller_id() + 'Rotation Diviation :  %i', data.data)

def callback_H(data):
    rospy.loginfo(rospy.get_caller_id() + 'Distance :  %i', data.data)

def listener():

    # In ROS, nodes are uniquely named. If two nodes with the same
    # name are launched, the previous one is kicked off. The
    # anonymous=True flag means that rospy will choose a unique
    # name for our 'listener' node so that multiple listeners can
    # run simultaneously.
    rospy.init_node('listenerPosition', anonymous=True)

    rospy.Subscriber('distance', Float32, callback_H)
    rospy.Subscriber('rotationDeviation', Float32, callback_V)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
