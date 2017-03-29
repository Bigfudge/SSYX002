#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import Int16

def talker():
    pub1 = rospy.Publisher('motor_H', Int16, queue_size=10)
    pub2 = rospy.Publisher('motor_V', Int16, queue_size=10)


    rospy.init_node('talker', anonymous=True)
    rate = rospy.Rate(10) # 10hz
    while not rospy.is_shutdown():
        hello_str = 0
        pub1.publish(hello_str)
        pub2.publish(hello_str)
        rate.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass