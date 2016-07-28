//Copyright 2016 Edutec Citilab
//
//Heavily based un Steppers by Igor Campos
//
//Steppers is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//Steppers is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Steppers.  If not, see <http://www.gnu.org/licenses/>.

#ifndef STEPPERS_H
#define STEPPERS_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define CONTINUOUS 0
#define ROTATIONS 1
#define STEPS 2
#define STOP 0
#define CW 1
#define CCW 2
#define NC 0
#define NEMA 1
#define OTHER 2



class Steppers
{
  private:
    boolean done;
    byte pin1;
    byte pin2;
    byte pin3;
    byte pin4;
    byte noSteps;
    byte steps[8];
    byte nextStep;
    byte direction;
    byte mode;
    byte stepperType;
    unsigned int stepsToGo;
    unsigned int stepsGone;
    unsigned int rotationsToGo;
    unsigned int rotationsGone;
    float stepCorrection;
    float rpm;
    float spr;
    float time;
    unsigned long timer;
    void setTimer();
    void calcTime();
    void disable();
	void step();
  public:
    Steppers(byte pin1, byte pin2, byte pin3 = 0, byte pin4 = 0, byte steps[] = (byte[]){8, B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001}, float spr = 4075.7728395, float rpm = 12, byte direction = CW);
    void setPins(byte pin1, byte pin2, byte pin3, byte pin4);
    void setSPR(float SPR);
    void setRPM(float RPM);
    void setDirection(byte direction = CW);
    void rotate(unsigned int rotations = 0);
    void rotateDegrees(float degrees);
    void run();
	boolean isDone();
};

#endif
