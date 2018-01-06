#ifndef MATH_SH
#define MATH_SH

#define C_PI 3.14159265359
#define C_H_PI (0.5*C_PI)
#define C_2_PI (2.0*C_PI)

float fast_acos(float x)
{
	return (-0.69813170079773212 * x * x - 0.87266462599716477) * x + 1.5707963267948966;
}


#endif