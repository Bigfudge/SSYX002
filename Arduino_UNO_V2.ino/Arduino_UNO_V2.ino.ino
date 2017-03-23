/*
   rosserial Publisher Example
   Prints "hello world!"
*/
#include <Servo.h>
#include <ros.h>
#include <ArduinoHardware.h>
#include <std_msgs/Int16.h>
#include <IRremote.h>
ros::NodeHandle  nh;

std_msgs::Int16 str_msg;
double V = 0, H = 0;
Servo motor_H, motor_V;        // Definierar "servon"

//ros::Publisher chatter("chatter", &str_msg);

void servo_cb1( const std_msgs::Int16& cmd_msg1)
{
  H = map(cmd_msg1.data, -100, 100, -495, 495);             // Mappar om insignalen (+-100) till det önskade intervallet (+-495) [map(value, fromLow, fromHigh, toLow, toHigh)]
  motor_H.writeMicroseconds(1500 - H);                      //set servo pwm width
  digitalWrite(13, HIGH - digitalRead(13));                 //toggle led
}

void servo_cb2( const std_msgs::Int16& cmd_msg2)
{
  V = map(cmd_msg2.data, -100, 100, -495, 495);             // Mappar om insignalen
  motor_V.writeMicroseconds(1500 + V);                      //set servo pwm width
  digitalWrite(13, HIGH - digitalRead(13));                 //toggle led
}

ros::Subscriber<std_msgs::Int16> sub1("motor_H", servo_cb1);
ros::Subscriber<std_msgs::Int16> sub2("motor_V", servo_cb2);
std_msgs::Int16 spdH_msg;
std_msgs::Int16 spdV_msg;
ros::Publisher pub_spdH("speedH", &spdH_msg);
ros::Publisher pub_spdV("speedV", &spdV_msg);

int fram = 16736925;     // Pilknapp framåt
int ok = 16712445;       // Mittenknapp
int forts = 4294967295;  // Vid nedtryckt knapp
volatile byte countH = 0; // Räknar antal pulser på höger sida
volatile byte countV = 0; // Räknar antal pulser på vänster sida
int speedH = 0;          // Hastighet höger sida
int speedV = 0;          // Hastighet vänster sida


IRrecv irrecv(11);    // Tar emot IR-signaler på pin 11
decode_results results;

void setup()
{
  nh.initNode();
  //nh.advertise(chatter);

  nh.subscribe(sub1);
  nh.subscribe(sub2);

  nh.advertise(pub_spdH);
  nh.advertise(pub_spdV);


  //Serial.begin(9600);   // Om vi vill skriva något till serial monitor
  irrecv.enableIRIn();  // Startar IR-mottagaren

  motor_H.attach(9);    // Ansluter höger motor på pin 9
  motor_V.attach(10);   // Ansluter vänster motor på pin 10

  motor_H.writeMicroseconds(1500);    // Ser till att båda motorer står still innan de får signal
  motor_V.writeMicroseconds(1500);

  pinMode(2, INPUT);  // Insignal för höger impulsgivare
  pinMode(3, INPUT);  // Insignal för vänster impulsgivare
  attachInterrupt(digitalPinToInterrupt(2), right, RISING); // Skapar interrupt för stigande flank på pulsen
  attachInterrupt(digitalPinToInterrupt(3), left, RISING);
  
}
long publisher_timer;

void loop()
{
if (millis() > publisher_timer) {
    // steg 1: kontrollera antal pulser sen senast och konvertera till hastighet
    speedH = (countH * 1000 / (1000 + millis() - publisher_timer));   // Antal pulser per sekund
    speedV = (countV * 1000 / (1000 + millis() - publisher_timer));
    speedH = speedH;    // Justering för antal pulser per meter
    speedV = speedV;
    //------------------------------------------------Kod måste läggas till för att kunna läsa encodrarna
    spdH_msg.data = speedH;
    spdV_msg.data = speedV;
    pub_spdH.publish(&spdH_msg);
    pub_spdV.publish(&spdV_msg);

    countH = 0;   // Nollställer räknare
    countV = 0;
  }

  publisher_timer = millis() + 1000; // Publisera en gång per sekund
  nh.spinOnce();

  if (irrecv.decode(&results))  // När det kommer en IR-signal
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

        Serial.println("Nödstopp aktiv");   // Skriver ut att nödstoppen är aktiv i serial monitor
      }
      irrecv.resume();                      // Tar emot nästa värde
    }
  }
}
void right() {        // Ökar räknaren för höger sida med ett för varje puls
  countH = ++countH;
}
void left() {         // Ökar räknaren för vänster sida med ett för varje puls
  countV = ++countV;
}
