#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tkbihuf.c"
#include "tkdelzd.c"
#include "tkdelze.c"

byte *loadfile(char *path, int *rsz)
{
	byte *buf;
	FILE *fd;
	int sz;
	
	fd=fopen(path, "rb");
	if(!fd)
		return(NULL);

	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);
	buf=malloc(sz);
	fread(buf, 1, sz, fd);
	fclose(fd);
	
	*rsz=sz;
	return(buf);
}

int storefile(char *path, byte *buf, int sz)
{
	FILE *fd;
	
	fd=fopen(path, "wb");
	if(!fd)
		return(-1);

	fwrite(buf, 1, sz, fd);
	fclose(fd);
	return(0);
}

u32 hash_buffer(byte *buf, int sz)
{
	u32 *cs, *cse;
	u32 ha, hb, hc;
	
	ha=0; hb=0;
	cs=(u32 *)buf; cse=cs+(sz>>2);
	while(cs<cse)
	{
		hc=*cs++;
		ha+=hc;
		hb+=ha;
	}
	hc=ha^hb;
	return(hc);
}

int main(int argc, char *argv[])
{
	TkDeLz_EncState tectx;
	TkDeLz_DecState tdctx;
	int t0, t1, t2, t3, t0e;
	u64 accsz;
	byte *ibuf, *tbuf, *obuf;
	char *ifn, *ofn;
	int isz, tsz, osz, esz, lvl, mod;
	int i, j, k;
	
	ifn=NULL;
	ofn=NULL;
	lvl=5;
	mod=1;
	
	for(i=1; argv[i]; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-e"))mod=1;
			if(!strcmp(argv[i], "-d"))mod=2;
			if(!strcmp(argv[i], "-t"))mod=0;

			if(!strcmp(argv[i], "-1"))lvl=0x01;
			if(!strcmp(argv[i], "-2"))lvl=0x02;
			if(!strcmp(argv[i], "-3"))lvl=0x03;
			if(!strcmp(argv[i], "-4"))lvl=0x04;
			if(!strcmp(argv[i], "-5"))lvl=0x05;
			if(!strcmp(argv[i], "-6"))lvl=0x06;
			if(!strcmp(argv[i], "-7"))lvl=0x07;
			if(!strcmp(argv[i], "-8"))lvl=0x08;
			if(!strcmp(argv[i], "-9"))lvl=0x09;

			if(!strcmp(argv[i], "-1f"))lvl=0x11;
			if(!strcmp(argv[i], "-2f"))lvl=0x12;
			if(!strcmp(argv[i], "-3f"))lvl=0x13;
			if(!strcmp(argv[i], "-4f"))lvl=0x14;
			if(!strcmp(argv[i], "-5f"))lvl=0x15;
			if(!strcmp(argv[i], "-6f"))lvl=0x16;
			if(!strcmp(argv[i], "-7f"))lvl=0x17;
			if(!strcmp(argv[i], "-8f"))lvl=0x18;
			if(!strcmp(argv[i], "-9f"))lvl=0x19;

			if(!strcmp(argv[i], "-1ff"))lvl=0x31;
			if(!strcmp(argv[i], "-2ff"))lvl=0x32;
			if(!strcmp(argv[i], "-3ff"))lvl=0x33;
			if(!strcmp(argv[i], "-4ff"))lvl=0x34;
			if(!strcmp(argv[i], "-5ff"))lvl=0x35;
			if(!strcmp(argv[i], "-6ff"))lvl=0x36;
			if(!strcmp(argv[i], "-7ff"))lvl=0x37;
			if(!strcmp(argv[i], "-8ff"))lvl=0x38;
			if(!strcmp(argv[i], "-9ff"))lvl=0x39;
			continue;
		}
		
		if(!ifn)
			{ ifn=argv[i]; continue; }
		if(!ofn)
			{ ofn=argv[i]; continue; }
	}
	
	if(!ifn)
	{
		return(-1);
	}
	
	if(!ofn)
	{
		mod=0;
	}
	
	if((mod==0) || (mod==1))
	{
		ibuf=loadfile(ifn, &isz);
		
		tsz=isz*2;
		osz=isz*2;
		tbuf=malloc(tsz);
		obuf=malloc(osz);
		
		memset(tbuf, 0, tsz);

		tsz=TkDeLz_EncodeBuffer(&tectx, tbuf, ibuf, tsz, isz, lvl);
		
		if(ofn)
		{
			storefile(ofn, tbuf, tsz);
		}
		
		if(mod==1)
			return(0);

		printf("TkDeLz E %d->%d %.2f%%\n", isz, tsz, (100.0*tsz)/isz);

		printf("Enc Hash %X\n", hash_buffer(tbuf, tsz));
		
		printf("Avg RLen=%f\n", (1.0*tectx.tot_rlen)/tectx.cnt_rlen);
		printf("Avg MLen=%f\n", (1.0*tectx.tot_mlen)/tectx.cnt_mlen);

		esz=TkDeLz_EstimateSizeBuffer(&tdctx, tbuf, tsz);
		printf("Est Decode Size %d\n", esz);

		osz=TkDeLz_DecodeBuffer(&tdctx, obuf, tbuf, osz, tsz);
		printf("TkDeLz D %d->%d, %s\n", isz, osz, (isz==osz)?"Pass":"Fail");
		
		if(isz!=osz)
			return(0);
		
		if(memcmp(ibuf, obuf, isz))
		{
			printf("Mismatch\n");
			return(0);
		}
		
		t0=clock();
		t0e=t0+(10*CLOCKS_PER_SEC);
		t1=t0;
		accsz=0;
		
		while(t1<t0e)
		{
			osz=TkDeLz_DecodeBuffer(&tdctx, obuf, tbuf, osz, tsz);
			accsz+=osz;

			t1=clock();
		}
		
		printf("%f MB/sec\n",
			(accsz*(1.0/(1024*1024)))/((t1-t0)/(1.0*CLOCKS_PER_SEC)));
		
		return(0);
	}
	
	if(mod==2)
	{
		ibuf=loadfile(ifn, &isz);

		osz=TkDeLz_EstimateSizeBuffer(&tdctx, ibuf, isz);
		obuf=malloc(osz);
		
		osz=TkDeLz_DecodeBuffer(&tdctx, obuf, ibuf, osz, isz);
		
		if(ofn)
		{
			storefile(ofn, obuf, osz);
		}
	}
}
