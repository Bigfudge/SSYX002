
import rospy
from beginner_tutorials.msg import engine

#En nod som publiserar värdena från reglersystemet. Alltså vad höger och vänster motor ska ha för hastighet

def talker():

    pub = rospy.Publisher('motorSignal', String, queue_size=10)
    rospy.init_node('regler_Node', anonymous=True)
    rate = rospy.Rate(10) # 10hz
    
    while not rospy.is_shutdown():
        hello_str = "hello world %s" % rospy.get_time()
        rospy.loginfo(hello_str)
        pub.publish(hello_str)
        rate.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass