#include <ros.h>
#include <ArduinoHardware.h>
#include <std_msgs/Float64.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

ros::NodeHandle  nh;

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

std_msgs::Float64 heading_msg;
ros::Publisher pub_heading("heading", &heading_msg);

float vinkelKorrigering = 0;
float heading = 0;          // Riktning

void setup()
{
  mag.begin();           // Startar kompassen
  nh.initNode();
  nh.advertise(pub_heading);
}
long publisher_timer = 0;

void loop()
{
  if (millis() > publisher_timer) {
    publisher_timer = millis() + 1000; // Publisera en gång per sekund 
    
    // hämta kompassriktning
    sensors_event_t event;
    mag.getEvent(&event);

    // beräkna riktning
    heading = atan2(event.magnetic.y, event.magnetic.x);

    float declinationAngle = 0.0567;    // Korrigerar för avvikelse mellan magnetfältet och norr
    heading += declinationAngle;

    if (heading < 0)    // Korrigera tecken
      heading += 2 * PI;

    if (heading > 2 * PI) // Kontrollera så att vinkeln inte blivit för stor efter korrigering
      heading -= 2 * PI;

    /*if(firstTime){
      vinkelKorrigering = heading * 180 / M_PI;
      firstTime=false;
    
    }*/
    heading_msg.data = ((heading * 180 / M_PI)); // Gör om från radianer till grader
    // publicera riktning
    pub_heading.publish(&heading_msg);
  }
    nh.spinOnce();
}
