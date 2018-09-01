// IMPLEMENTATION MODULE math
#define M2_IMPORT_math

#include <stdlib.h>
#include <math.h>

#define math_floor(x) floor(x)
#define math_ceil(x) ceil(x)
#define math_abs(x) fabs(x)
#define math_sin(x) sin(x)
#define math_cos(x) cos(x)
#define math_tan(x) tan(x)
#define math_asin(x) asin(x)
#define math_acos(x) acos(x)
#define math_atan(x) atan(x)
#define math_atan2(y, x) atan2(y, x)
#define math_exp(x) exp(x)
#define math_log(x) log(x)
#define math_pow(base, power) pow(base, power)
#define math_sqrt(x) sqrt(x)
#define math_rand() rand()
#define math_RAND_MAX() RAND_MAX
#define math_srand(seed) srand(seed)
#define math_is_nan(x) isnan(x)
#define math_NAN() NAN
#define math_INF() HUGE_VAL


int math_precision(void)
{
	static int cached = 0;
	double x, y;
	int i;

	if( cached != 0 )
		return cached;

	i = 0;
	x = 0.5;
	do {
		y = 1.0+x;
		if( y - 1.0 == 0.0 )
			break;
		x = 0.5 * x;
		i++;
	} while( 1 );
	cached = i;
	return i;
}


int math_scale(void)
{
	static int cached = 0;
	double x;
	int i;

	x = 0.5;
	i = 0;
	do {
		x = 0.5 * x;
		if( x == 0.0 )
			break;
		i++;
	} while( 1 );
	cached = i - math_precision();
	return cached;
}


// END
