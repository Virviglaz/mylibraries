

float Linearize(float value, float x1, float y1, float x2, float y2)
{
    float k = (y2 - y1) / (x2 - x1);
    float b = y1 - k * x1;
    
    return value * k + b;
}