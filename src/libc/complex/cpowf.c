#include "libc/libm.h"

float complex cpowf(float complex z, float complex c)
{
	return cexpf(c * clogf(z));
}
