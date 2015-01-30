#include <SoftwareSerial.h>  

#define RxD 7
#define TxD 6

int steeringMotorDirectionPinA   = 8;
int steeringMotorDirectionPinB   = 11;
int steeringMotorSpeedPin        = 9;

int drivingMotorDirectionPinA    = 12;
int drivingMotorDirectionPinB    = 13;
int drivingMotorSpeedPin         = 10;

SoftwareSerial BlueToothSerial(RxD,TxD);
char flag=1;

void setupMotor()
{
  pinMode(steeringMotorDirectionPinA,OUTPUT);
  pinMode(steeringMotorDirectionPinB,OUTPUT);
  pinMode(steeringMotorSpeedPin,OUTPUT);
  pinMode(drivingMotorDirectionPinA,OUTPUT);
  pinMode(drivingMotorDirectionPinB,OUTPUT);
  pinMode(drivingMotorSpeedPin,OUTPUT);
}

void Test_BlueTooth()
{
  unsigned char t=0;
  String RXD = "";
  Serial.println("Test BlueTooth ...");
  BlueToothSerial.print("AT\r\n");  
  delay(100);
  while(BlueToothSerial.available()>0)
  {    
    RXD +=  char( BlueToothSerial.read() );
    delay(1);
  } 
  do{
    t++;
    delay(400);
    Serial.println("Test Failed ! Retrying ...");
  }
  while( ((RXD[0]!='O')&&(RXD[1]!='K'))&&(t<5));  
  if(t<5) Serial.println("Test Successful !\r\n");
  else { 
    Serial.println("Retry Failed !"); 
    while(1); 
  }
}

void sendBlueToothCommand(char *Command)
{
  BlueToothSerial.print(Command);
  Serial.print(Command); 
  delay(100);
  while(BlueToothSerial.available())
  {    
    Serial.print(char(BlueToothSerial.read())); 
  }
}

void setupBlueTooth()
{
  Serial.println("Bluetooth Initialization ...");      
  sendBlueToothCommand("AT+NAME=WackyDriving\r\n");
  sendBlueToothCommand("AT+ROLE=0\r\n");
  sendBlueToothCommand("AT+CMODE=0\r\n");
  sendBlueToothCommand("AT+PSWD=1234\r\n");
  sendBlueToothCommand("AT+UART=38400,0,0\r\n");
  delay(500);
  Serial.println("Bluetooth Initialized Successfully !\r\n");
  do{
    if(Serial.available())
    {
      if( Serial.read() == 'S')
      {
        sendBlueToothCommand("AT+RESET\r\n");
        flag = 0;
      }
    }
  }
  while(flag);
}

void setup()
{
  setupMotor();
  Serial.begin(38400);     
  BlueToothSerial.begin(38400); 
  delay(500);
  //Test_BlueTooth();  
  //setupBlueTooth();
}

int parseValue(String input)
{
  String aValue = input.substring(4,7);
  int value = aValue.toInt();
  return value;  
}
void moveForward(String input)
{
  // parse the value
  int value = parseValue(input);
  
  // spin up the motor
  analogWrite(drivingMotorSpeedPin, value);
  digitalWrite(drivingMotorDirectionPinA, HIGH);
  digitalWrite(drivingMotorDirectionPinB, LOW);  

  // print what is happening
  Serial.print("Moving forward with speed ");
  Serial.println(value);
  
}
void moveBackward(String input)
{
  // parse the value
  int value = parseValue(input);
  
  // spin up the motor
  analogWrite(drivingMotorSpeedPin, value);
  digitalWrite(drivingMotorDirectionPinA, LOW);  
  digitalWrite(drivingMotorDirectionPinB, HIGH);
  
  // print what is happening
  Serial.print("Moving backward with speed ");
  Serial.println(value);
}
void steerLeft(String input)
{
  // parse the value
  int value = parseValue(input);
  
  // spin up the motor
  analogWrite(steeringMotorSpeedPin, value);
  digitalWrite(steeringMotorDirectionPinA, HIGH);  
  digitalWrite(steeringMotorDirectionPinB, LOW);  
  
  // print what is happening
  Serial.print("Steering left with speed ");
  Serial.println(value);
}
void steerRight(String input)
{
  // parse the value
  int value = parseValue(input);
  
  // spin up the motor
  analogWrite(steeringMotorSpeedPin, value);
  digitalWrite(steeringMotorDirectionPinA, LOW);
  digitalWrite(steeringMotorDirectionPinB, HIGH);  
  
  // print what is happening
  Serial.print("Steering right with speed ");
  Serial.println(value);
}

void processMove(String input)
{
  String aDirection = input.substring(2,3);
  if(aDirection == "F")
  {
    moveForward(input);
  }
  else if(aDirection == "B")
  {
    moveBackward(input);
  }
  else
  {
    Serial.println("Direction was unrecognized.");
  }      
}

void processTurn(String input)
{
  String aDirection = input.substring(2,3);
  if(aDirection == "L")
  {
    steerLeft(input);
  }
  else if(aDirection == "R")
  {
    steerRight(input);
  }
  else
  {
    Serial.println("Direction was unrecognized.");
  }
}

void processCommand(String input)
{
  String aCommand = input.substring(0,2);  
  if(aCommand == "MV")
  {
    processMove(input);
  }
  else if(aCommand == "TW")
  {
    processTurn(input);
  }
  else
  {
    Serial.println("Command was unrecognized.");
  }
}

String aString;

void loop()
{
  /*
   analogWrite(drivingMotorSpeedPin,0);
   digitalWrite(drivingMotorDirectionPinA,HIGH);
   digitalWrite(drivingMotorDirectionPinB,LOW);
   Serial.print("Speed set");
   
   */

  /*
  if(BlueToothSerial.available())
   {
   Serial.print(char(BlueToothSerial.read()));
   }
   */

  // Read from the Bluetooth serial and process the command
  while (BlueToothSerial.available() > 0)
  {
    char received = char(BlueToothSerial.read());
    aString += received; 

    // Process message when new line character is recieved
    if (received == '\n')
    {
      Serial.print("Received: "+aString);      
      processCommand(aString);
      aString = ""; // Clear received buffer
    }
  }

  if(Serial.available())
  {
    //read and discard
    Serial.read();

    String msg = "NINO TEST\r\n";
    Serial.print("Sending: "+msg);
    BlueToothSerial.print(msg);
  }	       

}
