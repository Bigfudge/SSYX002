#include <ros.h>
#include <ArduinoHardware.h>
#include <std_msgs/Int16.h>
#include <IRremote.h>
ros::NodeHandle  nh;

std_msgs::Int16 cmd_msg1;   // Höger motor
std_msgs::Int16 cmd_msg2;   // Vänster motor
ros::Publisher pub_msg1("speedH", &cmd_msg1);
ros::Publisher pub_msg2("speedV", &cmd_msg2);

double fram = 16736925;
double bak = 16754775;
double hoger = 16761405;
double vanster = 16720605;
double forts = 4294967295;
double noll = 16730805;
double ett = 16738455;
double tva = 16750695;
double tre = 16756815;
double fyra = 16743045;
double fem = 16718055;
double sex = 16724175;
double sju = 16716015;
double atta = 16726215;
double nio = 16734885;
double ok = 16712445;
double stjarna = 16728765;
double fyrkant = 16732845;

int hastighet = 0;    // Hastighet som standard 0-9 (stillastående-snabb)
int hastighetH = 0;
int hastighetV = 0;
int svang = 0;      // Svängtyp 0-1 (på plats-sänkt hastighet på en sida)
int remote = 0;

IRrecv irrecv(11);    // Tar emot IR-signaler på pin 11
decode_results results;

void setup()
{
  nh.initNode();

  nh.advertise(pub_msg1);
  nh.advertise(pub_msg2);

  irrecv.enableIRIn();  // Startar IR-mottagaren

}
long publisher_timer = 0;

void loop()
{
  if (remote = 1) {
    // steg 1: kontrollera antal pulser sen senast och konvertera till vinkelhastighet (rad/s)
    cmd_msg1.data = hastighetH * 0.87;
    cmd_msg2.data = hastighetV;

    // steg 2: publicera
    pub_msg1.publish(&cmd_msg1);
    pub_msg2.publish(&cmd_msg2);

    publisher_timer = millis() + 1000; // Publisera en gång per sekund
  }

  nh.spinOnce();

  if (irrecv.decode(&results))
  {
    //--------------------------------------------------------------- Hastighet
    if (results.value == noll)
    {
      hastighet = 0;
    }
    if (results.value == ett)
    {
      hastighet = 11;
    }
    if (results.value == tva)
    {
      hastighet = 22;
    }
    if (results.value == tre)
    {
      hastighet = 33;
    }
    if (results.value == fyra)
    {
      hastighet = 44;
    }
    if (results.value == fem)
    {
      hastighet = 56;
    }
    if (results.value == sex)
    {
      hastighet = 67;
    }
    if (results.value == sju)
    {
      hastighet = 78;
    }
    if (results.value == atta)
    {
      hastighet = 89;
    }
    if (results.value == nio)
    {
      hastighet = 100;
    }
    //--------------------------------------------------------------- Växla mellan fjärr av/på
    if (results.value == ok)
    {
      if (remote == 0)
      {
        remote = 1;
        hastighetH = 0;
        hastighetV = 0;
      }
      else
      {
        remote = 0;
      }
    }
    //--------------------------------------------------------------- Framåt
    if (results.value == fram)
    {
      irrecv.resume(); // Prepare to receive the next value
      delay(60); // Paus 60ms för att hinna starta om IR
      irrecv.decode(&results);  // Receive the next value
      while (results.value == forts)
      {
        hastighetH = hastighet; // Kör höger sida framåt
        hastighetV = hastighet; // Kör vänster sida framåt

        irrecv.resume(); // Prepare to receive the next value
        results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
        delay(150); // Paus 150ms för att hinna starta om IR
        irrecv.decode(&results);  // Receive the next value
      }
      hastighetH = 0;     // Stanna motor
      hastighetV = 0;     // Stanna motor
    }
    //-------------------------------------------------------------- Bakåt
    if (results.value == bak)
    {
      irrecv.resume(); // Prepare to receive the next value
      delay(60); // Paus 60ms för att hinna starta om IR
      irrecv.decode(&results);  // Receive the next value
      while (results.value == forts)
      {
        hastighetH = -hastighet; // Kör höger sida bakåt
        hastighetV = -hastighet; // Kör vänster sida bakåt

        irrecv.resume(); // Prepare to receive the next value
        results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
        delay(150); // Paus 150ms för att hinna starta om IR
        irrecv.decode(&results);  // Receive the next value
      }
      hastighetH = 0;     // Stanna motor
      hastighetV = 0;     // Stanna motor
    }
    if (svang == 0) //------------------------------------------------------- Sväng på stället
    {
      //-------------------------------------------------------------- Höger
      if (results.value == hoger)
      {
        irrecv.resume(); // Prepare to receive the next value
        delay(60); // Paus 60ms för att hinna starta om IR
        irrecv.decode(&results);  // Receive the next value
        while (results.value == forts)
        {
          hastighetH = -hastighet; // Kör höger sida bakåt
          hastighetV = hastighet; // Kör vänster sida framåt

          irrecv.resume(); // Prepare to receive the next value
          results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
          delay(150); // Paus 150ms för att hinna starta om IR
          irrecv.decode(&results);  // Receive the next value
        }
        hastighetH = 0;     // Stanna motor
        hastighetV = 0;     // Stanna motor
      }
      //-------------------------------------------------------------- Vänster
      if (results.value == vanster)
      {
        irrecv.resume(); // Prepare to receive the next value
        delay(60); // Paus 60ms för att hinna starta om IR
        irrecv.decode(&results);  // Receive the next value
        while (results.value == forts)
        {
          hastighetH = hastighet; // Kör höger sida framåt
          hastighetH = -hastighet; // Kör vänster sida bakåt

          irrecv.resume(); // Prepare to receive the next value
          results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
          delay(150); // Paus 150ms för att hinna starta om IR
          irrecv.decode(&results);  // Receive the next value
        }
        hastighetH = 0;     // Stanna motor
        hastighetV = 0;     // Stanna motor
      }
    }
    if (svang == 1) //------------------------------------------------------- Sväng, sänkt hastighet på en sida
    {
      //-------------------------------------------------------------- Höger
      if (results.value == hoger)
      {
        irrecv.resume(); // Prepare to receive the next value
        delay(60); // Paus 60ms för att hinna starta om IR
        irrecv.decode(&results);  // Receive the next value
        while (results.value == forts)
        {
          hastighetH = hastighet * 0.5; // Höger sida 50% hastighet framåt
          hastighetV = hastighet; // Kör vänster sida framåt

          irrecv.resume(); // Prepare to receive the next value
          results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
          delay(150); // Paus 150ms för att hinna starta om IR
          irrecv.decode(&results);  // Receive the next value
        }
        hastighetH = 0;     // Stanna motor
        hastighetV = 0;     // Stanna motor
      }
      //-------------------------------------------------------------- Vänster
      if (results.value == vanster)
      {
        irrecv.resume(); // Prepare to receive the next value
        delay(60); // Paus 60ms för att hinna starta om IR
        irrecv.decode(&results);  // Receive the next value
        while (results.value == forts)
        {
          hastighetH = hastighet; // Kör höger sida framåt
          hastighetV = hastighet*0.5; // Vänster sida 50% hastighet framåt
          
          irrecv.resume(); // Prepare to receive the next value
          results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
          delay(150); // Paus 150ms för att hinna starta om IR
          irrecv.decode(&results);  // Receive the next value
        }
        hastighetH = 0;     // Stanna motor
        hastighetV = 0;     // Stanna motor
      }
    }
    irrecv.resume(); // Receive the next value
  }
}
