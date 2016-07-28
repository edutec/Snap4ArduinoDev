
//Copyright 2016 Edutec Citilab
//Heavily based un Steppers by Igor Campos
//
//This is free software: you can redistribute it and/or modify
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

#include "Steppers.h"

Steppers::Steppers(byte pin1, byte pin2, byte pin3, byte pin4, byte steps[], float spr, float rpm, byte direction)
{

  this->setRPM(rpm);
  this->setSPR(spr);
  this->noSteps = steps[0];
  for (int i = 0; i < this->noSteps; i++)
    this->steps[i] = steps[i+1];    
  this->setPins(pin1, pin2, pin3, pin4);
  this->setDirection(direction);
  this->nextStep = 0;
  this->stepCorrection = 0;
  this->done = true;
}


void Steppers::setPins(byte pin1, byte pin2, byte pin3, byte pin4) 
{
  this->pin1 = pin1;
  this->pin2 = pin2;
  this->pin3 = pin3;
  this->pin4 = pin4;
  
  if (this->pin1 == this->pin2) 
   this->stepperType = NC;
  else { 
    if (this->pin3 == this->pin4)
     this->stepperType = NEMA;
    else   
     this->stepperType = OTHER;        
  };

  pinMode(this->pin1, OUTPUT);
  pinMode(this->pin2, OUTPUT);
  if (this->stepperType != NEMA) {
    pinMode(this->pin3, OUTPUT);   
    pinMode(this->pin4, OUTPUT);
  };

  if (this->stepperType == NEMA) {
    this->setRPM(24);
    this->setSPR(200);
    this->noSteps = 2;
    this->steps[0] = 0;
    this->steps[1] = 1;
  }; 
}  

void Steppers::step()
{

 /*if (this->stepsGone % 100 == 0)
  {
  Serial.print("stepsToGo: ");
	Serial.print(this->stepsToGo);
    Serial.print(" stepsGone: ");
	Serial.print(this->stepsGone);
    Serial.print(" rotationsToGo: ");
	Serial.print(this->rotationsToGo);
    Serial.print(" rotationsGone: ");
	Serial.print(this->rotationsGone);
    Serial.print(" mode: ");
	Serial.print(this->mode);
    Serial.print(" direction: ");
	Serial.print(this->direction);
    Serial.print(" stepCorrection: ");
	Serial.print(this->stepCorrection);
  Serial.println();
  Serial.println(this->stepperType);
	Serial.println();
  } */

  if (this->stepsToGo - this->stepsGone > 0)
  {
    if (this->direction != STOP)
    {
      if (this->stepperType == NEMA) {   
        digitalWrite(this->pin1, true);
        delayMicroseconds(500); 
        digitalWrite(this->pin1, false);
        delayMicroseconds(500);
      };  
      if (this->stepperType == OTHER) {  
        digitalWrite(this->pin1, (this->steps[this->nextStep] & B1000));
        digitalWrite(this->pin2, (this->steps[this->nextStep] & B0100));
        digitalWrite(this->pin3, (this->steps[this->nextStep] & B0010));
        digitalWrite(this->pin4, (this->steps[this->nextStep] & B0001));
      };
	  this->setTimer();
      if (this->direction == CW)
        this->nextStep == this->noSteps-1 ? this->nextStep = 0 : this->nextStep++;
      else if (this->direction == CCW)
        this->nextStep == 0 ? this->nextStep = this->noSteps-1 : this->nextStep--;
	  this->stepsGone++;
    }
    else
      this->disable();
  }
  else if (this->mode == CONTINUOUS)
  { 
    this->stepsGone = 0;
    this->step();
  }
  else if (this->mode == ROTATIONS)
  {
    this->stepsGone = 0;
    this->stepsToGo = this->spr + this->stepCorrection + 0.5; //Steps per rotation + the step correction + 0.5 to force round
    this->stepCorrection = this->spr + this->stepCorrection - this->stepsToGo; //adjust the step correction for the next steps
    if (++this->rotationsGone >= this->rotationsToGo)
      this->disable();
    else
      this->step();
  }
  else if (this->mode == STEPS)
    this->disable();
}

void Steppers::disable()
{
  this->time = 0;
  if (this->stepperType == OTHER) {
    digitalWrite(this->pin1, LOW);
    digitalWrite(this->pin2, LOW);
    digitalWrite(this->pin3, LOW);
    digitalWrite(this->pin4, LOW);
  }  
  this->done = true;
}

void Steppers::setRPM(float rpm)
{
  if (rpm > 0)
    this->rpm = rpm;
  this->calcTime();
}

void Steppers::setSPR(float spr)
{
  if (spr > 0)
    this->spr = spr;
  this->calcTime();
}

void Steppers::calcTime()
{
  if (this->spr > 0 && this->rpm > 0) {
    this->time = 60000000.0/this->spr/this->rpm;
    if (this->stepperType == NEMA)
     this->time = (this->time/2.0) - 1000;  
  }
}

void Steppers::setDirection(byte direction)
{
  if (direction == CW || direction == CCW || direction == STOP)
    this->direction = direction;
  if (this->stepperType == NEMA)
    digitalWrite(this->pin2, (direction == CW));
}

void Steppers::rotate(unsigned int rotations)
{
  if (this->spr > 0 && this->rpm > 0)
  {
    if (rotations > 0)
      this->mode = ROTATIONS;
    else
      this->mode = CONTINUOUS;
    this->rotationsToGo = rotations;
    this->rotationsGone = 0;
    this->stepsToGo = this->spr + this->stepCorrection + 0.5; //Steps per rotation + the step correction + 0.5 to force round
    this->stepCorrection = this->spr + this->stepCorrection - this->stepsToGo; //adjust the step correction for the next steps
    this->stepsGone = 0;
	this->done = false;
	this->calcTime();
    this->step();
  }
}

void Steppers::rotateDegrees(float degrees)
{
  if (this->spr > 0 && this->rpm > 0 && degrees > 0)
  {
    this->mode = STEPS;
    this->stepsToGo = degrees/360.0*this->spr + this->stepCorrection + 0.5;
    this->stepCorrection = degrees/360.0*this->spr + this->stepCorrection - this->stepsToGo;
    this->stepsGone = 0;
	this->done = false;
	this->calcTime();
    this->step();
  }
}

void Steppers::setTimer()
{
  this->timer = micros() + this->time;
}

void Steppers::run()
{
  if (this->stepperType != NC) {
    if (this->timer <= micros() && micros() < this->timer + this->time)
      this->step(); 
  }
}

boolean Steppers::isDone()
{
  return this->done;
}