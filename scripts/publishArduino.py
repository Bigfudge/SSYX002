#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import Float64

def talker():
    pub1 = rospy.Publisher('motor_H', Float64, queue_size=10)
    pub2 = rospy.Publisher('motor_V', Float64, queue_size=10)


    rospy.init_node('talker', anonymous=True)
    rate = rospy.Rate(10) # 10hz
    while not rospy.is_shutdown():
        hello_str = 30
        pub1.publish(hello_str)
        pub2.publish(hello_str)
        rate.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass