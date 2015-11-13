typedef struct tag1 {   
  int x;
  int y;
  int direc;
  int oldSpeed;                          // speed before turn or stop because too close to wall
  int curSpeed;                          //speed , + STILL_NUM = pwm to control robot move
  float curTemp;
  int stop4close;                        // stop because the robot is too close to the wall
} RobotInfo ;

RobotInfo etCurInfo();                  // if do not declare this prototype, the compiler will complain

typedef enum nnn_tag{
  FRONT= 0 ,
  LEFT,
  RIGHT
} Dire;

long getDistance( Dire mdir ) ;



