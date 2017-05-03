#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import Float64

def talker():
    pub1 = rospy.Publisher('distSide_camshifttrack', Float64, queue_size=10)
    pub2 = rospy.Publisher('heading', Float64, queue_size=10)


    rospy.init_node('publisherArduino', anonymous=True)
    rate = rospy.Rate(10) # 10hz
    while not rospy.is_shutdown():
        pub1.publish(0.02)
        pub2.publish(0)
        rate.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass