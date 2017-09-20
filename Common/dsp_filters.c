#include "dsp_filters.h"

void Kalman (KalmanStructTypeDef * Kalman)
{
  Kalman->Result = Kalman->K * Kalman->Value;
  Kalman->Value = 1.0 - Kalman->K;
  Kalman->Previous *= Kalman->Value;
  Kalman->Result += Kalman->Previous;
  Kalman->Previous = Kalman->Result;
}

int moving_average (float * values, float * averages, int size, int periods)
{
	float sum = 0;
	int i;
	
	for (i = 0; i < size; i++)
		if (i < periods)
		{
			sum += values[i];
			averages[i] = (i == periods - 1) ? sum / (float)periods : 0;
		}
		else
		{
			sum = sum - values[i - periods] + values[i];
			averages[i] = sum / (float)periods;
		}
		return (size - periods + 1 > 0) ? size - periods + 1 : 0;
}
