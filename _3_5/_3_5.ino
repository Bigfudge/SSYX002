
#include <Servo.h>
#include <ros.h>
#include <ArduinoHardware.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Bool.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

ros::NodeHandle  nh;

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

double V = 0, H = 0;
Servo motor_H, motor_V;        // Definierar "servon"

void servo_cb1( const std_msgs::Float64& cmd_msg1)                  // Styrning av höger motor
{
  H = map(cmd_msg1.data, -100, 100, -495, 495);            // Mappar om insignalen (+-100) till det önskade intervallet (+-495) [map(value, fromLow, fromHigh, toLow, toHigh)]
  motor_H.writeMicroseconds(1500-H);                      // set servo pwm width
}

void servo_cb2( const std_msgs::Float64& cmd_msg2)                // Styrning av vänster motor
{
  V = map(cmd_msg2.data, -100, 100, -495, 495);
  motor_V.writeMicroseconds(1500+V);                            // set servo pwm width
}

void servo_cb3( const std_msgs::Bool& cmd_msg3)                   // Stoppsignal
{
  if (cmd_msg3.data) 
  {
    motor_H.writeMicroseconds(1500);                        // Stoppar båda motorerna ifall vi får stopsignal
    motor_V.writeMicroseconds(1500);
    delay(1000);
  }                      
}

ros::Subscriber<std_msgs::Float64> sub1("motor_H", servo_cb1);  // Subscribe till styrning av höger motor
ros::Subscriber<std_msgs::Float64> sub2("motor_V", servo_cb2);  // Subscribe till styrning av vänster motor
ros::Subscriber<std_msgs::Bool> sub3("stop", servo_cb3);     // Subscribe till STOP

std_msgs::Float64 spdH_msg;
std_msgs::Float64 spdV_msg;
ros::Publisher pub_spdH("speedH", &spdH_msg);
ros::Publisher pub_spdV("speedV", &spdV_msg);

std_msgs::Float64 heading_msg;
ros::Publisher pub_heading("heading", &heading_msg);

volatile int countH = 0;    // Räknar antal pulser på höger sida
volatile int countV = 0;    // Räknar antal pulser på vänster sida
float heading = 0;          // Riktning
float firstTime = true;
float vinkelKorrigering = 0;


void setup()
{
  mag.begin();           // Startar kompassen
  nh.initNode();

  nh.subscribe(sub1);
  nh.subscribe(sub2);
  nh.subscribe(sub3);

  nh.advertise(pub_spdH);
  nh.advertise(pub_spdV);
  nh.advertise(pub_heading);

  motor_H.attach(9);    // Ansluter höger motor på pin 9
  motor_V.attach(10);   // Ansluter vänster motor på pin 10

  motor_H.writeMicroseconds(1500);    // Ser till att båda motorer står still innan de får signal
  motor_V.writeMicroseconds(1500);

  pinMode(2, INPUT);  // Insignal för höger impulsgivare
  pinMode(3, INPUT);  // Insignal för vänster impulsgivare
  attachInterrupt(2, right, RISING); // Skapar interrupt för räkna antal pulser
  attachInterrupt(3, left, RISING);

}
long publisher_timer = 0;

void loop()
{
  if (millis() > publisher_timer) {
    // steg 1: kontrollera antal pulser sen senast och konvertera till vinkelhastighet (rad/s)
    
    spdH_msg.data = countH*2*PI/365;   // 365 pulser/varv => antal varv, ett varv = 2PI rad => rad
    spdV_msg.data = countV*2*PI/365;

    // steg 2: publicera vinkelhastigheten
    pub_spdH.publish(&spdH_msg);
    pub_spdV.publish(&spdV_msg);


    // hämta kompassriktning
    sensors_event_t event;
    mag.getEvent(&event);

    // beräkna riktning
    heading = atan2(event.magnetic.y, event.magnetic.x);

    float declinationAngle = 0.0567;  // Korrigerar för avvikelse mellan magnetfältet och norr
    heading += declinationAngle;

    if (heading < 0)    // Korrigera tecken
      heading += 2 * PI;

    if (heading > 2 * PI) // Kontrollera så att vinkeln inte blivit för stor efter korrigering
      heading -= 2 * PI;

    
    if(firstTime){
      vinkelKorrigering = heading;
      firstTime=false;
    }

    //heading_msg.data = (heading * 180 / M_PI)-vinkelKorrigering; // Gör om från radianer till grader
    heading_msg.data =heading-vinkelKorrigering;
    
    // publicera riktning
    pub_heading.publish(&heading_msg);
    
    countH = 0;   // Nollställer räknare
    countV = 0;
    publisher_timer = millis() + 1000; // Publisera en gång per sekund  
    //nh.spinOnce();
    }
    nh.spinOnce();
    
}

void right() {        // Ökar räknaren för höger sida med ett för varje puls
  countH = ++countH;
}
void left() {         // Ökar räknaren för vänster sida med ett för varje puls
  countV = ++countV;
}
