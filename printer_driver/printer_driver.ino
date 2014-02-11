// RAMPS 1.4 pin definitions
#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

#define E_STEP_PIN         26
#define E_DIR_PIN          28
#define E_ENABLE_PIN       24

#define SDPOWER            -1
#define SDSS               53
#define LED_PIN            13

#define FAN_PIN            9

#define PS_ON_PIN          12
#define KILL_PIN           -1

#define HEATER_0_PIN       10
#define HEATER_1_PIN       8
#define TEMP_0_PIN          13   // ANALOG NUMBERING
#define TEMP_1_PIN          14   // ANALOG NUMBERING

#define maxBuffer  64 //maximum size of the buffer
#define stepsPerRev 20000 //steps per revolution
#define distPerStep 0.0023 //distance (in mm) per step

char buffer[maxBuffer]; //stores current line
int charsRead = 0; //counts characters read
int positioning = 91; //90=absolute positioning, 91=relative positioning
int laserfiring = 0; //am I firan mah laser?
float curX = 0; //current X,
float curY = 0; //Y,
float curZ = 0; //and Z positions

void setup()
{
  Serial.begin(9600); //sets up serial communication
  
  pinMode(LED_PIN  , OUTPUT);
  
  pinMode(X_STEP_PIN  , OUTPUT);
  pinMode(X_DIR_PIN    , OUTPUT);
  pinMode(X_ENABLE_PIN    , OUTPUT);
  
  pinMode(Y_STEP_PIN  , OUTPUT);
  pinMode(Y_DIR_PIN    , OUTPUT);
  pinMode(Y_ENABLE_PIN    , OUTPUT);
  
  pinMode(Z_STEP_PIN  , OUTPUT);
  pinMode(Z_DIR_PIN    , OUTPUT);
  pinMode(Z_ENABLE_PIN    , OUTPUT);
  
  digitalWrite(X_ENABLE_PIN    , LOW);
  digitalWrite(Y_ENABLE_PIN    , LOW);
  digitalWrite(Z_ENABLE_PIN    , LOW);
  newLine(); //prepares to recieve a new line
}

void newLine()
{
  charsRead=0; //resets character count
}

float commandParse(char code) 
{
  char *ptr=buffer;
  while(ptr && *ptr && ptr<buffer+charsRead) {
    if(*ptr==code) {
      return atof(ptr+1);
    }
    ptr=strchr(ptr,' ')+1;
  }
  return -1;
} 


void loop()
{
 
 while(Serial.available()>0) //while there are characters to read
 {
  char c=Serial.read(); //read next character
  if (charsRead<maxBuffer) buffer[charsRead++]=c; //adds character to buffer if there is room
  if ( buffer[charsRead-1]==10) break; //stops reading at newline
 }
 
 if(charsRead>0 && buffer[charsRead-1]==10) //if line has been read fully
   {
     Serial.print("Line End");
     buffer[charsRead]=0; //terminates the buffer
     executeCommand(); //does what the gcode says
     newLine(); //prepares for next line
   }
}


void executeCommand()
{
  int command=commandParse('G');
  switch(command)
  {
    case 1: {
            Serial.print("Moving");
            float dX=commandParse('X');//move somewhere
            float dY=commandParse('Y');
            float dZ=commandParse('Z');
            Move(dX,dY,dZ);
            break;
    }
    case 90: 
    {
      positioning=90; //absolute mode
      break;
    }
    case 91:
    {
      positioning=91; //relative mode
    }
  }
  command=commandParse('M');
  switch(command)
  {
    case 101: 
    {
      laserfiring=1;//laser on
      break;
    }
    case 103:
    {
      laserfiring=0;//laser off
      break;
    }
  }
  
}

void Move(float dX,float dY,float dZ)
{
  Serial.print(dX);
  Serial.print(" ");
  Serial.print(dY);
  Serial.print(" ");
  Serial.print(dZ);
  if(positioning==90)
  {
    dX=dX-curX;
    dY=dY-curY;
    dZ=dZ-curZ;
  }
  int dZ_Steps=dZ/distPerStep;
  if(dZ>0)
  {
    digitalWrite(Z_DIR_PIN,HIGH);
    for(int i=0;i<abs(dZ_Steps);i++)
    {
      digitalWrite(Z_STEP_PIN,HIGH);
      digitalWrite(Z_STEP_PIN,LOW);
    }
  }
  if(dZ<0)
  {
    digitalWrite(Z_DIR_PIN,LOW);
    for(int i=0;i<abs(dZ_Steps);i++)
    {
      digitalWrite(Z_STEP_PIN,HIGH);
      digitalWrite(Z_STEP_PIN,LOW);
    }
  }
  float dA=dX+dY; //CoreXY definitions
  long dA_Steps=dA/distPerStep;
  float dB=dX-dY; //CoreXY definitions
  long dB_Steps=dB/distPerStep;
  Serial.print(dA);
  Serial.print(" ");
  Serial.print(dB);
  Serial.print(" ");
  Serial.print(dA_Steps);
  Serial.print(" ");
  Serial.print(dB_Steps);
  if(dA<0)
  {
    Serial.print("test");
  }
  if(abs(dA)>=abs(dB))
  {
    float stepratio=dB/dA;
    int iBPrev=0;
    for(long i=0; i<abs(dA_Steps); i++)
    {
      int iB=i*stepratio;
      if(abs(iB-iBPrev)>=1)
      {
        if(dB>0)
        {
          digitalWrite(Z_DIR_PIN, HIGH);
          digitalWrite(Z_STEP_PIN, HIGH);
          digitalWrite(Z_STEP_PIN, LOW);
          delayMicroseconds(5);
        }
        if(dB<0)
        {
          digitalWrite(Z_DIR_PIN, LOW);
          digitalWrite(Z_STEP_PIN, HIGH);
          digitalWrite(Z_STEP_PIN, LOW);
          delayMicroseconds(5);
        }
        iBPrev=iB;
      }
      if(dA>0)
      {
        
        digitalWrite(X_DIR_PIN, HIGH);
        digitalWrite(X_STEP_PIN, HIGH);
        digitalWrite(X_STEP_PIN, LOW);
        delayMicroseconds(5);
      }
      if(dA<0)
      {
        digitalWrite(X_DIR_PIN, LOW);
        digitalWrite(X_STEP_PIN, HIGH);
        digitalWrite(X_STEP_PIN, LOW);
        delayMicroseconds(5);
      }
    }
  }
  if(abs(dB)>abs(dA))
  {
    float stepratio=dA/dB;
    int iAPrev=0;
    for(long i=0; i<abs(dB_Steps); i++)
    {
      int iA=i*stepratio;
      if(abs(iA-iAPrev)>=1)
      {
        if(dA>0)
        {
          digitalWrite(X_DIR_PIN, HIGH);
          digitalWrite(X_STEP_PIN, HIGH);
          digitalWrite(X_STEP_PIN, LOW);
          delayMicroseconds(5);
        }
        if(dA<0)
        {
          digitalWrite(X_DIR_PIN, LOW);
          digitalWrite(X_STEP_PIN, HIGH);
          digitalWrite(X_STEP_PIN, LOW);
          delayMicroseconds(5);
        }
        iAPrev=iA;
      }
      if(dB>0)
      {
        
        digitalWrite(Z_DIR_PIN, HIGH);
        digitalWrite(Z_STEP_PIN, HIGH);
        digitalWrite(Z_STEP_PIN, LOW);
        delayMicroseconds(5);
      }
      if(dB<0)
      {

        digitalWrite(Z_DIR_PIN, LOW);
        digitalWrite(Z_STEP_PIN, HIGH);
        digitalWrite(Z_STEP_PIN, LOW);
        delayMicroseconds(5);
      }
    }
  }
  Serial.print("done");
  curX=curX+dX;
  curY=curY+dY;
  curZ=curZ+dZ;
}

