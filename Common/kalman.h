#ifndef KALMAN_H
#define KALMAN_H

typedef struct
{
  float Result;                 //результирующее значение
  float Value;                  //исходное значение
  float Previous;               //результат вычисления в предыдущей интерации
  float K;                      //коэффициент стабилизации, по-умолчанию = 0.1
}KalmanFloatStructTypeDef;

typedef struct
{
  unsigned int Result;          //результирующее значение
  unsigned int Value;           //исходное значение
  unsigned int Previous;        //результат вычисления в предыдущей интерации
  float K;                      //коэффициент стабилизации, по-умолчанию = 0.1
}KalmanStructTypeDef;

void KalmanFloatCalc (KalmanFloatStructTypeDef * KalmanFloatStruct);
void KalmanCalc (KalmanStructTypeDef * KalmanFloatStruct);
#endif
