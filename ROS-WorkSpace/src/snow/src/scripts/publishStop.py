#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import Bool

def talker():
    pub1 = rospy.Publisher('stop', Bool, queue_size=10)

    rospy.init_node('talker', anonymous=True)
    rate = rospy.Rate(1000) # 10hz
    while not rospy.is_shutdown():
        hello_str = True
        pub1.publish(hello_str)
        rate.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass