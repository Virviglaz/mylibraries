/**
  ******************************************************************************
  * @file    pid_cpp.h
  * @author  Pavel Nadein
  * @version V1.0
  * @date    11-March-2019
  * @brief   This file contains all the functions/macros to use PID.
  ******************************************************************************
  */

#ifndef __PID_H__
#define __PID_H__

#include <stdint.h>
#include <stdbool.h>

class Pid
{
	public:
		/* Constructor */
	Pid(float _kp, float _ki, float _kd, float _min, float _max, float _target)
	{
		kp = _kp;
		ki = _ki;
		kd = _kd;
		min = _min;
		max = _max;
		target = _target;
	};

	Pid(float _min, float _max, float _target)
	{
		kp = 5;
		ki = 2;
		kd = 0.01;
		min = _min;
		max = _max;
		target = _target;
	};
	
	float compute(float input);
	
	private:
		float kp; // * (P)roportional Tuning Parameter
		float ki; // * (I)ntegral Tuning Parameter
		float kd; // * (D)erivative Tuning Parameter
		float min, max;
		float ITerm, lastInput;
		float target;
};

#endif // __PID_H__
