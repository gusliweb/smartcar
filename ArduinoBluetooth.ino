// arduino  >>>  bluetooth
// D06      >>>  Rx
// D05      >>>  Tx

#include <SoftwareSerial.h>// import the serial library
#include <Servo.h>
#include <Wire.h>
//#include<Car.h>

#define SLAVE_ADDRESS 0x04

#define THERMOPILE A0
#define THERMISTOR A1

// constants for the thermistor calculations
const float a = -412.6;
const float b = 140.41;
const float c = 0.00764;
const float d = -0.0000000000000000625;
const float e = -0.00000000000000000000000115;

// constants for the thermopile calculation
const float k = 0.004313; 
const float delta = 2.468;

const float reftemp = 25; // reference temperature is 25C
const float shiftv = 0.6; // amount to shift thermopile voltage for negative V values in range
const float verr = 0.6;  // voltage error introduced to thermopile by circuit

int number = 0;
int state = 0;

SoftwareSerial CustomSerial(5, 6); // RX, TX
int ledpin=13; // led on D13 will show blink on / off
int BluetoothData = 0; // the data given from Computer
Servo myservo0,myservo1,myservo2,myservo3; // declare the servo objects
char incomingByte; // variable to receive data from the serial port


#define trigPin 16
#define echoPin 17
#define led 18
#define led2 19

int pos=0, pos1=0, pos2=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  CustomSerial.begin(9600);
  myservo0.attach(11);  // attaches the servo on pin 9 to the servo object 
  myservo1.attach(12);  // attaches the servo object in pin
  myservo2.attach(10);  // attaches the servo on pin 9 to the servo object 
  myservo3.attach(9);
  myservo0.write(100);
  myservo1.write(0);
  myservo2.write(160);
  myservo3.write(60);
  
  CustomSerial.println("Bluetooth On please press 1 or 0 blink LED ..");
  pinMode(ledpin,OUTPUT);
  
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  
  //for Ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
}

char old = 0;
void loop() {
  
  //get distance from Ultrasonic sensor
  getDistance();
  
  //get temperature
  getTemp();
  // put your main code here, to run repeatedly:
   if (CustomSerial.available()){
     BluetoothData=getInstruction();
   }
   else {
     delay(15);
     if (CustomSerial.available()){
        BluetoothData=getInstruction();
      }
    }
     //Serial.println(BluetoothData); //for debugging
     
     if(BluetoothData=='1'){   // if number 1 pressed ....
       digitalWrite(ledpin,HIGH);
       CustomSerial.println("Forward");
       for(pos = 45,pos1 = 160; pos > 0,pos1 > 70; pos -= 1,pos1 -= 1)  // goes from 0 degrees to 180 degrees 
        { //pos2 = 60;                                 // in steps of 1 degree 
          myservo1.write(pos);              // tell servo to go to position in variable 'pos' 
          myservo2.write(pos1);
          /*if(30<pos2<60){
          myservo3.write(pos2);}
          pos2--;*/
          delay(80);                       // waits 15ms for the servo to reach the position 
        } 
        for(pos2=60 ; pos2 < 100 ; pos2 += 1)
        {
          myservo0.write(pos2);
        }
        
     } else if (BluetoothData=='0' && old != '0'){// if number 0 pressed ....
      digitalWrite(ledpin,LOW);
      CustomSerial.println("Stop");
      myservo0.write(100);
      myservo1.write(0);
      myservo2.write(160);
      myservo3.write(60);
      delay(500);
     }
   
  old = BluetoothData;
  delay(100);// prepare for next data ...
}

//get instruction from bluetooth module
char getInstruction() {
  char c,junk;
  Serial.println("reading");
  for(int i=0; i<10; i++) {
    Serial.print("**");
    junk = CustomSerial.read();
    Serial.println(junk);
  }
  Serial.print("|");
  c = CustomSerial.read();
  Serial.print(c);
  Serial.print("|**");
  junk = CustomSerial.read();
  Serial.println("**");
  return c;
}

// callback for received data
void receiveData(int byteCount){

  while(Wire.available()) {
    number = Wire.read();
    Serial.print("data received: ");
    Serial.println(number);

    if (number == 1){

      if (state == 0){
        digitalWrite(ledpin, HIGH); // set the LED on
        state = 1;
      }
      else{
        digitalWrite(ledpin, LOW); // set the LED off
        state = 0;
      }
     }
  }
}

// callback for sending data
void sendData(){
  Wire.write(number);
}


//get distnace from Ultrasonic sensor
void getDistance(){
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  if (distance < 4) {  // This is where the LED On/Off happens
    digitalWrite(led,HIGH); 
  digitalWrite(led2,LOW);
  }
  else {
    digitalWrite(led,LOW);
    digitalWrite(led2,HIGH);
  }
  Serial.print(distance);
  Serial.println(" cm");
  
  CustomSerial.println(distance);
  delay(500);
}

void getTemp(){
  float thermopileValue = analogRead(THERMOPILE);
  float thermistorValue = analogRead(THERMISTOR);
  
  // work out thermistor temp from reading
  float v1 = (thermistorValue / 1024) * 5; // source voltage is 5v so reading is fraction of that
  float r = -(v1*1000)/(v1-5); // to get the resistance
  float ambtemp = a + b * sqrt(1+c*r) + d*pow(r,5) + e*pow(r,7); // ambient temp
  
  float comp = k * (pow(ambtemp,4-delta)-pow(reftemp,4-delta));  // equivalent thermopile V for amb temp
  
  // calculate the thermopile temp
  float v2 = (thermopileValue / 1024) * 5 + comp - verr - shiftv; // thermopile voltage
  float objtemp = pow((v2+k*pow(ambtemp,4-delta))/k, 1/(4-delta)); // object temp
  
  Serial.print("thermopile value = ");
  Serial.println(thermopileValue);
  Serial.print("thermistor value = ");
  Serial.println(thermistorValue);
  Serial.print("ambient temp = ");
  Serial.println(ambtemp);
  Serial.print("object temp = ");
  Serial.println(objtemp);
  Serial.println();

  delay(2000);
}
