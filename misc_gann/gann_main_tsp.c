/*
Mock-up scenario, Traveling Salesman Problem

It will generate 256 randomized cities and try to search for the shortest path based on mutating the permutation.

Note that this does not "solve" the problem as the path is not gueranteed to be optimal.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include "gann.h"
#include "gann_fpn.c"
#include "gann_maj.c"


static byte gann_nnbuf[1<<16];
static u16 gann_brseq;
static int gann_bxid;

int GANN_Test_CacheMember(GANN_Context *ctx, GANN_Member *mm)
{
	if(mm->brseq!=gann_brseq)
	{
//		memset(gann_nnbuf, 0, 1<<16);
		GANN_GetMemberAsBytes(mm, gann_nnbuf);
		gann_brseq=mm->brseq;
	}
	return(0);
}

int gann_tsp_points[256*2];
byte gann_tsp_pgc[256];

int gann_tsp_dist(int x0, int y0, int x1, int y1)
{
	int dx, dy, d;

	dx=x1-x0;
	dy=y1-y0;
	dx^=dx>>31;
	dy^=dy>>31;
	
	if(dx>=dx)
		d=dx+(dy>>1);
	else
		d=dy+(dx>>1);
	return(d);
}

int GANN_Test_TestTsp0(GANN_Context *ctx, GANN_Member *mm)
{
	byte map[256];
	byte *sttab;
	u64 bv, wv, wv0, wv1, mbcur;
	int len, lmask, ix, bx, bi, bd;
	int cx, cy, lcx, lcy, dx, dy;
	int probe, terr, trng;
	int i, j, k, l, p, ev, pen;
	
	GANN_GetMemberAsBytes(mm, gann_nnbuf);
//	qnb=(u64 *)nbuf;
	terr=0;

	gann_bxid=256/8;

	for(i=0; i<256; i++)
	{
		gann_tsp_pgc[i]=i;
		map[i]=0;
	}

	sttab=gann_nnbuf;

#if 0
	for(i=0; i<256; i++)
	{
		j=sttab[i];
		k=gann_tsp_pgc[i];
		gann_tsp_pgc[i]=gann_tsp_pgc[j];
		gann_tsp_pgc[j]=k;
	}
#endif

	ev=0;

	for(i=0; i<256; i++)
	{
		j=sttab[i];
		for(k=0; k<256; k++)
		{
			l=(k>>1)^(((s32)(k<<31))>>31);
			l=(j+l)&255;
			if(!map[l])
			{
				gann_tsp_pgc[i]=l;
				map[l]=1;
				break;
			}
			ev+=16;
		}
	}

	j=gann_tsp_pgc[0];
	lcx=gann_tsp_points[j*2+0];
	lcy=gann_tsp_points[j*2+1];
	for(i=0; i<256; i++)
	{
		j=gann_tsp_pgc[i];
		cx=gann_tsp_points[j*2+0];
		cy=gann_tsp_points[j*2+1];
		k=gann_tsp_dist(cx, cy, lcx, lcy);
		ev+=k;
		lcx=cx;
		lcy=cy;
	}

	return(ev);
}


byte *GANN_LoadFile(char *name, int *rsz)
{
	byte *buf;
	FILE *fd;
	int sz, i;
	
	fd=fopen(name, "rb");
	if(!fd)
	{
		return(NULL);
	}
	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);
	buf=malloc(sz+24);
	i=fread(buf, 1, sz, fd);
	fclose(fd);
	
	if(i!=sz)
	{
		if(i>0)
		{
			sz=i;
		}else
		{
			free(buf);
			return(NULL);
		}
	}

	memset(buf+sz, 0, 16);
	
	*rsz=sz;
	return(buf);
}

int GANN_StoreFile(char *name, void *buf, int sz)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)
		return(-1);
	
	fwrite(buf, 1, sz, fd);
	fclose(fd);
	return(0);
}

int gann_log2u(int val)
{
	int v, e;
	
	v=val; e=0;
	while(v>1)
		{ v=(v+1)>>1; e++; }
	return(e);
}

int gann_log2u_f8(int val)
{
	int v, e;
	
	if(val<8)
		return(val);
	
	v=val; e=1;
	while(v>=16)
		{ v=(v+1)>>1; e++; }
	v=(v&7)|(e<<3);
	return(v);
}

int gann_exp2u_f8(int val)
{
	int v, e;
	
	if(val<8)
		return(val);
	v=8|(val&7); e=(val>>3)&31;
	v=v<<(e-1);
	return(v);
}

int main(int argc, char *argv[])
{
	GANN_Context *ctx;
	GANN_Member *vm;
	u64 mbits[64];
	char tbuf[256];
	char tb1[256];
	byte *ibuf;
	int szme, is16b, ngen, szm1, bxid, szgen, szkeep;
	int i, j, k, l;

	GANN_InitMaj();
	GANN_InitFp8Tab();
	
	vm=malloc(sizeof(GANN_Member));
	memset(vm, 0, sizeof(GANN_Member));

	GANN_Test_TestTsp0(NULL, vm);
	bxid=gann_bxid;
	

	for(i=0; i<512; i++)
	{
		gann_tsp_points[i]=GANN_Rand16()&4095;
	}

	j=GANN_AddFp16J(0x3FAB, 0x4CDF);
	k=GANN_AddFp16I(0x3FAB, 0x4CDF);
	if(j!=k)
		{ __debugbreak(); }
	
	szme=256;
	while(szme<(bxid*8))
		szme=szme<<1;
	
	j=gann_log2u_f8(bxid*8);
	szme=gann_exp2u_f8(j);
	
	if(szme<(bxid*8))
		{ __debugbreak(); }
	
	is16b=GANN_MODE_MAJ7;
//	is16b=GANN_MODE_MAJ3;
//	is16b=GANN_MODE_HAMM;
//	szme=1<<14;
//	szme=1<<22;
//	szme=1<<19;
//	ngen=(1<<16);
	ngen=(1<<12);
//	ngen=(1<<10);

//	szgen=1<<8;		szkeep=1<<4;
	szgen=1<<10;	szkeep=1<<4;
//	szgen=1<<6;		szkeep=1<<3;

//	ctx=GANN_AllocContext(256, szme, is16b);
	ctx=GANN_AllocContext(szgen, szme, is16b);

	ctx->TestMember=GANN_Test_TestTsp0;

	printf("bxid=%d\n", bxid);

	sprintf(tb1, "gann_%u", bxid);
	mkdir(tb1, 0777);
	
	for(i=0; i<8; i++)
	{
		sprintf(tb1, "gann_%u/gann_%u_a%u.dat", bxid, bxid, i);
		ibuf=GANN_LoadFile(tb1, &k);
		if(ibuf)
		{
			sprintf(tb1, "gann_%u/gann_%u_b%u.dat", bxid, bxid, i);
			GANN_StoreFile(tb1, ibuf, szme);
			free(ibuf);
		}

		sprintf(tb1, "gann_%u/gann_%u_%u.dat", bxid, bxid, i);
		ibuf=GANN_LoadFile(tb1, &k);
		if(ibuf)
		{
			sprintf(tb1, "gann_%u/gann_%u_a%u.dat", bxid, bxid, i);
			GANN_StoreFile(tb1, ibuf, szme);

			GANN_SetMemberAsBytes(ctx->marr[i+ 0], ibuf);
			GANN_SetMemberAsBytes(ctx->marr[i+ 8], ibuf);
			if(szgen>=32)
			{
				GANN_SetMemberAsBytes(ctx->marr[i+16], ibuf);
				GANN_SetMemberAsBytes(ctx->marr[i+24], ibuf);
			}
			free(ibuf);
		}
	}

	for(i=0; i<ngen; i++)
	{
		if((i&7)==7)
		{
			tb1[0]=0;

			printf(
				"mstat: X:%02X S:%02X C:%02X ID:%02X"
				"  D:%02X I:%02X %02X %02X  %s\n",
				gann_log2u_f8(ctx->mstat_sk[0]),
				gann_log2u_f8(ctx->mstat_sk[1]),
				gann_log2u_f8(ctx->mstat_sk[2]),
				gann_log2u_f8(ctx->mstat_sk[3]),
				gann_log2u_f8(ctx->mstat_sk[4]),
				gann_log2u_f8(ctx->mstat_sk[5]),
				gann_log2u_f8(ctx->mstat_sk[6]),
				gann_log2u_f8(ctx->mstat_sk[7]),
				tb1);

			ctx->mstat_sk[0]=0;		ctx->mstat_sk[1]=0;
			ctx->mstat_sk[2]=0;		ctx->mstat_sk[3]=0;
			ctx->mstat_sk[4]=0;		ctx->mstat_sk[5]=0;
			ctx->mstat_sk[6]=0;		ctx->mstat_sk[7]=0;
		}
	
		printf("%d/%d a:%d b:%d c:%d d:%d mrm:%02X/%02X  %s  \n", i, ngen,
			ctx->earr[0], ctx->earr[1],
			ctx->earr[2], ctx->earr[3],
			ctx->marr[0]->mrmb,
			ctx->marr[0]->mrmc,
			tbuf);
		fflush(stdout);

		ctx->curgen=i;
		GANN_TestMembers(ctx);
		GANN_SortMembers(ctx);
//		GANN_BreedMembers(ctx, 16);
		GANN_BreedMembers(ctx, szkeep);

		tbuf[0]=0;

		if(i && !(i&255))
		{
			for(j=0; j<8; j++)
			{
				GANN_Test_CacheMember(ctx, ctx->marr[j]);
				sprintf(tb1, "gann_%u/gann_%u_%u.dat", bxid, bxid, j);
				GANN_StoreFile(tb1, gann_nnbuf, szme);
			}
		}
	}
	
	printf("\n");

	for(i=0; i<256; i++)
	{
		printf("city %d: %d %d\n", i,
			gann_tsp_points[i*2+0], gann_tsp_points[i*2+1] );
	}

	for(i=0; i<256; i++)
	{
		printf("%d ", gann_tsp_pgc[i]);
	}
	printf("\n");

	for(i=0; i<256; i++)
	{
		j=gann_tsp_pgc[i];
		printf("city %d: %d %d\n", j,
			gann_tsp_points[j*2+0], gann_tsp_points[j*2+1] );
	}


}
