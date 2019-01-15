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
#define responseSpeed 10 // Speed of change of delayed input *change to step rate
#define swapSpeed 1 // Speed at which couch can change direction.

int directional = 0;
int turningDirection = 0; //Left = -1, Right = 1
int started;
int centreX = 512;
int centreY = 512;
int input[2]; // X = [0], Y = [1]
//unsigned long currentTime;
//unsigned long oldTime;

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
    int JoyX;
    int JoyY;
    while(true){
        if (digitalRead(StartPin)){
            JoyX = analogRead(JoyXPin);
            JoyY = analogRead(JoyYPin);
            if (JoyX > 512 - deadZone && JoyX < 512 + deadZone) {
                Serial.print("Running\n");
                break;
            }
            Serial.println("Joystick not inside deadzone.");
            Serial.print("X: ");
            Serial.print(JoyX);
            Serial.print(", Y: ");
            Serial.print(JoyY);
            Serial.println();
        }
    }
    centreX = JoyX;
    centreY = JoyY;
}

int getInput(int inputPin, int centre){
    int JoyIn = analogRead(inputPin);
    int JoyOut;
    if (JoyIn > centre + deadZone){ //Joystick pushed forward
        // Calculate the distance from the edge of the dead zone and map to range 0 - 255
        JoyOut = map((JoyIn - (centre + deadZone)), 0, centre, 0, 255);
        // Serial.println(JoyOut);
    } 
    else if (JoyIn < centre - deadZone){ //Joystick pushed back
        // Calculate the distance from the edge of the dead zone and make positive
        JoyOut = -1 * map((JoyIn - (centre - deadZone)) * -1, 0, centre, 0, 255);
        // Serial.println(JoyOut);
    }
    else{
        JoyOut = 0;
        // Serial.println(JoyOut);
    }

    // if (abs(JoyOut - JoyOutSmooth) < (timeChange * responseSpeed)){ // If the amount to be added is more than
    //     JoyOutSmooth = JoyOut; // the difference between the input and the current, make them equal, stops over shooting
    // } else {
    //     if(JoyOut > 0){
    //         JoyOutSmooth += (timeChange * responseSpeed); // Delta time will adjust the response speed 
    //     } else if(JoyOut < 0){
    //         JoyOutSmooth -= (timeChange * responseSpeed); // so that exactly responseSpeed units will be added each second
    //     }
    // }  
    return JoyOut;
}

// long checkTime(){
//     currentTime = micros();
//     long timeChange = currentTime - oldTime; // Change into seconds
//     oldTime = currentTime;
//     return timeChange;
// }

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
    int wheelVal = Y - X;
    if(wheelVal < 0){
        wheelVal = 0;
    }
    return wheelVal;
}


void loop(){
    if(!started){
        startup(); // Loops inside itself
        started = 1;
        // oldTime = micros();
    }
    // long timeChange = checkTime();
    input[0] = getInput(JoyXPin, centreX);
    input[1] = getInput(JoyYPin, centreY);
    directional = getOutputDirection(input[1]);
    if(directional == 1){
        digitalWrite(outDirPinL, 1);
        digitalWrite(outDirPinR, 1);
    }
    else if(directional == -1){
        digitalWrite(outDirPinL, 0);
        digitalWrite(outDirPinR, 0);       
    }
    else {
        if(turningDirection == 1){
            digitalWrite(outDirPinL, 0);
            digitalWrite(outDirPinR, 1);
            analogWrite(outPWMPinL, abs(input[0])/2);
            analogWrite(outPWMPinR, abs(input[0])/2);
        } else if(turningDirection == -1){
            digitalWrite(outDirPinL, 1);
            digitalWrite(outDirPinR, 0);
            analogWrite(outPWMPinL, abs(input[0])/2);
            analogWrite(outPWMPinR, abs(input[0])/2);
        }
    }

    int slowWheel = calculateTurningWheel(abs(input[0]), abs(input[1]));
    int fastWheel = abs(input[1]);
    turningDirection = getOutputDirection(input[0]);
    if(turningDirection == 1){
        //Serial.println("Going right");
        analogWrite(outPWMPinL, fastWheel);
        analogWrite(outPWMPinR, slowWheel);
    }
    else if(turningDirection == -1){
        //Serial.println("Going left");
        analogWrite(outPWMPinL, slowWheel);
        analogWrite(outPWMPinR, fastWheel);
    } 
    else if(turningDirection == 0){
        if(directional == 1){
            //Serial.println("Going forward");
        }
        else if(directional == -1){
            //Serial.println("Going back");
        }
        else if(directional == 0){
            //Serial.println("Stationary");
        }
        Serial.println(fastWheel);
        analogWrite(outPWMPinL, fastWheel);
        analogWrite(outPWMPinR, fastWheel);
    }
    //Serial.println(slowWheel);
    // Serial.print("X: ");
    // Serial.print(input[0]);
    // Serial.print("| Y:");
    // Serial.print(input[1]);
    // Serial.println();
    delay(10);
}
