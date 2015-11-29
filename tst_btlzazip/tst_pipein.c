#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double curtime()
{
#ifdef _WIN32
	int t0;
	t0=clock();
	return(t0/((double)CLOCKS_PER_SEC));
#else
	static time_t base=0;
	struct timeval tv;
	time_t ds;
	
	gettimeofday(&tv, NULL);
	if(!base)
		base=tv.tv_sec;
	
	ds=tv.tv_sec-base;
	return((ds)+(tv.tv_usec*0.000001));
//	return((double)ds);
#endif
}

int main()
{
	long long aisz, aosz;
	double dt;
	double t0, t1, t2;
	char *buf;
	int i;
	
//	t0=clock();
	t0=curtime();
	aisz=0;
	buf=malloc(1<<20);
	while(!feof(stdin))
//	while(1)
	{
		i=fread(buf, 1, 1<<20, stdin);
		aisz+=1<<20;
		t1=curtime();
		t2=t1-t0;
		dt=t2;
		
		fprintf(stderr, "%.3fMB/s %.3f   \r",
			(aisz/(1024.0*1024.0))/dt, dt);
//		fprintf(stderr, "%.3fMB/s %.3f(%d)   \r",
//			(aisz/(1024.0*1024.0))/dt, dt, t2);
	}
	printf("\n");
	printf("dt=%.3fs\n", dt);
}