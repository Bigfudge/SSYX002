#!/usr/bin/env python
# 

import rospy
from std_msgs.msg import Float64

def callback_V(data):
    V =11.4474 + 5.96269* data.data 
    pu2.publish(V)

def callback_H(data):
    H=(5.41462 *data.data + 10.2732)
    pu1.publish(H)
    

#def callback_Head(data):
#    rospy.loginfo(rospy.get_caller_id() + 'Heading:  %i', data.data)

def listener():

    rospy.init_node('test', anonymous=True)

    rospy.Subscriber('test_H', Float64, callback_V)
    rospy.Subscriber('test_V', Float64, callback_H)

    pu1 = rospy.Publisher('motor_H', Float64, queue_size=10)
    pu2 = rospy.Publisher('motor_V', Float64, queue_size=10)

    rate = rospy.Rate(10) # 10hz
    #rospy.Subscriber('heading', Float64, callback_Head)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
