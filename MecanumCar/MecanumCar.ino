/**
 * This example changes from the [my code of 3d platform](https://github.com/MiaoDX/platformMovingV2/blob/master/serial_platformMoving/serial_SerialMultiSteppers_sketch/serial_SerialMultiSteppers_sketch.ino), luckily, almost all functions are the same
 * Our car also have three dimension movement (forward, left, rotate). And we have no need to take care about the limit control, it is easier than that.
 */

/**
 * This example is changed from [Arduino-CmdMessenger SendAndReceiveArguments](https://github.com/thijse/Arduino-CmdMessenger/blob/master/examples/SendAndReceiveArguments/SendAndReceiveArguments.ino) example.
 * 
 * And the only change is change the `kFloatAddition` into what we do: move multiple steppers
 * 
 * TO use it, one can operate in a stateless way (each and every time reconnect the serial)
 * or in keep alive way, only send and receive command.
 * 
 * It's up to the client, this code do not care at all. But the former is more robust, while the latter can be more friendly in time.
 */

// *** SendandReceiveArguments ***

// This example expands the previous SendandReceive example. The Arduino will now receive multiple 
// and sent multiple float values. 
// It adds a demonstration of how to:
// - Return multiple types status; It can return an Acknowlegde and Error command
// - Receive multiple parameters,
// - Send multiple parameters
// - Call a function periodically

#include <CmdMessenger.h>  // CmdMessenger
#include "PinsAndConfigurationCar.h"
#include "mecanum.h"


// Blinking led variables 
unsigned long previousToggleLed = 0;   // Last time the led was toggled
bool ledState                   = 0;   // Current state of Led
const int kBlinkLed             = 13;  // Pin of internal Led

// Platform specific settings
const char fld_separator = '@';
const char cmd_separator = '#';
#define MOVEDATATYPE double

// Up to 10 steppers can be handled as a group by MultiStepper
MultiStepperDecorateNoLimit steppersDecorate;
Mecanum mecanumCar;

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial, fld_separator, cmd_separator);

// This is the list of recognized commands. These can be commands that can either be sent or received. 
// In order to receive, attach a callback function to these events
enum
{
  // Commands
  kAcknowledge         , // Command to acknowledge that cmd was received
  kError               , // Command to report errors
  kMovement       , // Command to request three relative stpes
  kMovementResult , // Command to report addition result
  
  kHeartBeat,            // addby miaodx
};

// Commands we send from the PC and want to receive on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below.

void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(kHeartBeat, OnHeartBeat);
  cmdMessenger.attach(kMovement, OnMovement);
}

// ------------------  C A L L B A C K S -----------------------

// At first, this is mean to used to stop/init steppers, espicailly when the host closed it's connection for some reason or just want a `STOP` funciton call.
// however, since all the methods are processed one by one, so this stop can be useless.
void makeSteppersStop()
{
  Serial.println("Going to stop the steppers");
  double relativeDistance[4] = { 0,0,0,0 };
  steppersDecorate.moveRelativeDistancesWithPredefinedAccelOrSpeed(relativeDistance);
  int rangeStatusArr[3] = {0,0,0};
  Serial.print("Stop steppers done:");
  Serial.println(rangeStatusArr[0]+rangeStatusArr[1]+rangeStatusArr[2]);  
}


// Called when a received command has no attached function
void OnUnknownCommand()
{
  cmdMessenger.sendCmd(kError,"Command without attached callback");
}

// Called when call for heartbeat
void OnHeartBeat()
{
  makeSteppersStop();
  cmdMessenger.sendCmd(kHeartBeat,"ALL OK IN ARDUINO");
}

void setConstSpeed(double constSpeed=600.0)
{
  //- Set constSpeed
  //double constSpeed = 600.0;
  double constSpeeds[4] = { constSpeed , constSpeed, constSpeed, constSpeed };
  steppersDecorate.setConstSpeeds ( constSpeeds );  

  Serial.print("Now speed1:");
  Serial.println(stepper1Decorate.stepper.speed());
}

// Callback function that responds that Arduino is ready (has booted up)
void OnArduinoReady()
{
  //extra_print();  
  makeSteppersStop();
  cmdMessenger.sendCmd(kAcknowledge,"Arduino ready");
}

// Callback function movement
void OnMovement()
{
  setConstSpeed();
  
  Serial.println("Arduino side, going to receive data for moving");
  unsigned long time = micros ();
  Serial.print ( "Before:" );
  Serial.println ( time );
  
  // Retreive first parameter as double
  double x_dis = cmdMessenger.readDoubleArg();
  double y_dis = cmdMessenger.readDoubleArg();
  double yaw_angle = cmdMessenger.readDoubleArg();

  Serial.println("Arduino: we got the move distance is");
  Serial.println(x_dis);
  Serial.println(y_dis);
  Serial.println(yaw_angle);
  Serial.println("Going to move");

  mecanumCar.x = x_dis;
  mecanumCar.y = y_dis;
  mecanumCar.z = yaw_angle;

  Mecanum_Decompose(&mecanumCar);
  double relativeDistance[4] = {mecanumCar.w1, mecanumCar.w2, mecanumCar.w3, mecanumCar.w4};

  steppersDecorate.moveRelativeDistancesWithPredefinedAccelOrSpeed (relativeDistance);

  int rangeStatusArr[3] = {0, 0, 0};
  // Send back the result of the addition
  cmdMessenger.sendCmdStart(kMovementResult);
  cmdMessenger.sendCmdArg(rangeStatusArr[0]);
  cmdMessenger.sendCmdArg(rangeStatusArr[1]);
  cmdMessenger.sendCmdArg(rangeStatusArr[2]);
  cmdMessenger.sendCmdEnd();

  Serial.print ( "After:" );
  Serial.println ( micros () );
  Serial.print ( "Spent:" );
  Serial.println ( micros () - time );
}

// ------------------ M A I N  ----------------------



// Setup function
void setup() 
{
  // Listen on serial connection for messages from the pc
  Serial.begin(115200); 

  // Then give them to MultiStepper to manage
  steppersDecorate.addStepper(stepper1Decorate);
  steppersDecorate.addStepper(stepper2Decorate);
  steppersDecorate.addStepper(stepper3Decorate);
  steppersDecorate.addStepper(stepper4Decorate);

  // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Send the status to the PC that says the Arduino has booted
  cmdMessenger.sendCmd(kAcknowledge,"Arduino has started!");

  // set pin for blink LED
  pinMode(kBlinkLed, OUTPUT);
}

// Returns if it has been more than interval (in ms) ago. Used for periodic actions
bool hasExpired(unsigned long &prevTime, unsigned long interval) {
  if (  millis() - prevTime > interval ) {
    prevTime = millis();
    return true;
  } else     
    return false;
}

// Loop function
void loop() 
{
  // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();

  // Toggle LED periodically. If the LED does not toggle every 2000 ms, 
  // this means that cmdMessenger are taking a longer time than this  
  if (hasExpired(previousToggleLed,2000)) // Toggle every 2 secs
  {
    toggleLed();  
  } 
}

// Toggle led state 
void toggleLed()
{  
  ledState = !ledState;
  digitalWrite(kBlinkLed, ledState?HIGH:LOW);
}  

