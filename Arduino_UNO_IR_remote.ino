#include <IRremote.h>
#include <Servo.h>

Servo motor_H;
Servo motor_V;

int RECV_PIN = 11;
int MOTOR_H_PIN = 9;
int MOTOR_V_PIN = 10;

int hastighet = 0;  // Hastighet som standard 0-9 (stillastående-snabb)
int svang = 0;      // Svängtyp 0-1 (på plats-sänkt hastighet på en sida)

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

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  motor_H.attach(MOTOR_H_PIN);
  motor_V.attach(MOTOR_V_PIN);
  motor_H.writeMicroseconds(1500);    // Stanna motor
  motor_V.writeMicroseconds(1500);    // Stanna motor
}

void loop()
{
  if (irrecv.decode(&results))
  {
    //--------------------------------------------------------------- Hastighet
    if (results.value == noll)
    {
      hastighet = 0;
    }
    if (results.value == ett)
    {
      hastighet = 55;
    }
    if (results.value == tva)
    {
      hastighet = 110;
    }
    if (results.value == tre)
    {
      hastighet = 165;
    }
    if (results.value == fyra)
    {
      hastighet = 220;
    }
    if (results.value == fem)
    {
      hastighet = 275;
    }
    if (results.value == sex)
    {
      hastighet = 330;
    }
    if (results.value == sju)
    {
      hastighet = 385;
    }
    if (results.value == atta)
    {
      hastighet = 440;
    }
    if (results.value == nio)
    {
      hastighet = 495;
    }
    //--------------------------------------------------------------- Typ av sväng
    if (results.value == stjarna)
    {
      svang = 0;
    }
    if (results.value == fyrkant)
    {
      svang = 1;
    }
    //--------------------------------------------------------------- Tidlås
    if (results.value == ok)
    {
      Serial.println("Locked for 10s");
      delay(10000);   // Låser allt i 10s
      Serial.println("Unlocked");
    }
    //--------------------------------------------------------------- Framåt
    if (results.value == fram)
    {
      irrecv.resume(); // Prepare to receive the next value
      delay(60); // Paus 60ms för att hinna starta om IR
      irrecv.decode(&results);  // Receive the next value
      while (results.value == forts)
      {
        motor_H.writeMicroseconds(1500 - hastighet); // Kör höger sida framåt
        motor_V.writeMicroseconds(1500 + hastighet); // Kör vänster sida framåt
        Serial.print("Kör framåt med hastighet ");
        Serial.println(hastighet);
        irrecv.resume(); // Prepare to receive the next value
        results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
        delay(150); // Paus 150ms för att hinna starta om IR
        irrecv.decode(&results);  // Receive the next value
      }
      motor_H.writeMicroseconds(1500);    // Stanna motor
      motor_V.writeMicroseconds(1500);    // Stanna motor
      Serial.println("Står still, efter att ha kört");
    }
    //-------------------------------------------------------------- Bakåt
    if (results.value == bak)
    {
      irrecv.resume(); // Prepare to receive the next value
      delay(60); // Paus 60ms för att hinna starta om IR
      irrecv.decode(&results);  // Receive the next value
      while (results.value == forts)
      {
        motor_H.writeMicroseconds(1500 + hastighet); // Kör höger sida framåt
        motor_V.writeMicroseconds(1500 - hastighet); // Kör vänster sida framåt
        Serial.print("Kör bakåt med hastighet ");
        Serial.println(hastighet);
        irrecv.resume(); // Prepare to receive the next value
        results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
        delay(150); // Paus 150ms för att hinna starta om IR
        irrecv.decode(&results);  // Receive the next value
      }
      motor_H.writeMicroseconds(1500);    // Stanna motor
      motor_V.writeMicroseconds(1500);    // Stanna motor
      Serial.println("Står still, efter att ha kört");
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
          motor_H.writeMicroseconds(1500 + hastighet); // Kör höger sida framåt
          motor_V.writeMicroseconds(1500 + hastighet); // Kör vänster sida framåt
          Serial.print("Svänger höger (*) med hastighet ");
          Serial.println(hastighet);
          irrecv.resume(); // Prepare to receive the next value
          results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
          delay(150); // Paus 150ms för att hinna starta om IR
          irrecv.decode(&results);  // Receive the next value
        }
        motor_H.writeMicroseconds(1500);    // Stanna motor
        motor_V.writeMicroseconds(1500);    // Stanna motor
        Serial.println("Står still, efter att ha kört");
      }
      //-------------------------------------------------------------- Vänster
      if (results.value == vanster)
      {
        irrecv.resume(); // Prepare to receive the next value
        delay(60); // Paus 60ms för att hinna starta om IR
        irrecv.decode(&results);  // Receive the next value
        while (results.value == forts)
        {
          motor_H.writeMicroseconds(1500 - hastighet); // Kör höger sida framåt
          motor_V.writeMicroseconds(1500 - hastighet); // Kör vänster sida framåt
          Serial.print("Svänger vänster (*) med hastighet ");
          Serial.println(hastighet);
          irrecv.resume(); // Prepare to receive the next value
          results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
          delay(150); // Paus 150ms för att hinna starta om IR
          irrecv.decode(&results);  // Receive the next value
        }
        motor_H.writeMicroseconds(1500);    // Stanna motor
        motor_V.writeMicroseconds(1500);    // Stanna motor
        Serial.println("Står still, efter att ha kört");
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
          motor_H.writeMicroseconds(1500 - hastighet*0.5); // Höger sida 50% hastighet framåt
          motor_V.writeMicroseconds(1500 + hastighet); // Kör vänster sida framåt
          Serial.print("Svänger höger (#) med hastighet ");
          Serial.println(hastighet);
          irrecv.resume(); // Prepare to receive the next value
          results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
          delay(150); // Paus 150ms för att hinna starta om IR
          irrecv.decode(&results);  // Receive the next value
        }
        motor_H.writeMicroseconds(1500);    // Stanna motor
        motor_V.writeMicroseconds(1500);    // Stanna motor
        Serial.println("Står still, efter att ha kört");
      }
      //-------------------------------------------------------------- Vänster
      if (results.value == vanster)
      {
        irrecv.resume(); // Prepare to receive the next value
        delay(60); // Paus 60ms för att hinna starta om IR
        irrecv.decode(&results);  // Receive the next value
        while (results.value == forts)
        {
          motor_H.writeMicroseconds(1500 - hastighet); // Kör höger sida framåt
          motor_V.writeMicroseconds(1500 + hastighet*0.5); // Vänster sida 50% hastighet framåt
          Serial.print("Svänger vänster (#) med hastighet ");
          Serial.println(hastighet);
          irrecv.resume(); // Prepare to receive the next value
          results.value = 0;  // Nollställer resultatet så att inte loopen går förevigt
          delay(150); // Paus 150ms för att hinna starta om IR
          irrecv.decode(&results);  // Receive the next value
        }
        motor_H.writeMicroseconds(1500);    // Stanna motor
        motor_V.writeMicroseconds(1500);    // Stanna motor
        Serial.println("Står still, efter att ha kört");
      }
    }
    irrecv.resume(); // Receive the next value
    Serial.println(results.value, HEX);
  }
}
