#ifndef _MYMATH_H_
#define _MYMATH_H_
float Linearize(float value, float x1, float y1, float x2, float y2);

static const struct
{
	float (* Lin) (float value, float x1, float y1, float x2, float y2);
}MyMath = { Linearize };
#endif
