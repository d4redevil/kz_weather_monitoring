#include <SoftwareSerial.h>
#include <dht.h>
#include <LiquidCrystal.h>

//Defining Analog pins
#define dht_apin A3
#define mq2_pin A4

//Defining Instance
dht DHT;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
SoftwareSerial mySerial(6, 7); //SIM800L Tx & Rx is connected to Arduino #6 & #7

int i;
int Smokelvl;
const int rs = 12, en = 11, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
String SMS_text;
String cmd;


void setup() {
  pinMode(A3,INPUT);

  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);


  //To Define baud rate and To Begin Serial Communication with Serial Moniter
  Serial.begin(9600);
  delay(100);
  Serial.println("\n....Serial Connection Is Established....");
  Serial.println("*********************");
  Serial.print("Initializing");
  for(i=0;i<6;i++)
   {
    Serial.print(".");  
    delay(500);
    }
  Serial.println("--------Good To Go---------");
  Serial.println("Initializing GSM..."); 
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, it will Return OK

  mySerial.write(Serial.read());
  while(mySerial.available())
  {
  String Reply=mySerial.readString();
  if(Reply == "OK")
  {
  Serial.println("GSM is Ready To Use.");
  }
  else
  {
  Serial.println("Make Sure GSM is Connected Properly");
  }
  }

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();

  mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();

  mySerial.print("Setup Completed!! Ready To Use."); //text content
  updateSerial();

  mySerial.write(26);//26 is used to define EOF(End Of File)
  updateSerial();

  mySerial.println("AT+CMGF=1"); //Configuring TEXT Mode 
  updateSerial();
  
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Configuring GSM to Forward the Latest Recieved Messaged to the Serial Port
  updateSerial();

  //Lcd Initilzation
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Intilizing");
  lcd.setCursor(0,1);
  lcd.print("    :)    ");
  delay(500);
  inti();
}

void loop()

{
//UPDATING SENSORS READINGS
  Smokelvl=analogRead(mq2_pin);
  DHT.read11(dht_apin);

//To Read Received Request/Message and To Seach For Word "Status" in the message.
  while(mySerial.available()) 
  {
  lcd.clear();
  SMS_text=mySerial.readString();
  int S=SMS_text.indexOf("Status");
  if (S > 10)
  {
    Right();
    }
   }

//To Read Request from Serial Moniter
  if(Serial.available() > 0)
  {
  cmd = Serial.readString();
  if((cmd == "Status"))
  {
    Right();
    }
  else 
  {
    intrupW();
    }
   }
   
//To Check The Threashold Value of smokelvl
  if (Smokelvl>300) 
  {
   smklvl(); 
   }
   
//To Check The Theashold Value of Temperature
  if(DHT.temperature > 50.00)
  {
   templvl();
   }

//Updation of Status on LCD
  lcd.setCursor(0,0);
  lcd.print("Humidity:");
  lcd.print(DHT.humidity);
  lcd.setCursor(0,1);
  lcd.print("Temperature:");
  lcd.print(DHT.temperature);

  Serial.flush();
 }

void inti() 
{
//READING DHT11 READING
  DHT.read11(dht_apin);

//SERIAL OUTPUT
  Serial.println("====================");
  Serial.print("Initially The Humidity percentage is ");
  Serial.print(int(DHT.humidity));
  Serial.println(" %");
  Serial.print("Initially The Tempreture is ");
  Serial.print(int(DHT.temperature));
  Serial.println(" c");
  Serial.println("====================");
  
//DELAY STATEMENT
  delay(1000);
 
 }

void Right()

{
//READING TEMPERATURE AND HUMIDITY STATUS
DHT.read11(dht_apin);

//SERIAL OUTPUT
Serial.println("Request Received");
Serial.println("Sending SMS");

//LCD OUTPUT
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Request Received");
lcd.setCursor(0,1);
lcd.print("Sending SMS");

//CREATION OF SMS CONTENET
String Output="The Humidity percentage is "+String(int(DHT.humidity))+" %"+"\nThe Tempreture is "+String(int(DHT.temperature))+" c"; 

//GSM INITILIZATION AND OUTPUT
mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
updateSerial();

mySerial.println("AT+CMGS=\"+916382954917\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
updateSerial();


mySerial.print(Output); //text content
updateSerial();

mySerial.write(26);

//DELAY STATEMENT
delay(10000);

//CONFIGURING GSM TO FORWARD SMS TO SERIAL PORT
mySerial.println("AT+CMGF=1"); 
mySerial.println("AT+CNMI=1,2,0,0,0");
}

void intrupW()

{
//SERIAL OUTPUT
Serial.println("Request Received But Invalid WAKE WORD");

//LCD OUTPUT
lcd.clear();
lcd.setCursor(0,0);
lcd.print("INVALID Command");

//DELAY STATEMENT
delay(5000);

}


void smklvl()
{
//READING SMOKELEVEL STATUS
Smokelvl = analogRead(mq2_pin);

//SERIAL OUTPUT
Serial.println("Smoke Level is above the Limit");
Serial.print(Smokelvl);
Serial.println("smoke is Detected");

//LCD OUTPUT
lcd.clear();  
lcd.setCursor(0,0);
lcd.print("Smoke Detected");
lcd.setCursor(0,1);
lcd.print("Open Windows");

//GSM INITILIZATION AND OUTPUT
mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
updateSerial();

mySerial.println("AT+CMGS=\"+916382954917\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
updateSerial();

mySerial.print("Smoke Level is Above The Limit."); //text content
updateSerial();

mySerial.write(26);

mySerial.println("AT+CMGF=1"); 
mySerial.println("AT+CNMI=1,2,0,0,0");

//DELAY Statement
delay(30000);
}

void templvl()

{
//READING TEMPERATURE STATUS
DHT.read11(dht_apin);

//SERIAL OUTPUT
Serial.print("Temperature Level Exceeded the Limit ");
Serial.print(DHT.temperature);
Serial.println(" Degree C");

//LCD OUTPUT
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Temperature");
lcd.setCursor(0,1); 
lcd.print("Exceeded");
lcd.clear();

//GSM INITIALIZATION AND OUTPUT
mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
updateSerial();

mySerial.println("AT+CMGS=\"+916382954917\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
updateSerial();

mySerial.println("Temperature level is Too High"); //SMS Content Which you wanna send.
updateSerial();

mySerial.write(26);

mySerial.println("AT+CMGF=1"); 
mySerial.println("AT+CNMI=1,2,0,0,0");

//DELAY Statement
delay(30000);
}

//Uncomment Second while statement if you are getting Troubles -
//If you wanna Troulbshoot It will give you a idea where you are getting trouble

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }


//  while(mySerial.available()) 
//  {
//    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
//  }
}
