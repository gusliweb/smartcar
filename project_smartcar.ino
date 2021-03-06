#include <Wire.h>



#include <Ultrasonic.h>

#include <Servo.h>

#include <Smartcar.h>

//#include <NewPing.h>

//#include "Ultrasonic.h"




//implement thermometer left


Car car;
//attach components
Odometer encoderLeft, encoderRight;
Gyroscope gyro;
SR04 ultrasonicFront; // define Ultrasonic object
SR04 ultrasonicBack;//values
int speedValue, angle;
int frontObjEncountered, backObjEncountered;//read input for direction
const int TRIGGER_PIN =6;
const int ECHO_PIN=5;


char in;
// wei Li 


int distance=0;
// set up the active buzzer

int speakerPin = A0;
int volume=0;



void setup() {
 Serial.begin(9600);  //initialise components
 gyro.attach();
 encoderLeft.attach(2);
 encoderRight.attach(3);
 encoderLeft.begin();
 encoderRight.begin();
 gyro.begin();
 car.begin(encoderLeft, encoderRight, gyro);
 ultrasonicFront.attach(6, 5);
 ultrasonicBack.attach(10,9);


//  ultrasonicfront ;
 in=0;
 speedValue=0;
 angle=0;
 frontObjEncountered=0;
 backObjEncountered=0;
 
 // wei Li
 //int distance=0;

 pinMode (speakerPin, OUTPUT);
}

void loop() {
if(Serial3.available()){
 in=Serial3.read();
 handleInput();
 
}else{
if(frontIsClear()==false){  
 speedValue = 0;
 //angle = 0;
 //car.setAngle(angle);
 //car.setSpeed(speedValue);
 
 // wei Li code block for collusion check
   distance=ultrasonicFront.getDistance();
 //  distance=ultrasonicBack.Ranging(CM);

       if ( distance>15)
       {
        Serial.println(distance);
        speedValue=50;
        car.setSpeed(speedValue);
        
       
       }

       else
       {
        Serial.println(distance);
        speedValue=0;
        car.setSpeed(speedValue);
        
       
      for( volume; volume<255; volume++)
      {
       analogWrite (speakerPin, 100);
       delay (50);
      }
     
       // volume=0;
     
       }
   
 }
}
}
//handle serial input if there is any
void handleInput() {
 switch(in){
   //go front
 case 'f': if(speedValue<100 && speedValue !=0)
              speedValue = speedValue + 10;
              if(speedValue == 0){
               speedValue = 40;
                }
           car.setSpeed(speedValue);
           car.setAngle(angle);
   break;
   
 //go back
 case 'b': if(speedValue>-100 && speedValue!=0)
              speedValue = -speedValue;
              if(speedValue == 0){
               speedValue = -40;
              }
           car.setSpeed(speedValue);
           car.setAngle(angle);      break;   
           //go right
 case 'r': if(angle<60)
              angle = angle + 15;        
           car.setAngle(angle);
           car.setSpeed(speedValue);
 
  break; 
  case 'l':  if(angle>-60)
              angle = angle - 15;            
           car.setAngle(angle);
           car.setSpeed(speedValue);
   break;
   //rotate left
 case 'a': car.rotate(-80);
 
  break;//rotate right
 case 'd': car.rotate(80);
  break;//stop
 case 's': speedValue = 0;
           angle = 0;
           car.setAngle(angle);
           car.setSpeed(speedValue);
       
}
if(frontIsClear()==false){  
 speedValue = 0;
 angle = 0;
 car.setAngle(angle);
 car.setSpeed(speedValue);
}
} 
// check the distance given by the ultrasonic
//in order to see if the car encounters an object or not in front
boolean frontIsClear(){
frontObjEncountered =  ultrasonicFront.getDistance();
 if(frontObjEncountered > 50)
 return true;
if(frontObjEncountered == 0)
 return true; 
 return false;
}//check the distance given by the ultrasonic
//in order to see if the car encounters an object or not in the back
boolean backIsClear(){
backObjEncountered = ultrasonicBack.getDistance();
if (backObjEncountered > 50)
 return true;
if (backObjEncountered == 0)
 return true;  
 return false;
}
