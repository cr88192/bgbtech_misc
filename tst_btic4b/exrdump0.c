#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef SBYTE_T
#define SBYTE_T
typedef signed char sbyte;
#endif

#ifndef PDLIB_INT_BITS_T
#define PDLIB_INT_BITS_T
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
#endif

#ifndef UINT_T
#define UINT_T
typedef unsigned int uint;
#endif

#if !defined(LITTLEENDIAN) && !defined(BIGENDIAN)
#if defined(X86) || defined(X86_64) || defined(ARM)
#define LITTLEENDIAN
#endif
#endif


#ifdef _MSC_VER
#define force_inline __forceinline
#define default_inline __inline
#endif

#ifdef __GNUC__
#define force_inline inline
#define default_inline inline
#endif

#ifndef force_inline
#define force_inline
#define default_inline
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef M_TAU
#define M_TAU 6.283185307179586476925286766559
#endif

int main(int argc, char *argv[])
{
	int ofst[1024];
	byte *buf;
	byte *cs, *cs1;
	char *an, *aty, *chn;
	FILE *ifd;
	int cmp, xs, ys;
	int isz, asz;
	int i, j, k, l;
	
	ifd=fopen(argv[1], "rb");
	if(!ifd)
		return(-1);
	
	fseek(ifd, 0, 2);
	isz=ftell(ifd);
	fseek(ifd, 0, 0);
	
	buf=malloc(isz);
	fread(buf, 1, isz, ifd);
	fclose(ifd);
	
	if(	(buf[0]!=0x76) || (buf[1]!=0x2F) ||
		(buf[2]!=0x31) || (buf[3]!=0x01))
	{
		printf("EXR Magic Fail\n");
	}
	cs=buf+4;
	i=*(u32 *)cs; cs+=4;
	printf("Flags=%08X\n", i);
	
	while(1)
	{
		if(!(*cs))
		{
			cs++;
			break;
		}
	
		an=(char *)cs;
		cs=cs+strlen(an)+1;
		aty=(char *)cs;
		cs=cs+strlen(aty)+1;
		asz=*(u32 *)cs;
		cs+=4;
		
		printf("%s %s %d: ", an, aty, asz);

		if(!strcmp(aty, "compression"))
		{
			cmp=cs[0];
			printf("%d", cs[0]);
		}

		if(!strcmp(aty, "lineOrder"))
		{
			printf("%d", cs[0]);
		}

		if(!strcmp(aty, "float"))
		{
			printf("%f", ((float *)cs)[0]);
		}
		
		if(!strcmp(aty, "box2i"))
		{
		
			if(!strcmp(an, "dataWindow"))
			{
				xs=(((int *)cs)[2]-((int *)cs)[0])+1;
				ys=(((int *)cs)[3]-((int *)cs)[1])+1;
			}
		
			printf("(%d, %d, %d, %d)",
				((int *)cs)[0], ((int *)cs)[1],
				((int *)cs)[2], ((int *)cs)[3]);
		}

		if(!strcmp(aty, "v2i"))
		{
			printf("(%d, %d)",
				((int *)cs)[0], ((int *)cs)[1]);
		}

		if(!strcmp(aty, "v2f"))
		{
			printf("(%f, %f)",
				((float *)cs)[0], ((float *)cs)[1]);
		}
		
		if(!strcmp(aty, "chlist"))
		{
			printf("\n");

			cs1=cs;
			while(*cs1)
			{
				chn=(char *)cs1;
				cs1=cs1+strlen(chn)+1;
				printf("\tN=%s PxT=%d Lin=%d Xss=%d Yss=%d\n", chn,
					*(int *)cs1, cs1[4],
					*(int *)(cs1+8), *(int *)(cs1+12));
				cs1+=16;
			}
		}
		
		printf("\n");

#if 0
		for(i=0; i<((asz+15)>>4); i++)
		{
			for(j=0; j<16; j++)
			{
				if(j && !(j&7))
					printf(" ");
				if(j && !(j&3))
					printf(" ");
				k=i*16+j;
				if(k>=asz)
				{
					printf("   ");
					continue;
				}
				printf("%02X ", cs[k]);
			}

			for(j=0; j<16; j++)
			{
				k=i*16+j;
				if((k>=asz) || (cs[k]<' ') || (cs[k]>'~'))
				{
					printf(".");
					continue;
				}
				printf("%c", cs[k]);
			}
			printf("\n");
		}
#endif

		cs+=asz;
	}
	
	printf("Cmp=%d Xs=%d Ys=%d\n", cmp, xs, ys);
	
	l=(ys+31)/32;
	for(i=0; i<l; i++)
	{
		ofst[i]=((s64 *)cs)[i];
//		ofst[i]=((int *)cs)[i];
	}

	for(i=0; i<l; i++)
		printf("%08X\n", ofst[i]);
}
