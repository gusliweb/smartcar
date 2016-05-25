#include <Wire.h>

#include <Smartcar.h>
#include <Servo.h>
#include <Smartcar.h>

//implement thermometer left                                               

//create the object Car
Car car;

//declare components
Odometer encoderLeft, encoderRight;
Gyroscope gyro;
SR04 ultrasonicFront; // define Ultrasonic object
SR04 ultrasonicBack;//values
int speedValue, angle;
int frontObjEncountered, backObjEncountered;//read input for direction

//variable declaration
//for the speed

//for the bluetooth
char in;
int distance=0;
//for the buzzer
int speakerPin =A0; 
int volume=0;

int tempPin= A5;   // attached the temperature pin to analog in A5


void setup() {
  Serial.begin(9600);

  //attach components 
  gyro.attach();
  encoderLeft.attach(2);
  encoderRight.attach(3);
  encoderLeft.begin();
  encoderRight.begin();
  gyro.begin();
  car.begin(encoderLeft, encoderRight, gyro);
  ultrasonicFront.attach(6, 5);
  ultrasonicBack.attach(10,9); 
  pinMode (speakerPin, OUTPUT);
  // attach the temperature sensor 
  pinMode (tempPin, OUTPUT); 

  //ultrasonic
  in=0;
  speedValue=0;
  angle=0;
  frontObjEncountered=0;
  backObjEncountered=0;
 }

void loop(){
  
  getTemp(); // call the function to calcualte temperature value
  
  if( Serial3.available() ){
      in=Serial3.read();
      handleInput();
  }
  else if ( frontIsClear()==false ){  
      speedValue = 0; 
      distance=ultrasonicFront.getDistance();
       if ( distance>15){
          Serial.println(distance);
          speedValue=50;
          car.setSpeed(speedValue);      
          volume=0;
       }
       else{
          Serial.println(distance);
          speedValue=0;
          car.setSpeed(speedValue);       
          for( volume; volume<255; volume++){
             analogWrite (speakerPin, volume);
             delay (100);
          }
          volume=0;
       }
   }
   else if(backIsClear()==false){  
       speedValue = 0;
       distance=ultrasonicBack.getDistance();
       if ( distance>15){
           Serial.println(distance);
           speedValue=50;
           car.setSpeed(speedValue);
           volume=0;
       }
      else{
           Serial.println(distance);
           speedValue=0;
           car.setSpeed(speedValue);
           for( volume; volume<255; volume++){
              analogWrite (speakerPin, volume);
              delay (100);
           }
           volume=0;
         }
       }
}  

double Thermister(int RawADC)
{  //Function to perform the Steinhart-Hart equation
 double Temp;
 Temp = log(((10240000/RawADC) - 10000));
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;              // Convert Kelvin to Celsius
 return Temp;
}

void getTemp()
{
  
  int val;                //Create an integer variable
  double temp;            //Variable to hold a temperature value
  val=analogRead(tempPin);      //Read the analog from A5 and store the value in val
  temp=Thermister(val);   //Runs the  math on the raw analog value
  Serial.println(temp);   //Print the value to the serial port
  delay(100);
  
  
  
  
}
void handleInput() {
  switch(in){
   //go front
   case 'f': if(speedValue<100 && speedValue !=0)
              speedValue = speedValue + 10;
              if(speedValue == 0){
               speedValue = 70;
                }
           car.setSpeed(speedValue);
           car.setAngle(angle);
   break;
   
 //go back
 case 'b': if(speedValue>-100 && speedValue!=0)
              speedValue = -speedValue;
              if(speedValue == 0){
               speedValue = -70;
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
   case 's':
      speedValue = 0;
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
