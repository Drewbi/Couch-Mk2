/* Motorised couch project
 * Version 2.0
 * By Drew Alexander
 * 
 *
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

int currentDirection = 0; // -1 reverse, 0 neutral, 1 forward

int PWMLeft; // 0 - 255
int DIRLeft; // 0 reverse, 1 forward
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

/* Startup stops the flow of the program until the start button
 * has been pressed and the joyStick is within the deadzone
*/
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

int getInputDirection(int joyIn){
    if (joyIn > 0){ //Joystick pushed forward
        return 1;
    } 
    else if (joyIn < 0){ //Joystick pushed back
        return -1;
    }
    return 0;
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
    }
    else if (inputY < currentY){
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

int getCurrentDirection(int inputY, int speed, int currentDirection){
    if(speed < swapSpeed && inputY == 0){
        return 0; //neutral
    }
    if(speed < swapSpeed && inputY < 0){
        return -1;// negative
    }
    if(speed < swapSpeed && inputY > 0){
        return 1;// negative
    }
    return currentDirection; // Don't change direction
}

int calculateTurningWheel(int X, int Y){
    int wheelVal = Y - abs(X);
    if(wheelVal < 0){
        wheelVal = 0;
    }
    return wheelVal;
}

int getSpeed(int speedL, int speedR)
    if (speedL > speedR){
        return speedL;
    } 
    return speedR;
}

void loop(){
    if(!started){
        startup(); // Loops inside itself
        started = 1;
    }
    rawInputX = getInput(JoyXPin, centreX);
    rawInputY = getInput(JoyYPin, centreY);

    smoothY = accelerationThrottle(smoothY, getTimeDiff());

    currentDirection = getCurrentDirection(smoothY, getSpeed(speedL, speedR), currentDirection);
    if (currentDirection != 0){
        if (getInputDirection(rawInputX) < 0){ // Turning left
            PWMLeft = calculateTurningWheel(abs(rawInputX), smoothY);
            PWMRight = smoothY;
        } else if (getInputDirection(rawInputX) > 0){ // Turning right
            PWMRight = calculateTurningWheel(abs(rawInputX), smoothY);
            PWMLeft = smoothY;
        } else { // Not turning
            PWMLeft = smoothY;
            PWMRight = smoothY;
        }
        if (currentDirection == 1){ // Sets motors to forwards direction
            DIRLeft = 1;
            DIRRight = 1;
        } else { // Sets motors to reverse direction
            DIRLeft = 0;
            DIRRight = 0;
        }
    } else { // Neutral turning
        if (getInputDirection(rawInputX) < 0){ // Turning left
            PWMLeft = abs(rawInputX)/spinSpeedReduction;
            DIRLeft = 0;
            PWMRight = abs(rawInputX)/spinSpeedReduction;
            DIRRight = 1;
        } else if (getInputDirection(rawInputX) > 0){ // Turning right
            PWMLeft = abs(rawInputX)/spinSpeedReduction;
            DIRLeft = 1;
            PWMRight = abs(rawInputX)/spinSpeedReduction;
            DIRRight = 0;
        } else {
            PWMLeft = 0;
            PWMRight = 0;
        }
    }
}
