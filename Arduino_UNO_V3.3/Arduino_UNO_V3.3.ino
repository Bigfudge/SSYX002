#include <Servo.h>
#include <ros.h>
#include <ArduinoHardware.h>
#include <std_msgs/Float64.h>
//#include <IRremote.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
ros::NodeHandle  nh;

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

std_msgs::Float64 str_msg;
double V = 0, H = 0;
Servo motor_H, motor_V;        // Definierar "servon"

ros::Publisher chatter("chatter", &str_msg);

void servo_cb1( const std_msgs::Float64& cmd_msg1)            // Styrning av höger motor
{
  H = map(cmd_msg1.data, -100, 100, -495, 495);             // Mappar om insignalen (+-100) till det önskade intervallet (+-495) [map(value, fromLow, fromHigh, toLow, toHigh)]
  motor_H.writeMicroseconds(1500 - H*(0.87));                      // set servo pwm width
}

void servo_cb2( const std_msgs::Float64& cmd_msg2)            // Styrning av vänster motor
{
  V = map(cmd_msg2.data, -100, 100, -495, 495);             // Mappar om insignalen
  motor_V.writeMicroseconds(1500 + V);                      // set servo pwm width
}

void servo_cb3( const std_msgs::Float64& cmd_msg3)            // Stoppsignal
{
  if (cmd_msg3.data)
  {
    motor_H.writeMicroseconds(1500);                        // Stoppar båda motorerna då en 1:a skickas
    motor_V.writeMicroseconds(1500);
  }
}

ros::Subscriber<std_msgs::Float64> sub1("motor_H", servo_cb1);  // Subscribe till styrning av höger motor
ros::Subscriber<std_msgs::Float64> sub2("motor_V", servo_cb2);  // Subscribe till styrning av vänster motor
ros::Subscriber<std_msgs::Float64> sub3("stop", servo_cb3);     // Subscribe till STOP

std_msgs::Float64 spdH_msg;
std_msgs::Float64 spdV_msg;
ros::Publisher pub_spdH("speedH", &spdH_msg);
ros::Publisher pub_spdV("speedV", &spdV_msg);

std_msgs::Float64 heading_msg;
ros::Publisher pub_heading("heading", &heading_msg);

float fram = 16736925;      // Pilknapp framåt
float ok = 16712445;        // Mittenknapp
float forts = 4294967295;   // Vid nedtryckt knapp
volatile int countH = 0;    // Räknar antal pulser på höger sida
volatile int countV = 0;    // Räknar antal pulser på vänster sida
float heading = 0;          // Riktning


//IRrecv irrecv(11);    // Tar emot IR-signaler på pin 11
//decode_results results;

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

  //irrecv.enableIRIn();  // Startar IR-mottagaren

  motor_H.attach(9);    // Ansluter höger motor på pin 9
  motor_V.attach(10);   // Ansluter vänster motor på pin 10

  motor_H.writeMicroseconds(1500);    // Ser till att båda motorer står still innan de får signal
  motor_V.writeMicroseconds(1500);

  pinMode(2, INPUT);  // Insignal för höger impulsgivare
  pinMode(3, INPUT);  // Insignal för vänster impulsgivare
  attachInterrupt(digitalPinToInterrupt(2), right, RISING); // Skapar interrupt för räkna antal pulser
  attachInterrupt(digitalPinToInterrupt(3), left, RISING);

}
long publisher_timer = 0;

void loop()
{
  sensors_event_t event;
  mag.getEvent(&event);
  if (millis() > publisher_timer) {
    // steg 1: kontrollera antal pulser sen senast och konvertera till vinkelhastighet (rad/s)
    
    spdH_msg.data = countH/(365*2*PI);   // 365 pulser/varv => antal varv, ett varv = 2PI rad => rad
    spdV_msg.data = countV/(365*2*PI);

    // steg 2: publicera vinkelhastigheten
    pub_spdH.publish(&spdH_msg);
    pub_spdV.publish(&spdV_msg);
/*
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

    heading_msg.data = heading * 180 / M_PI; // Gör om från radianer till grader

    // publicera riktning
    pub_heading.publish(&heading_msg);
*/
    countH = 0;   // Nollställer räknare
    countV = 0;
    publisher_timer = millis() + 1000; // Publisera en gång per sekund
  }

  nh.spinOnce();

  /*if (irrecv.decode(&results))  // När det kommer en IR-signal
    {
    if (results.value == ok)    // Nödstopp när man trycker på mittenknappen, nollställs genom att trycka pil upp.
    {
      while (results.value != fram)
      {
        motor_H.writeMicroseconds(1500);    // Stannar båda motorerna
        motor_V.writeMicroseconds(1500);

        irrecv.resume();                    // Förbereder för att ta emot nästa värde
        delay(100);                         // Paus 100ms för att hinna starta om IR
        irrecv.decode(&results);            // Tar emot nästa värde
      }
      irrecv.resume();                      // Tar emot nästa värde
    }
    }*/
}

void right() {        // Ökar räknaren för höger sida med ett för varje puls
  countH = ++countH;
}
void left() {         // Ökar räknaren för vänster sida med ett för varje puls
  countV = ++countV;
}
