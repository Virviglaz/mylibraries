#include "pid_cpp.h"

float Pid::compute(float input)
{
    /* Compute all the working error variables */
    float error = target - input;

    ITerm += (ki * error);

    if (ITerm > max)
      ITerm = max;
    else if (ITerm < min)
      ITerm = min;

    float dInput = input - lastInput;

    /* Compute PID Output */
    float output = kp * error + ITerm - kd * dInput;

    if (output > max)
      output = max;
    else if (output < min)
      output = min;

    /* Remember some variables for next time */
    lastInput = input;

    return output;
}
