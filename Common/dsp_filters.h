#ifndef DSP_FILTERS
#define DSP_FILTERS

typedef struct
{
  float Result;
  float Value;
  float Previous;
  float K;
}KalmanStructTypeDef;

void Kalman (KalmanStructTypeDef * Kalman);
int moving_average (float * values, float * averages, int size, int periods);
#endif
