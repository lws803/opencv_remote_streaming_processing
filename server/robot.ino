/*
  Project : Robot control program
  Author  : Bin Xiao, Xu Guo, Peter Guan
  Date    : 11/12/2015
  Version : 1
  
  Version 0 is developed by Bin Xiao, Xu and Peter made some modification for new project usage.

  CopyRight:
            Reserved by the author.
            Please keep this claim for academic usage.
            For commercial use, please contact the author.
*/
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Servo.h>
#include "mytypes.h"                // for the data structures defined by users and prototype declaration for functions used these data structures

//SoftwareSerial mySerial(10, 11); // RX, TX

//ultra sonic sensor pin for Front
//#define trigPinF 2
//#define echoPinF 3



#define STILL_NUM              1500
#define DELTA_SPEED            30
#define MAX_SPEED              200
#define BACK_SPEED             200
#define ROT_SPEED              60                            //Xu changed this from 200 to 60
//#define DIST_STOP              15                            // robot will stop when distance to front < DIST_STOP 
#define DIST_FAR_STOP          30
#define RIGHT_CALIBERATE       3
#define LEFT_CALIBERATE        1



#define  DEFAULT_ACTION    15
#define  CMD_START    13


#define SLAVE_ADDRESS 0x04



const double TimeHalfRound = 2.3;
const double rotateFactor = 0.40;
const double RIGHT_ANGLE = PI / 2.0;                // 90 degree
const double TINY_ANGLE = 20 * PI / 180;
long duration;                                      //for distance sensor

//------for distance detecting-------
//------not using this----------
//int gPins[3][2]={ { trigPinF,echoPinF } , {trigPinL,echoPinL },{ trigPinR,echoPinR } };
//int gPins[1][2] = { { trigPinF, echoPinF } };

float t = 0;


uint8_t cmd = DEFAULT_ACTION;
uint8_t serverCmd = DEFAULT_ACTION;             //DEFAULT_ACTION for do nothing
												//1 startUp
												//2 stop
												//3 for turn left
												//4 for turn right
												//5 speedup
												//6 speeddown
												//7 for move backward
												//8 tinyLeft
												//9 tinyRight

uint8_t request = DEFAULT_ACTION;
uint8_t action = DEFAULT_ACTION;

int backCount = 0;   //for auto backWard, not using


uint8_t i = 0;
//uint8_t readFlag = 0;
Servo servoLeft;
Servo servoRight;




//--------------------------------callback function for I2C---------------------------------
// callback for received data
void receiveData(int byteCount) {

  while (Wire.available()) {
    serverCmd = Wire.read();                   //read from PI to get action value
    Serial.print("serverCmd received: ");
    Serial.println(serverCmd);
    if (serverCmd > 9)
      request = serverCmd;
    else
      action = serverCmd;
  }
  //readFlag = 1;
}


// callback for sending data
void sendData() {                               //send x, y, direc, pwm back to pi, after receiving the action
  if (request != DEFAULT_ACTION) {
    Wire.write(0);
    //Wire.write(rx64data[0]);
    //Serial.println("writing to pi");
    //Serial.println(rx64data[request]);
    request = DEFAULT_ACTION;
  }
  else {
    Wire.write(0xff);
  }
}
//------------------------------------------------------------------------------------------


//-------------------------  functins related to robot control --------------------------------
RobotInfo  gCurInfo;

long readDistSensor(int trigpin, int echopin) {
  long distance = 0;
  digitalWrite(trigpin, LOW);                     // Added this line
  delayMicroseconds(2);                           // Added this line
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);                          // Added this line
  digitalWrite(trigpin, LOW);
  duration = pulseIn(echopin, HIGH);
  distance = (duration / 2) / 29.1;
  return (distance);
}


/* long getDistance( Dire mdir ) {
  long dt = readDistSensor( gPins[mdir][0], gPins[mdir][1] );
  //Serial.println("I am here 1.1.1");
  while (( dt <= 1 ) || (dt >= 1000) ) {
    dt = readDistSensor( gPins[mdir][0], gPins[mdir][1] );
    //    Serial.print("Direction=");
    //    Serial.println(mdir);
    //    Serial.print(",value=");
    //    Serial.println(dt);
    //    delay(10);
  }
  //Serial.println("I am here 1.2");
  return dt;
  } */


void setRobotSpeed( int curSpeed ) {
  servoLeft.writeMicroseconds(STILL_NUM + curSpeed + LEFT_CALIBERATE);
  servoRight.writeMicroseconds(STILL_NUM - curSpeed - RIGHT_CALIBERATE);
  //delay(250);                        //xiao bin commented it out, since we check sensor to stop
}

void stayput() {                                   // stop the robot
  servoLeft.writeMicroseconds(STILL_NUM);
  servoRight.writeMicroseconds(STILL_NUM);
  delay(100);
}


inline double getTime2Delay( double angle) {
  return ( rotateFactor * TimeHalfRound * angle / PI * 1000);   /* return in millisecond */
}

void turnLeft(double angle) {
  double delayTime = getTime2Delay(angle);
  stayput();
  servoLeft.writeMicroseconds(STILL_NUM - ROT_SPEED);
  servoRight.writeMicroseconds(STILL_NUM - ROT_SPEED);
  delay(delayTime);
  stayput();
}

void turnRight(double angle) {
  double delayTime = getTime2Delay(angle);
  stayput();
  servoLeft.writeMicroseconds(STILL_NUM + ROT_SPEED);
  servoRight.writeMicroseconds(STILL_NUM + ROT_SPEED);
  delay(delayTime * 1.2);
  stayput();
}


// assume g_angle =0 before  this call
void turnAngle( double angle ) {
  // drive servor
  if ( angle < 0.0 )
    turnLeft(-angle);
  else if (angle > 0.0 ) {
    turnRight(angle);
  }
  stayput();
}


//----------------------------setup function-------------------------
void setup() {
  
  Serial.begin(9600);
  Serial.println("SETUP");
  Wire.begin(SLAVE_ADDRESS);

// define callbacks for i2c communication
  Wire.onReceive(receiveData); 
//Wire.onRequest(sendData);     //for sending data from slave to master, not using
//
  servoLeft.attach(13);
  servoRight.attach(12);
  servoLeft.writeMicroseconds(STILL_NUM);
  servoRight.writeMicroseconds(STILL_NUM);
//    pinMode(trigPinF, OUTPUT);
//    pinMode(echoPinF, INPUT);
//
//
//gCurInfo.direc = 0;
  gCurInfo.curSpeed = 0;
  gCurInfo.oldSpeed = 0;
//gCurInfo.stop4close = 0;
  
  delay(5000);

Serial.println("SETUPend");
}



void updateCurInfo() {
  Serial.println("updateCurInfo");
  Serial.println("Done-updateCurInfo");
}


void nothing() {
  // do nothing.
  Serial.println("Do nothing");
}

void startUp() {
  Serial.print("Start up");
  if ( gCurInfo.curSpeed == 0 ) {            // from still to move, if move then do not change speed
    gCurInfo.curSpeed = DELTA_SPEED;
    setRobotSpeed( gCurInfo.curSpeed );
  }
}

void getTemp() {
  Serial.print("get getTemp");
  sendData(65);
}


void stayStop() {
  Serial.print("Stop");
  gCurInfo.curSpeed = 0;
  gCurInfo.oldSpeed = 0;
  setRobotSpeed( gCurInfo.curSpeed );
}

void goLeft() {
  Serial.print("Turn to left");
  turnLeft(RIGHT_ANGLE);
  /*
    if( gCurInfo.stop4close ) {
    gCurInfo.curSpeed = gCurInfo.oldSpeed;    // restore the speed before stop if stop because too close to wall
    } */
  gCurInfo.curSpeed = 0;
  //gCurInfo.stop4close = 0;
  setRobotSpeed( gCurInfo.curSpeed );
}

void goRight() {
  Serial.print("Turn to right");
  turnRight(RIGHT_ANGLE);
  /* if( gCurInfo.stop4close ) {
    gCurInfo.curSpeed = gCurInfo.oldSpeed;    // restore the speed before stop if stop because too close to wall
    }  */
  gCurInfo.curSpeed = 0;
  //gCurInfo.stop4close = 0;
  setRobotSpeed( gCurInfo.curSpeed );
}

void stepOn() {
  Serial.print("Accelerate !");
  if ( gCurInfo.stop4close )
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

void tinyLeft() {
  double delayTime = getTime2Delay(TINY_ANGLE);
  stayput();
  servoRight.writeMicroseconds(STILL_NUM - ROT_SPEED);
  //delay(delayTime);
  //stayput();
  //setRobotSpeed( gCurInfo.curSpeed );
}

void tinyRight() {
  double delayTime = getTime2Delay(TINY_ANGLE);
  stayput();
  servoLeft.writeMicroseconds(STILL_NUM + ROT_SPEED);
  //delay(delayTime);
  //stayput();
  //setRobotSpeed( gCurInfo.curSpeed );
}

typedef void (*funcPt)();
funcPt robotActions[] = {&startUp, &stayStop, &goLeft, &goRight, &stepOn, &slowDown , &backWard , &tinyLeft, &tinyRight, &nothing, &getTemp};
const int actionNum = sizeof(robotActions) / sizeof(robotActions[0]);

void  robotDoSomething(uint8_t  cmd ) {
  
  Serial.print("get a command:");
  Serial.println(cmd);
  if (cmd > CMD_START) {
    Serial.print("Not a command:");
    return;
  }
  //cmd-=CMD_START;
  if (cmd >= actionNum ) {
    Serial.print("Wrong command:");
    Serial.println(cmd);
    return ;
  }
  robotActions[cmd]();
}



//--------------------------------main loop--------------------------------//
void loop()
{
//-----code for receiving cmd from USB port,so we can test the robot through pc's serial monitor----- 
  Serial.println("loop start..");
  if (Serial.available()) {
    char s;
    s = Serial.read();
    action = (uint8_t)(s - 48);

  }
//-----end------


  //updateCurInfo();
//-----for debug-----
  Serial.println("LOOP3..");
  Serial.print("action: ");
  Serial.println(action);
//-----end-----
  robotDoSomething(action);
  action = DEFAULT_ACTION;

  delay(200);
}
