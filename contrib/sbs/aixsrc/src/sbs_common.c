#include <stdlib.h>

#include <sys/time.h>



double gettime()

{

	static int f;

	static double start;

	double result;

	struct timeval t;

	gettimeofday(&t,NULL);

	result = t.tv_sec+(t.tv_usec/1000000.0);

	if(f == 0) {

		start = result;	

	}

	f = 1;

	return result-start;

}

