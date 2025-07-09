/*
Mockup of a branch predictor state machine.

Idea is that one has a state machine with a 5-bit state, and as input is given the last bit (branch direction, as the LSB of the input) with the goal of predicting the next branch direction (LSB of output). The other bits of the output will represent the next state for the state machine.

It will start in a random state, and should ideally find the target state based on the input bit pattern.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include "gann.h"
#include "gann_fpn.c"
#include "gann_maj.c"


#define GANN_TEST_STATETABLE	//mock up, branch-predictor FSM


static byte gann_nnbuf[1<<24];
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

int GANN_Test_TestStateTable0(GANN_Context *ctx, GANN_Member *mm)
{
	static u64 pattab[16]={
		0x0000000000000000ULL,		//0, 0000
		0xFFFFFFFFFFFFFFFFULL,		//1, 1111
		0x5555555555555555ULL,		//2, 0101
		0xAAAAAAAAAAAAAAAAULL,		//3, 1010
		0x6DB6DB6DB6DB6DB6ULL,		//4, 110110
		0x9249249249249249ULL,		//5, 001001
		0x1111111111111111ULL,		//6, 00010001
		0xEEEEEEEEEEEEEEEEULL,		//7, 11101110
		0xCCCCCCCCCCCCCCCCULL,		//8, 11001100
		0x0820820820820820ULL,		//9, 100000100000
		0x0842108421084210ULL,		//A, 1000010000
		0xF7BDEF7BDEF7BDEFULL,		//B, 0111101111
		0x5555555555555555ULL,		//C
		0xAAAAAAAAAAAAAAAAULL,		//D
		0x6DB6DB6DB6DB6DB6ULL,		//E
		0x9249249249249249ULL,		//F
	};
	static u64 modtab[16]={
		64,		64,		64,		64,
		63,		63,		64,		64,
		64,		60,		60,		60,
		64,		64,		64,		64
	};

//	u64 mbits[64];
	byte pgc[64];
	byte *sttab;
	u64 bv, wv, wv0, wv1, mbcur;
	int len, lmask, ix, bx, bi, bd;
	int probe, terr, trng;
	int i, j, k, l, p, ev, pen;
	
	GANN_GetMemberAsBytes(mm, gann_nnbuf);
//	qnb=(u64 *)nbuf;
	terr=0;

	trng=0x5555;
	if(ctx)
	{
		trng=ctx->curgen;
		trng=(trng<<1)^(trng>>17);
		trng=(trng<<1)^(trng>>17);
		trng=(trng<<1)^(trng>>17);
	}

	gann_bxid=8;
	
	sttab=gann_nnbuf;
	ix=trng&31;
	l=0;
	
	ev=0;
	for(i=0; i<64; i++)
	{
		ix=trng&31;
		trng=(trng<<1)^(trng>>17);

		for(j=24+((trng>>8)&31); j<modtab[i&15]; j++)
		{
			k=sttab[(ix<<1)|l];
			p=k&1;
			ix=(k>>1)&31;
			l=(pattab[i&15]>>j)&1;
		}

		for(j=0; j<modtab[i&15]; j++)
		{
			k=sttab[(ix<<1)|l];
			p=k&1;
			ix=(k>>1)&31;
			l=(pattab[i&15]>>j)&1;
			
			if(p!=l)
				ev++;
		}
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

#ifdef GANN_TEST_STATETABLE
	GANN_Test_TestStateTable0(NULL, vm);
#else
	k=GANN_Test_RunMember0(NULL, vm, mbits);
#endif
	bxid=gann_bxid;
	

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
	ngen=(1<<16);
//	ngen=(1<<12);
//	ngen=(1<<10);
//	szgen=1<<8;		szkeep=1<<4;
//	szgen=1<<7;		szkeep=1<<3;
//	szgen=1<<6;		szkeep=1<<3;
//	szgen=1<<5;		szkeep=1<<3;
	szgen=1<<4;		szkeep=1<<2;

#ifdef GANN_TEST_STATETABLE
//	szgen=1<<8;		szkeep=1<<4;
	szgen=1<<6;		szkeep=1<<3;
#endif

//	ctx=GANN_AllocContext(256, szme, is16b);
	ctx=GANN_AllocContext(szgen, szme, is16b);

#ifdef GANN_TEST_STATETABLE
	ctx->TestMember=GANN_Test_TestStateTable0;
#else
	ctx->TestMember=GANN_Test_TestMember0;
#endif

//	k=GANN_Test_RunMember0(ctx, ctx->marr[0], mbits);
//	bxid=gann_bxid;

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

#ifdef GANN_TEST_STATETABLE
	for(i=0; i<64; i++)
	{
		printf("  6'h%02X: st1=6'h%02X;\n", i, gann_nnbuf[i]&63);
	}
#endif
}
