/**
  PUT all pins specification in one file to make things easier
*/
#ifndef PinsAndConfigurationCar_h
#define PinsAndConfigurationCar_h


// For RAMPS 1.4
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

#define Q_STEP_PIN         36
#define Q_DIR_PIN          34
#define Q_ENABLE_PIN       30

#define X_disPerRound			47.1 //3.14*15
//#define Z_disPerRound			47.1 //3.14*15

// MultipleSteppersDecorate.ino
// -*- mode: C++ -*-
// Use MultiStepperDecorate class to manage multiple steppers and test their homing function.

// Since we want to test homing function, we make the steppers' limit stop pin enable.

#include <AccelStepper.h>
#include <AccelStepperDecorateNoLimit.h>
#include <MultiStepperDecorateNoLimit.h>


/*
    AccelStepperDecorateNoLimit(char motorName,
                         int dirPin,
                         int stepPin,
                         int enablePin,

                         int subdivision,
                         int reductionRatio,
                         double stepAngle,
                         double maxSpeed,
                         double acceleration,

                         double disPerRound);
*/


AccelStepperDecorateNoLimit stepper1Decorate('X',
    X_DIR_PIN,
    X_STEP_PIN,
    X_ENABLE_PIN,

    1,
    30,
    1.8,
    600,
    100,

    X_disPerRound
                                            );


AccelStepperDecorateNoLimit stepper2Decorate ('Z',
    Z_DIR_PIN,
    Z_STEP_PIN,
    Z_ENABLE_PIN,

    1,
    30,
    1.8,
    600,
    100,

    X_disPerRound
                                             );




AccelStepperDecorateNoLimit stepper3Decorate ('E',
    E_DIR_PIN,
    E_STEP_PIN,
    E_ENABLE_PIN,

    1,
    30,
    1.8,
    600,
    100,

    X_disPerRound
                                             );


AccelStepperDecorateNoLimit stepper4Decorate('Q',
    Q_DIR_PIN,
    Q_STEP_PIN,
    Q_ENABLE_PIN,

    1,
    30,
    1.8,
    600,
    100,

    X_disPerRound
                                            );

#endif
