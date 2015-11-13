#include <XBee.h>
#include <Wire.h>
#include <Servo.h> 
#include "mytypes.h"                // for the data structures defined by users and prototype declaration for functions used these data structures
#include "DHT.h"                    // Lib for temperature sensor
//ultra sonic sensor pin for Front
#define trigPinF 10
#define echoPinF 11



#define STILL_NUM     1500
#define DELTA_SPEED   30
#define MAX_SPEED     200
#define BACK_SPEED    200
#define ROT_SPEED     200                           /* do not change this speed */
#define DIST_STOP     15                            // robot will stop when distance to front < DIST_STOP 
#define DIST_FAR_STOP 30
#define RIGHT_CALIBERATE 3
#define LEFT_CALIBERATE 1



#define  DEFAULT_ACTION    11
#define  CMD_START    20


#define SLAVE_ADDRESS 0x04



const double TimeHalfRound = 2.3;
const double rotateFactor = 0.40;
const double RIGHT_ANGLE = PI/2.0;                  // 90 degree
const double TINY_ANGLE = 20*PI/180;
long duration;                                      //for distance sensor
int gPins[3][2]={ { trigPinF,echoPinF } , {trigPinL,echoPinL },{ trigPinR,echoPinR } };

float t = 0;


uint8_t cmd = DEFAULT_ACTION;
uint8_t serverCmd = 0;            //DEFAULT_ACTION for do nothing
                                  //5 for move forward
                                  //6 for move backward
                                  //7 for turn left
                                  //8 for turn rightw
                                  //9 for stop

uint8_t request = DEFAULT_ACTION;



uint8_t i = 0;
uint8_t readFlag = 0;
Servo servoLeft;
Servo servoRight;




//--------------------------------callback function for I2C---------------------------------
// callback for received data
void receiveData(int byteCount){

    while(Wire.available()) {
        serverCmd = Wire.read();                   //read from PI to get action value
        Serial.print("serverCmd received: ");
        Serial.println(serverCmd);
        if(serverCmd<5)
          request = serverCmd;
        else
          cmd = serverCmd;       
    }
    readFlag = 1;
}


// callback for sending data
void sendData(){                                //send x, y, direc, pwm back to pi, after receiving the action
  if(request!=DEFAULT_ACTION){
    Wire.write(???);
    //Wire.write(rx64data[0]);
    //Serial.println("writing to pi");
    //Serial.println(rx64data[request]);
    request=DEFAULT_ACTION;
  }
  else{
    Wire.write(0xff);
  }
}
//------------------------------------------------------------------------------------------


//-------------------------  functins related to robot control --------------------------------
RobotInfo  gCurInfo;

long readDistSensor(int trigpin, int echopin){
  long distance=0;
  digitalWrite(trigpin, LOW);                     // Added this line
  delayMicroseconds(2);                           // Added this line
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);                          // Added this line
  digitalWrite(trigpin, LOW);
  duration = pulseIn(echopin, HIGH);
  distance = (duration/2) / 29.1;
  return (distance);
}


long getDistance( Dire mdir ) {
  long dt = readDistSensor( gPins[mdir][0],gPins[mdir][1] );
  //Serial.println("I am here 1.1.1");
  while (( dt <=1 ) ||(dt >=1000) ) {
    dt = readDistSensor( gPins[mdir][0],gPins[mdir][1] );
    Serial.print("Direction=");
    Serial.println(mdir);
    Serial.print(",value=");
    Serial.println(dt);
    delay(10);
  }
  //Serial.println("I am here 1.2");
  return dt;
}


void setRobotSpeed( int curSpeed ){
  servoLeft.writeMicroseconds(STILL_NUM + curSpeed +LEFT_CALIBERATE);
  servoRight.writeMicroseconds(STILL_NUM- curSpeed - RIGHT_CALIBERATE); 
  //delay(250);                        //xiao bin commented it out, since we check sensor to stop
}

void stayput(){                                    // stop the robot
  servoLeft.writeMicroseconds(STILL_NUM);
  servoRight.writeMicroseconds(STILL_NUM);
  delay(100);
}


inline double getTime2Delay( double angle) {
  return( rotateFactor* TimeHalfRound * angle /PI *1000);       /* return in millisecond */
}

void turnLeft(double angle){
  double delayTime = getTime2Delay(angle);
  stayput();
  servoLeft.writeMicroseconds(STILL_NUM-ROT_SPEED);
  servoRight.writeMicroseconds(STILL_NUM-ROT_SPEED);
  delay(delayTime);
  stayput();  
}

void turnRight(double angle){
  double delayTime = getTime2Delay(angle);
  stayput();
  servoLeft.writeMicroseconds(STILL_NUM + ROT_SPEED);
  servoRight.writeMicroseconds(STILL_NUM+ ROT_SPEED);
  delay(delayTime*1.2);
  stayput(); 
}


// assume g_angle =0 before  this call
void turnAngle( double angle ) {
  // drive servor
  if( angle < 0.0 ) 
    turnLeft(-angle);
  else if (angle > 0.0 ) {
    turnRight(angle);
  }
  stayput();  
}


//----------------------------setup function-------------------------
void setup() {

    Serial.begin(9600);
    xbee.setSerial(Serial);  
    dht.begin();
    
    servoLeft.attach(13);   
    servoRight.attach(12);     
    servoLeft.writeMicroseconds(STILL_NUM);
    servoRight.writeMicroseconds(STILL_NUM);
    pinMode(trigPinF, OUTPUT);
    pinMode(echoPinF, INPUT);
    pinMode(trigPinL, OUTPUT);
    pinMode(echoPinL, INPUT);
    pinMode(trigPinR, OUTPUT);
    pinMode(echoPinR, INPUT);
    
    gCurInfo.direc = 0;
    gCurInfo.curSpeed = 0;
    gCurInfo.oldSpeed = 0;
    gCurInfo.stop4close = 0;
    Serial.print("SETUP");
    delay(5000);

}



void updateCurInfo() {

  long d2Front   = getDistance(FRONT);

  //Serial.println("I am here 1.1");
  //Serial.print(" front * 1 * =");
  //Serial.println(d2Front);
  if( gCurInfo.curSpeed <0 ) {
    backCount++;
    if ( backCount >50) {
      stayput();
      gCurInfo.curSpeed = 0;
    }
  }
  
  if( d2Front <= DIST_STOP ) {
    delay(10);
    d2Front = getDistance(FRONT);
    if( d2Front <=  DIST_STOP )    {    // confirm
      stayput();
      if(gCurInfo.stop4close ) {
        gCurInfo.oldSpeed = gCurInfo.curSpeed;          // remember the speed before stop    
        gCurInfo.curSpeed = 0;
        gCurInfo.stop4close = 1;
      }
    } else {
      // do nothing
    }
  } 
  
  
/*   // get x,y of robot
  switch (gCurInfo.direc ) {
    case 0 :  
      gCurInfo.x =  WALL_X_LEN - d2Right;
      gCurInfo.y =  d2Front;
      break; 
      
    case 1:
      gCurInfo.x =  d2Front; 
      gCurInfo.y =  d2Right;
      break;
    
    case 2:
      gCurInfo.x =  d2Right;
      gCurInfo.y =  WALL_Y_LEN - d2Front;
      break;
     
    case 3:
      gCurInfo.x =  WALL_X_LEN - d2Front; 
      gCurInfo.y =  WALL_Y_LEN - d2Right;
      break;
      
    default:        
      Serial.print("Wrong direction");
      break;
  } */
  
  
/*   Serial.println("I am here 1.2");
  Serial.print(" front 2 =");
  Serial.print(d2Front);
  Serial.print(", right=");
  Serial.print(d2Right);
  Serial.print(", stop flag = ");
  Serial.print(gCurInfo.stop4close);
  Serial.print(", currrent speed = ");
  Serial.print(gCurInfo.curSpeed);
  Serial.print(", old speed = ");
  Serial.print(gCurInfo.oldSpeed);
  Serial.println(); */
}


void nothing() {
  // do nothing.
  Serial.println("Do nothing");
}

void startUp() {
  Serial.print("Start up");
  if( gCurInfo.stop4close )
    return;
  if( gCurInfo.curSpeed == 0 ) {             // from still to move, if move then do not change speed 
    gCurInfo.curSpeed = DELTA_SPEED;
    setRobotSpeed( gCurInfo.curSpeed );
  }
}

void stayStop() {
    Serial.print("Stop");
    gCurInfo.curSpeed = 0;
    gCurInfo.oldSpeed = 0;    
    setRobotSpeed( gCurInfo.curSpeed );
}

void goLeft() {
    Serial.print("Turn to left");
    gCurInfo.direc = (gCurInfo.direc +1)&3;     // &3 =%4
    turnLeft(RIGHT_ANGLE);
    /*
    if( gCurInfo.stop4close ) {
      gCurInfo.curSpeed = gCurInfo.oldSpeed;    // restore the speed before stop if stop because too close to wall
    } */
    gCurInfo.curSpeed = 0;
    gCurInfo.stop4close = 0;
    setRobotSpeed( gCurInfo.curSpeed );
}

void goRight() {
    Serial.print("Turn to right");
    gCurInfo.direc = (gCurInfo.direc -1)&3;     // &3 =%4
    turnRight(RIGHT_ANGLE);
    /* if( gCurInfo.stop4close ) {
      gCurInfo.curSpeed = gCurInfo.oldSpeed;    // restore the speed before stop if stop because too close to wall
    }  */
    gCurInfo.curSpeed = 0;  
    gCurInfo.stop4close = 0;    
    setRobotSpeed( gCurInfo.curSpeed );
}

void stepOn() {
    Serial.print("Accelerate !");
    if( gCurInfo.stop4close )
      return;  
    gCurInfo.curSpeed += DELTA_SPEED;
    if ( gCurInfo.curSpeed > MAX_SPEED) {
      gCurInfo.curSpeed = MAX_SPEED;
    }
    setRobotSpeed( gCurInfo.curSpeed );
}

void slowDown() {
    Serial.print("Slow Down");
    if ( gCurInfo.curSpeed >= DELTA_SPEED ) {            //if you want backward , can replace >=DELTA_SPEED with > -MAX_SPEED
      gCurInfo.curSpeed -= DELTA_SPEED;
    } else {
      return ;
    }
    setRobotSpeed( gCurInfo.curSpeed );
  }

void backWard() {
    stayput();
    Serial.print("backWard to 20+ cm");
    gCurInfo.curSpeed = - DELTA_SPEED;
    backCount = 0;
    setRobotSpeed( gCurInfo.curSpeed );  
  }

void tinyLeft(){
  double delayTime = getTime2Delay(TINY_ANGLE);
  stayput();
  servoLeft.writeMicroseconds(STILL_NUM-ROT_SPEED);
  delay(delayTime);
  stayput();  
  setRobotSpeed( gCurInfo.curSpeed );
}

void tinyRight(){
  double delayTime = getTime2Delay(TINY_ANGLE);
  stayput();
  servoLeft.writeMicroseconds(STILL_NUM + ROT_SPEED);
  delay(delayTime);
  stayput();
  setRobotSpeed( gCurInfo.curSpeed );
}

typedef void (*funcPt)();
funcPt robotActions[] = { &nothing , &startUp, &stayStop, &goLeft, &goRight, &stepOn, &slowDown ,&backWard ,&tinyLeft, & tinyRight};
const int actionNum = sizeof(robotActions)/sizeof(robotActions[0]);

void  robotDoSomething(uint8_t  cmd ) {
  Serial.print("get a command:");
  Serial.println(cmd);
  if(cmd<CMD_START) {
    Serial.print("Not a command:");
    return;
  }
  cmd-=CMD_START;
  if (cmd >= actionNum ) {
    Serial.print("Wrong command:");
    Serial.println(cmd);
    return ;
  }
  robotActions[cmd]();
}

#define TEST
//--------------------------------main loop--------------------------------//
void loop() 
{ 
    updateCurInfo();
   //Serial.println("I am here 1");
   if (ReportFlag == 0)
   {
/*    #if TEST 
      Serial.println("I am here 2");
      payload[0]=USR_TX_GET;
      payload[1]=gCurInfo.x;             //x value
      payload[2]=gCurInfo.y;             //y value
      payload[3]=gCurInfo.direc;          
      payload[4]=(uint8_t)(gCurInfo.curTemp);
      Serial.print("temperature=");
      Serial.println(payload[4]);
  #else
//for testing-----------------------------------------
      payload[0]=USR_TX_GET;
      payload[1]=5;             //x value
      payload[2]=6;             //y value
      payload[3]=7;          
      payload[4]=8;         
    #endif */
//for testing end-------------------------------------            
      //payloadLen=5;
      //Serial.print("LOOP...");
      //Serial.println(IN1);
  
      
      
      ReportFlag = 1; //after sending out (x,y) ReportFlag taggles to let car recieve ACK from Pi
      
   }
   else
   {  Serial.println("LOOP3..");
      
          Serial.println(Action);
          robotDoSomething(Action);
          /* Serial.println(Action);
          Serial.print("rx64data[1]=");
          Serial.println(" "); */
//        i = 0;

      }
      ReportFlag = 0;
    } 
    delay(50);
}