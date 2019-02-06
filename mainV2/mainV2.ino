/* Motorised couch project
 * Version 2.0
 * By Drew Alexander
 * 
 * This program maps joystick input to a middle centred Forward, Back, Left and Right values
 * and then to smoothed left and right wheel values.
 */

#define JoyYPin 0
#define JoyXPin 1
#define StartPin 2

#define outPWMPinL 3
#define outDirPinL 4
#define outPWMPinR 5
#define outDirPinR 6

#define deadZone 50
#define spinSpeedReduction 2 // Higher number = lower stationary rotation
#define stepRate 10 // Speed of change of delayed input
#define swapSpeed 1 // Speed at which couch can change direction.

// Global Variables
int started;
int centreX = 512;
int centreY = 512;
unsigned long oldTime;

int rawInputX; //-255 to 255, left is negative
int rawInputY; // Back is negative

double smoothY = 0;

double speedL; // Speed recieved from left wheel
double speedR; // Speed recieved from right wheel

int currentDirection; // -1 reverse, 0 neutral, 1 forward

int PWMLeft;
int DIRLeft;
int PWMRight;
int DIRRight;

void setup(){
    Serial.begin(115200);
    pinMode(JoyYPin, INPUT);
    pinMode(JoyXPin, INPUT);
    pinMode(StartPin, INPUT);
    pinMode(StartPin, INPUT);
    pinMode(outPWMPinL, OUTPUT);
    pinMode(outPWMPinR, OUTPUT);
    pinMode(outDirPinL, OUTPUT);
    pinMode(outDirPinR, OUTPUT);

    started = 1;
    Serial.println("Initialized");    
}

void startup(){
    while(true){
        if (digitalRead(StartPin)){
            int JoyX = analogRead(JoyXPin);
            int JoyY = analogRead(JoyYPin);
            if (JoyX > 512 - deadZone && JoyX < 512 + deadZone) {
                Serial.print("Running\n");
                break; // Exits the loop
            }
        }
    }
    centreX = JoyX;
    centreY = JoyY;
    oldTime = micros();
}

int getInput(int inputPin, int centre){
    int joyIn = analogRead(inputPin);
    int joyOut = 0;
    if (joyIn > centre + deadZone){ //Joystick pushed forward (512 - 1024)
        // Calculate the distance from the edge of the dead zone and map to range 0 to 255
        joyOut = map(joyIn, centre + deadZone, 1024, 0, 255);
    } 
    else if (joyIn < centre - deadZone){ //Joystick pulled back (0 - 512)
        // Calculate the distance from the edge of the dead zone (0 to -255)
        joyOut = map(joyIn, 0, centre - deadZone, -255, 0);
    }
    return joyOut;
}

int getDirection(int joyIn, int centre){
    int dirOut = 0;
    if (joyIn > centre + deadZone){ //Joystick pushed forward
        dirOut = 1;
    } 
    else if (joyIn < centre - deadZone){ //Joystick pushed back
        dirOut = -1;
    }
    return dirOut;
}

double accelerationThrottle(double currentY, double timeDiff){
    double joyOutSmooth = currentY;
    yStep = timeDiff * stepRate; // This number will add up to stepRate each second, allowing exact additions to be made
    double inputY = rawInputY;
    // To prevent overshooting of the desired input, this logic makes sure the step to be added does not
    // make currentY larger than inputY. It also makes currentY increase or descrease towards inputY
    if(inputY > currentY){
        if (inputY - (currentY + yStep) >= 0){
            joyOutSmooth += yStep;
        }
    } else if (inputY < currentY){
        if ((currentY + yStep) - inputY >= 0){
            joyOutSmooth -= yStep;
        }
    }
    return joyOutSmooth;
}

double getTimeDiff(){
    unsigned long currentTime = micros();
    double timeDiff = (double)(currentTime - oldTime)/(double)1000000; // Change into seconds
    oldTime = currentTime;
    return timeDiff;
}

int getOutputDirection(int input){
   if(input < 0){
       return -1;//negative position
   }
  else if(input > 0 ){
      return 1;// positive position
  }
  else{
      return 0;// netural postition
  }
}

int calculateTurningWheel(int X, int Y){
    int wheelVal = Y - abs(X);
    if(wheelVal < 0){
        wheelVal = 0;
    }
    return wheelVal;
}


void loop(){
    if(!started){
        startup(); // Loops inside itself
        started = 1;
    }
    rawInputX = getInput(JoyXPin, centreX);
    rawInputY = getInput(JoyYPin, centreY);

    smoothY = accelerationThrottle(smoothY, getTimeDiff())

    if (getDirection(rawInputX, centreX) < 0){
        PWMLeft = calculateTurningWheel(abs(rawInputX), smoothY);
        PWMRight = smoothY;
    } else if (getDirection(rawInputX, centreX) > 0){
        PWMRight = calculateTurningWheel(abs(rawInputX), smoothY);
        PWMLeft = smoothY;
    } else {
        PWMLeft = smoothY;
        PWMRight = smoothY;
    }
    
}
