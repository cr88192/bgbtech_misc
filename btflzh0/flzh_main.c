#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "flzh_multi.c"


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
	buf=malloc(sz+64);
	fread(buf, 1, sz, fd);
	fclose(fd);
	
	*rsz=sz;
	return(buf);
}

int storefile(char *name, byte *ibuf, int isz)
{
	FILE *fd;

	fd=fopen(name, "wb");
	if(!fd)
	{
		printf("Fail Open Write %s\n", name);
		return(-1);
	}

	fwrite(ibuf, 1, isz, fd);
	fclose(fd);
	return(0);
}

u32 BtFLZH_Hash32(byte *buf, int sz)
{
	u32 *cs, *cse;
	u32 h, v, v0, v1, v2, v3;
	int n;
	
//	n=(sz+3)/4;
	n=sz/4;
	cs=(u32 *)buf;
	cse=cs+n;

	h=0;

	while((cs+4)<=cse)
	{
		v0=cs[0];	v1=cs[1];
		v2=cs[2];	v3=cs[3];
		cs+=4;
		h=h*65521+v0;
		h=h*65521+v1;
		h=h*65521+v2;
		h=h*65521+v3;
	}
	
	while(cs<cse)
	{
		v=*cs++;
		h=h*65521+v;
	}
	
	return(h);
}

int BtFLZH_CompareBuffer(byte *sbuf, byte *tbuf, int sz)
{
	u32 *cs, *cse;
	u32 *ct;
	int i, n;
	
	n=sz/4;
	
	cs=(u32 *)sbuf; cse=cs+n;
	ct=(u32 *)tbuf;
	
	while(cs<cse)
	{
		if(*cs!=*ct)break;
		cs++;
		ct++;
	}
	
	i=(int)(cs-((u32 *)sbuf));
	return(i*4);
}

void print_usage(char *name)
{
	fprintf(stderr, "usage: %s [args] infile outfile\n", name);
	fprintf(stderr, "\t-z   Encode File\n");
	fprintf(stderr, "\t-d   Decode File\n");
	fprintf(stderr, "\t-bt  Run Self Test Mode\n");
	fprintf(stderr, "\t-1   Compress Fast, Prioritize Ratio\n");
	fprintf(stderr, "\t-9   Compress Best, Prioritize Ratio\n");
	fprintf(stderr, "\t-1f  Compress Fast, Prioritize Decode Speed\n");
	fprintf(stderr, "\t-9f  Compress Best, Prioritize Decode Speed\n");
}

int main(int argc, char *argv[])
{
	BtFLZH_EncodeCtx *ectx;
	BtFLZH_DecodeCtx *dctx;
	byte *ibuf, *obuf, *tbuf;
	char *infn, *outfn;
	int isz, osz, tsz, tesz;
	u64 acsz;
	u32 h0, h1;
	int btt;
	int t0, t1, t2;
	int md, lvl;
	int i, j, k;
	
	infn=NULL;
	outfn=NULL;

	md=0; lvl=6; btt=0;

	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-z"))
				{ md=1; continue; }
			if(!strcmp(argv[i], "-d"))
				{ md=2; continue; }
			if(!strcmp(argv[i], "-bt"))
				{ md=3; btt=10; continue; }
			if(!strcmp(argv[i], "-bt60"))
				{ md=3; btt=60; continue; }
			if(!strcmp(argv[i], "-bt120"))
				{ md=3; btt=120; continue; }

			if(!strcmp(argv[i], "-1"))	{ lvl=16|1; continue; }
			if(!strcmp(argv[i], "-2"))	{ lvl=16|2; continue; }
			if(!strcmp(argv[i], "-3"))	{ lvl=16|3; continue; }
			if(!strcmp(argv[i], "-4"))	{ lvl=16|4; continue; }
			if(!strcmp(argv[i], "-5"))	{ lvl=16|5; continue; }
			if(!strcmp(argv[i], "-6"))	{ lvl=16|6; continue; }
			if(!strcmp(argv[i], "-7"))	{ lvl=16|7; continue; }
			if(!strcmp(argv[i], "-8"))	{ lvl=16|8; continue; }
			if(!strcmp(argv[i], "-9"))	{ lvl=16|9; continue; }

			if(!strcmp(argv[i], "-1f"))	{ lvl=1; continue; }
			if(!strcmp(argv[i], "-2f"))	{ lvl=2; continue; }
			if(!strcmp(argv[i], "-3f"))	{ lvl=3; continue; }
			if(!strcmp(argv[i], "-4f"))	{ lvl=4; continue; }
			if(!strcmp(argv[i], "-5f"))	{ lvl=5; continue; }
			if(!strcmp(argv[i], "-6f"))	{ lvl=6; continue; }
			if(!strcmp(argv[i], "-7f"))	{ lvl=7; continue; }
			if(!strcmp(argv[i], "-8f"))	{ lvl=8; continue; }
			if(!strcmp(argv[i], "-9f"))	{ lvl=9; continue; }

			if(!strcmp(argv[i], "-1ff"))	{ lvl=32|1; continue; }
			if(!strcmp(argv[i], "-2ff"))	{ lvl=32|2; continue; }
			if(!strcmp(argv[i], "-3ff"))	{ lvl=32|3; continue; }
			if(!strcmp(argv[i], "-4ff"))	{ lvl=32|4; continue; }
			if(!strcmp(argv[i], "-5ff"))	{ lvl=32|5; continue; }
			if(!strcmp(argv[i], "-6ff"))	{ lvl=32|6; continue; }
			if(!strcmp(argv[i], "-7ff"))	{ lvl=32|7; continue; }
			if(!strcmp(argv[i], "-8ff"))	{ lvl=32|8; continue; }
			if(!strcmp(argv[i], "-9ff"))	{ lvl=32|9; continue; }

			fprintf(stderr, "unexpected argument '%s'\n", argv[i]);
		}else
		{
			if(!infn)
				{ infn=argv[i]; continue; }
			if(!outfn)
				{ outfn=argv[i]; continue; }
			fprintf(stderr, "unexpected argument '%s'\n", argv[i]);
		}
	}
	
//	if(!md || !infn || (!outfn && (md!=3)))
	if(!infn || (!outfn && (md!=3)))
	{
		print_usage(argv[0]);
		return(0);
	}
	
	if((md==0) || (md==1))
	{
		ibuf=loadfile(infn, &isz);
		
		if(!ibuf)
		{
			fprintf(stderr, "Failed Load %s\n", infn);
			return(-1);
		}
		
		h0=BtFLZH_Hash32(ibuf, isz);
		printf("H0=%08X\n", h0);
		
		tsz=isz*2;
		tbuf=malloc(tsz);
		
		ectx=BtFLZH_AllocEncodeContext();
		tesz=BtFLZH_LzEncodeStream(ectx, ibuf, isz, tbuf+16, tsz, lvl);
		BtFLZH_DumpContextStats(ectx);
		BtFLZH_FreeEncodeContext(ectx);
		
		printf("%d -> %d %.2f%%\n", isz, tesz, (100.0*tesz)/isz);
		
		tbuf[0]='B';
		tbuf[1]='L';
		tbuf[2]='Z';
		tbuf[3]='F';
		tbuf[4]=11;		//Method
		tbuf[5]=0;		//Version
		tbuf[6]=((24-14)<<4) | (18-14);	//Window, Chunk Size
		tbuf[7]=0;
		BTFLZH_SETU32(tbuf+8, isz);
		BTFLZH_SETU32(tbuf+12, h0);
		
		j=0;
		for(i=0; i<16; i++)
			j=j*31+tbuf[i];
		tbuf[7]=j;
		
		storefile(outfn, tbuf, tesz+16);
		return(0);
	}

	if(md==2)
	{
		ibuf=loadfile(infn, &isz);
		
		if(!ibuf)
		{
			fprintf(stderr, "Failed Load %s\n", infn);
			return(-1);
		}
		
		if((ibuf[0]!='B') || (ibuf[1]!='L') ||
			(ibuf[2]!='Z') || (ibuf[3]!='F') ||
			(ibuf[4]!=11) ||
			((ibuf[5]&0xF0)!=0x00))
		{
			printf("Bad Magic Numbers\n");
			return(-1);
		}

		k=ibuf[7];
		ibuf[7]=0;

		j=0;
		for(i=0; i<16; i++)
			j=j*31+ibuf[i];
		j=j&255;

		if(j!=k)
		{
			printf("Header Hash Fail %d!=%d\n", j, k);
			return(-1);
		}

		tsz=BTFLZH_GETU32(ibuf+8);
		h0=BTFLZH_GETU32(ibuf+12);
		obuf=malloc(tsz*2);

		dctx=BtFLZH_AllocDecodeContext();
		osz=BtFLZH_DecodeStream(dctx, ibuf+16, isz-16, obuf, tsz);
		BtFLZH_FreeDecodeContext(dctx);

		if(osz!=tsz)
		{
			printf("Size Mismatch %d != %d\n", osz, tsz);
			return(-1);
		}

		h1=BtFLZH_Hash32(obuf, osz);

		if(h1!=h0)
		{
			printf("Hash Mismatch %08X != %08X\n", h1, h0);
			return(-1);
		}


		storefile(outfn, obuf, osz);
		return(0);
	}
	
	if(md==3)
	{
		ibuf=loadfile(infn, &isz);
		
		if(!ibuf)
		{
			fprintf(stderr, "Failed Load %s\n", infn);
			return(-1);
		}
		
		h0=BtFLZH_Hash32(ibuf, isz);
		printf("H0=%08X\n", h0);
		
		tsz=isz*2;
		tbuf=malloc(tsz);
		
		ectx=BtFLZH_AllocEncodeContext();
		tesz=BtFLZH_LzEncodeStream(ectx, ibuf, isz, tbuf, tsz, lvl);
		BtFLZH_DumpContextStats(ectx);
		BtFLZH_FreeEncodeContext(ectx);
		
		printf("%d -> %d %.2f%%\n", isz, tesz, (100.0*tesz)/isz);
		
		obuf=malloc(isz*2);
		
		t0=clock();
		
		dctx=BtFLZH_AllocDecodeContext();
		osz=BtFLZH_DecodeStream(dctx, tbuf, tesz, obuf, isz*2);
		BtFLZH_FreeDecodeContext(dctx);

		t1=clock();

		printf("%d -> %d %s\n", isz, osz, (isz==osz)?"OK":"BAD");

		if(isz==osz)
		{
			h1=BtFLZH_Hash32(obuf, osz);
			printf("H1=%08X %s\n", h1, (h0==h1)?"OK":"BAD");
			
			if(h1!=h0)
			{
				i=BtFLZH_CompareBuffer(ibuf, obuf, isz);
				printf("Match Fail At %08X / %08X\n", i, isz);
			}
			
			t2=t1-t0;
			printf("dec %dms %.2fMB/s\n", t2, (osz/1000000.0)/(t2/1000.0));
		}

#if 1
		t0=clock();
		t1=t0; t2=0;

		acsz=0;
		dctx=BtFLZH_AllocDecodeContext();
//		while(t2<60000)
//		while(t2<10000)
		while(t2<(btt*1000))
		{
//			dctx=BtFLZH_AllocDecodeContext();
			osz=BtFLZH_DecodeStream(dctx, tbuf, tesz, obuf, isz*2);
//			BtFLZH_FreeDecodeContext(dctx);
			acsz+=osz;
			t1=clock();
			t2=t1-t0;
			printf("dec %dms %.2fMB/s\r", t2, (acsz/1000000.0)/(t2/1000.0));
		}
		BtFLZH_FreeDecodeContext(dctx);
		printf("\n");
#endif
	}
	return(0);
}
