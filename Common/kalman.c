#include "kalman.h"

void KalmanFloatCalc (KalmanFloatStructTypeDef * KalmanFloatStruct)
{
  KalmanFloatStruct->Result = KalmanFloatStruct->K * KalmanFloatStruct->Value;
  KalmanFloatStruct->Value = 1.0 - KalmanFloatStruct->K;
  KalmanFloatStruct->Previous *= KalmanFloatStruct->Value;
  KalmanFloatStruct->Result += KalmanFloatStruct->Previous;
  KalmanFloatStruct->Previous = KalmanFloatStruct->Result;
}

void KalmanCalc (KalmanStructTypeDef * KalmanFloatStruct)
{
  KalmanFloatStruct->Result = (unsigned int)(KalmanFloatStruct->K * 
                                             (float)KalmanFloatStruct->Value);
  KalmanFloatStruct->Value = (unsigned int)(1.0 - KalmanFloatStruct->K);
  KalmanFloatStruct->Previous *= KalmanFloatStruct->Value;
  KalmanFloatStruct->Result += KalmanFloatStruct->Previous;
  KalmanFloatStruct->Previous = KalmanFloatStruct->Result;
}
