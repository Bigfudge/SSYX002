#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include <WProgram.h>
#endif

#include <Servo.h>
#include <ros.h>
#include <std_msgs/UInt16.h>
#include <IRremote.h>

ros::NodeHandle  nh;

double V = 0, H = 0;
Servo motor_H, motor_V;        // Definierar "servon"

void servo_cb1( const std_msgs::UInt16& cmd_msg1)
{
  H = map(cmd_msg1.data,-100,100,-495,495);                 // Mappar om insignalen (+-100) till det önskade intervallet (+-495) [map(value, fromLow, fromHigh, toLow, toHigh)]
  motor_H.writeMicroseconds(1500 - H);                      //set servo pwm width
  digitalWrite(13, HIGH - digitalRead(13));                 //toggle led
}

void servo_cb2( const std_msgs::UInt16& cmd_msg2)
{
  V = map(cmd_msg2.data,-100,100,-495,495);                 // Mappar om insignalen
  motor_V.writeMicroseconds(1500 + V);                      //set servo pwm width
  digitalWrite(13, HIGH - digitalRead(13));                 //toggle led
}

ros::Subscriber<std_msgs::UInt16> sub1("motor_H", servo_cb1);
ros::Subscriber<std_msgs::UInt16> sub2("motor_V", servo_cb2);

int fram = 16736925;     // Pilknapp framåt
int ok = 16712445;       // Mittenknapp
int forts = 4294967295;  // Vid nedtryckt knapp

IRrecv irrecv(11);    // Tar emot IR-signaler på pin 11
decode_results results;

void setup()
{
  nh.initNode();
  nh.subscribe(sub1);
  nh.subscribe(sub2);

  Serial.begin(9600);   // Om vi vill skriva något till serial monitor
  irrecv.enableIRIn();  // Startar IR-mottagaren

  motor_H.attach(9);    // Ansluter höger motor på pin 9
  motor_V.attach(10);   // Ansluter vänster motor på pin 10

  motor_H.writeMicroseconds(1500);    // Ser till att båda motorer står still innan de får signal
  motor_V.writeMicroseconds(1500);
}

void loop()
{
  if (irrecv.decode(&results))  // När det kommer en IR-signal
  {
    nh.spinOnce();
    delay(1);

    if (results.value == ok)    // Nödstopp när man trycker på mittenknappen, nollställs genom att trycka pil upp.
    {
      while (results.value != fram)
      {
        motor_H.writeMicroseconds(1500);    // Stannar båda motorerna
        motor_V.writeMicroseconds(1500);

        irrecv.resume();                    // Förbereder för att ta emot nästa värde
        delay(100);                         // Paus 100ms för att hinna starta om IR
        irrecv.decode(&results);            // Tar emot nästa värde

        Serial.println("Nödstopp aktiv");   // Skriver ut att nödstoppen är aktiv i serial monitor
      }
      irrecv.resume();                      // Tar emot nästa värde
    }
  }
}
