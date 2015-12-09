/*
Copyright (C) 2015 by Brendan G Bohannon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

double btic1g_ssqrt(double f)
{
	return((f>=0)?sqrt(f):(-sqrt(-f)));
}

double btic1g_ssqr(double f)
{
	return((f>=0)?(f*f):(-f*f));
}

double btic1g_slog(double f)
{
	return((f>=0)?log(f+1):(-log(-f+1)));
}

double btic1g_sexp(double f)
{
	return((f>=0)?exp(f)-1:(-exp(-f)+1));
}

void BTIC1H_InitQuant()
{
	static int init=0;
	int i, j, k;

	if(init)
		return;
	init=1;
}

int BTIC1H_EmitCommandCode(BTIC1H_Context *ctx, int cmd)
{
	int bit0;
	int i, j, k;
	
	ctx->cnt_cmds++;

	if((cmd<0) || (cmd>=64))
		{ *(int *)-1=-1; }
                          
	j=ctx->cmdidx[cmd];
	i=(byte)(j-ctx->cmdwpos);
	if(j>0)
	{
		k=ctx->cmdwin[j&15];
		if(k!=cmd)
			j=-1;
	}
	
	if((j>=0) && (i>=0) && (i<16))
	{
		if(i<0)
			return(-1);
	
		bit0=ctx->bs_bits;
		BTIC1H_Rice_WriteAdRice(ctx, i+1, &(ctx->rk_cmdidx));
		ctx->bits_cmdidx+=ctx->bs_bits-bit0;

		if(i>0)
		{
			j=(byte)(ctx->cmdwpos);
			k=ctx->cmdwin[(j+i-1)&15];
			ctx->cmdwin[(j+i-1)&15]=cmd;
			ctx->cmdwin[(j+i+0)&15]=k;
			ctx->cmdidx[cmd&255]=(byte)(j+i-1);
			ctx->cmdidx[k&255]=(byte)(j+i);
		}

		return(0);
	}
	
	bit0=ctx->bs_bits;

	BTIC1H_Rice_WriteAdRice(ctx, 0, &(ctx->rk_cmdidx));
	BTIC1H_Rice_WriteAdRice(ctx, cmd, &(ctx->rk_cmdabs));
	
	ctx->bits_cmdabs+=ctx->bs_bits-bit0;

	j=(byte)(--ctx->cmdwpos);
	k=ctx->cmdwin[j&15];

	if((ctx->cmdidx[k]&15)==(j&15))
		ctx->cmdidx[k]=-1;

	ctx->cmdwin[j&15]=cmd;
	ctx->cmdidx[cmd&255]=j;

	return(0);
}

int BTIC1H_EmitDeltaYUV(BTIC1H_Context *ctx,
	int cy, int cu, int cv)
{
	int bit0;
	int dy, du, dv, dd;
	int py, pu, pv;
	int qdy, qdu, qdv, qdd;
	
	dy=cy-ctx->cy;
	du=cu-ctx->cu;
	dv=cv-ctx->cv;

	qdy=(dy*ctx->fx_qfy +2048)>>12;
	qdu=(du*ctx->fx_qfuv+2048)>>12;
	qdv=(dv*ctx->fx_qfuv+2048)>>12;

	py=ctx->cy+(qdy*ctx->qfy);
	pu=ctx->cu+(qdu*ctx->qfuv);
	pv=ctx->cv+(qdv*ctx->qfuv);

	if((py|pu|pv)>>8)
	{
		qdy=qdy-(py>>8);
		qdu=qdu-(pu>>8);
		qdv=qdv-(pv>>8);

		py=ctx->cy+(qdy*ctx->qfy);
		pu=ctx->cu+(qdu*ctx->qfuv);
		pv=ctx->cv+(qdv*ctx->qfuv);
	}

	ctx->cy=py;
	ctx->cu=pu;
	ctx->cv=pv;

//	ctx->cy+=qdy*ctx->qfy;
//	ctx->cu+=qdu*ctx->qfuv;
//	ctx->cv+=qdv*ctx->qfuv;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdy, &(ctx->rk_dy));
	ctx->bits_dy+=ctx->bs_bits-bit0;
	ctx->bits_dyuv+=ctx->bs_bits-bit0;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdu, &(ctx->rk_duv));
	BTIC1H_Rice_WriteAdSRice(ctx, qdv, &(ctx->rk_duv));
	ctx->bits_duv+=ctx->bs_bits-bit0;
	ctx->bits_dyuv+=ctx->bs_bits-bit0;

#ifdef BT1H_DEBUG_TRAPRANGE
	if((ctx->cy<0) || (ctx->cy>255) ||
	   (ctx->cu<0) || (ctx->cu>255) ||
	   (ctx->cv<0) || (ctx->cv>255) ||
	   (ctx->cd<0) || (ctx->cd>255))
	{
		*(int *)-1=-1;
	}
#endif

	return(0);
}

#if 0
int BTIC1H_EmitDeltaYUVD(BTIC1H_Context *ctx,
	int cy, int cu, int cv, int cd)
{
	int bit0;
	int dd, qdd, pd;

	dd=cd-ctx->cd;
//	qdd=dd/ctx->qfd;
	qdd=(dd*ctx->fx_qfd+2048)>>12;
//	ctx->cd+=qdd*ctx->qfd;
	pd=ctx->cd+(qdd*ctx->qfd);
	if(pd>>8)
	{
		qdd=qdd-(pd>>8);
		pd=ctx->cd+(qdd*ctx->qfd);
	}
	ctx->cd=pd;
	
	BTIC1H_EmitDeltaYUV(ctx, cy, cu, cv);	
	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdd, &(ctx->rk_ddy));
	ctx->bits_ddy+=ctx->bs_bits-bit0;

	return(0);
}
#endif

#if 1
int BTIC1H_EmitDeltaYUVD(BTIC1H_Context *ctx,
	int cy, int cu, int cv, int cd)
{
	int bit0;
	int by, bu, bv, bd;
	int dy, du, dv, dd;
	int py, pu, pv, pd;
	int qdy, qdu, qdv, qdd;

	by=ctx->cy;		bu=ctx->cu;
	bv=ctx->cv;		bd=ctx->cd;
	dy=cy-by;		du=cu-bu;
	dv=cv-bv;		dd=cd-bd;
	qdy=(dy*ctx->fx_qfy +2048)>>12;
	qdu=(du*ctx->fx_qfuv+2048)>>12;
	qdv=(dv*ctx->fx_qfuv+2048)>>12;
	qdd=(dd*ctx->fx_qfd +2048)>>12;
	py=by+(qdy*ctx->qfy);	pu=bu+(qdu*ctx->qfuv);
	pv=bv+(qdv*ctx->qfuv);	pd=bd+(qdd*ctx->qfd);

	if((py|pu|pv|pd)>>8)
	{
		qdy=qdy-(py>>8);		qdu=qdu-(pu>>8);
		qdv=qdv-(pv>>8);		qdd=qdd-(pd>>8);
		py=by+(qdy*ctx->qfy);	pu=bu+(qdu*ctx->qfuv);
		pv=bv+(qdv*ctx->qfuv);	pd=bd+(qdd*ctx->qfd);
	}

	ctx->cy=py;		ctx->cu=pu;
	ctx->cv=pv;		ctx->cd=pd;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdy, &(ctx->rk_dy));
	ctx->bits_dy+=ctx->bs_bits-bit0;
	ctx->bits_dyuv+=ctx->bs_bits-bit0;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdu, &(ctx->rk_duv));
	BTIC1H_Rice_WriteAdSRice(ctx, qdv, &(ctx->rk_duv));
	ctx->bits_duv+=ctx->bs_bits-bit0;
	ctx->bits_dyuv+=ctx->bs_bits-bit0;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdd, &(ctx->rk_ddy));
	ctx->bits_ddy+=ctx->bs_bits-bit0;

#ifdef BT1H_DEBUG_TRAPRANGE
	if(	((ctx->cy<0) || (ctx->cy>255)) ||
		((ctx->cu<0) || (ctx->cu>255)) ||
		((ctx->cv<0) || (ctx->cv>255)) ||
		((ctx->cd<0) || (ctx->cd>255)))
	{
		*(int *)-1=-1;
	}
#endif

	return(0);
}
#endif

#if 1
int BTIC1H_EmitDeltaYUVDyuv(BTIC1H_Context *ctx,
	int cy, int cu, int cv,
	int cdy, int cdu, int cdv)
{
	int bit0;
	int by, bu, bv, bdy, bdu, bdv;
	int dy, du, dv, ddy, ddu, ddv;
	int py, pu, pv, pdy, pdu, pdv;
	int qdy, qdu, qdv, qddy, qddu, qddv;

	by=ctx->cy;		bu=ctx->cu;
	bv=ctx->cv;		bdy=ctx->cdy;
	bdu=ctx->cdu;	bdv=ctx->cdv;
	dy=cy-by;		du=cu-bu;
	dv=cv-bv;		ddy=cdy-bdy;
	ddu=cdu-bdu;	ddv=cdv-bdv;
	qdy=(dy*ctx->fx_qfy +2048)>>12;
	qdu=(du*ctx->fx_qfuv+2048)>>12;
	qdv=(dv*ctx->fx_qfuv+2048)>>12;
	qddy=(ddy*ctx->fx_qfdy +2048)>>12;
	qddu=(ddu*ctx->fx_qfduv+2048)>>12;
	qddv=(ddv*ctx->fx_qfduv+2048)>>12;
	py=by+(qdy*ctx->qfy);	pu=bu+(qdu*ctx->qfuv);
	pv=bv+(qdv*ctx->qfuv);
	pdy=bdy+(qddy*ctx->qfdy);
	pdu=bdu+(qddu*ctx->qfduv);
	pdv=bdv+(qddv*ctx->qfduv);

//	if((py|pu|pv|pdy|pdu|pdv)>>8)
	if((py|pu|pv)>>8)
	{
		qdy=qdy-(py>>8);			qdu=qdu-(pu>>8);
		qdv=qdv-(pv>>8);
		py=by+(qdy*ctx->qfy);		pu=bu+(qdu*ctx->qfuv);
		pv=bv+(qdv*ctx->qfuv);
	}

//	if(((pdy<-256)||(pdy>256))||
//		((pdu<-256)||(pdu>256))||
//		((pdv<-256)||(pdv>256)))
	if(((pdy^(pdy>>8))|(pdu^(pdu>>8))|(pdv^(pdv>>8)))>>8)
	{
		qddy=qddy-(pdy>>8);
		qddu=qddu-(pdu>>8);			qddv=qddv-(pdv>>8);
		pdy=bdy+(qddy*ctx->qfdy);
		pdu=bdu+(qddu*ctx->qfduv);	pdv=bdv+(qddv*ctx->qfduv);
	}

	ctx->cy=py;		ctx->cu=pu;
	ctx->cv=pv;		ctx->cdy=pdy;
	ctx->cdu=pdu;	ctx->cdv=pdv;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdy, &(ctx->rk_dy));
	ctx->bits_dy+=ctx->bs_bits-bit0;
	ctx->bits_dyuv+=ctx->bs_bits-bit0;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdu, &(ctx->rk_duv));
	BTIC1H_Rice_WriteAdSRice(ctx, qdv, &(ctx->rk_duv));
	ctx->bits_duv+=ctx->bs_bits-bit0;
	ctx->bits_dyuv+=ctx->bs_bits-bit0;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qddy, &(ctx->rk_ddy));
	ctx->bits_ddy+=ctx->bs_bits-bit0;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qddu, &(ctx->rk_dduv));
	BTIC1H_Rice_WriteAdSRice(ctx, qddv, &(ctx->rk_dduv));
	ctx->bits_dduv+=ctx->bs_bits-bit0;
	ctx->bits_dyuv+=ctx->bs_bits-bit0;

#ifdef BT1H_DEBUG_TRAPRANGE
	if(	((ctx->cy<0) || (ctx->cy>255)) ||
		((ctx->cu<0) || (ctx->cu>255)) ||
		((ctx->cv<0) || (ctx->cv>255)))
	{
		*(int *)-1=-1;
	}

	if(	((ctx->cdy<-256)||(ctx->cdy>255))||
		((ctx->cdu<-256)||(ctx->cdu>255))||
		((ctx->cdv<-256)||(ctx->cdv>255)))
	{
		*(int *)-1=-1;
	}
#endif

	return(0);
}
#endif

int BTIC1H_EmitDeltaY(BTIC1H_Context *ctx, int cy)
{
	int bit0;
	int dy, du, dv, dd;
	int qdy, qdu, qdv, qdd;
	
	dy=cy-ctx->cy;
	qdy=(dy*ctx->fx_qfy +2048)>>12;
	ctx->cy+=qdy*ctx->qfy;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdy, &(ctx->rk_dy));
	ctx->bits_dy+=ctx->bs_bits-bit0;
	ctx->bits_dyuv+=ctx->bs_bits-bit0;
	return(0);
}

int BTIC1H_EmitDeltaUV(BTIC1H_Context *ctx, int cu, int cv)
{
	int bit0;
	int dy, du, dv, dd;
	int qdy, qdu, qdv, qdd;
	
	du=cu-ctx->cu;
	dv=cv-ctx->cv;

	qdu=(du*ctx->fx_qfuv+2048)>>12;
	qdv=(dv*ctx->fx_qfuv+2048)>>12;

	ctx->cu+=qdu*ctx->qfuv;
	ctx->cv+=qdv*ctx->qfuv;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdu, &(ctx->rk_duv));
	BTIC1H_Rice_WriteAdSRice(ctx, qdv, &(ctx->rk_duv));
	ctx->bits_duv+=ctx->bs_bits-bit0;
	ctx->bits_dyuv+=ctx->bs_bits-bit0;
	
	return(0);
}

int BTIC1H_EmitDeltaYD(BTIC1H_Context *ctx, int cy, int cd)
{
	int bit0;
	int dy, du, dv, dd;
	int qdy, qdu, qdv, qdd;
	
	dy=cy-ctx->cy;
	dd=cd-ctx->cd;
	qdy=(dy*ctx->fx_qfy+2048)>>12;
	qdd=(dd*ctx->fx_qfd+2048)>>12;
	ctx->cy+=qdy*ctx->qfy;
	ctx->cd+=qdd*ctx->qfd;

	bit0=ctx->bs_bits;
	BTIC1H_Rice_WriteAdSRice(ctx, qdy, &(ctx->rk_dy));
	BTIC1H_Rice_WriteAdSRice(ctx, qdd, &(ctx->rk_ddy));
	ctx->bits_dy+=ctx->bs_bits-bit0;
	return(0);
}

int BTIC1H_EmitDeltaA(BTIC1H_Context *ctx, int cy)
	{ return(BTIC1H_EmitDeltaY(ctx, cy)); }

int BTIC1H_EmitDeltaAD(BTIC1H_Context *ctx, int cy, int cd)
	{ return(BTIC1H_EmitDeltaYD(ctx, cy, cd)); }


int BTIC1H_EmitDeltaQfYUVD(BTIC1H_Context *ctx,
	int cy, int cuv, int cd)
{
	int dy, duv, dd;
	int qdy, qdu, qdv, qdd;
	
	dy=cy-ctx->qfy;
	duv=cuv-ctx->qfuv;
	dd=cd-ctx->qfd;

	ctx->qfy=cy;
	ctx->qfuv=cuv;
	ctx->qfd=cd;

	ctx->fx_qfy =4096/cy;
	ctx->fx_qfuv=4096/cuv;
	ctx->fx_qfd =4096/cd;

	BTIC1H_Rice_WriteAdSRice(ctx, dy, &(ctx->rk_qfy));
	BTIC1H_Rice_WriteAdSRice(ctx, duv, &(ctx->rk_qfuv));
	BTIC1H_Rice_WriteAdSRice(ctx, dd, &(ctx->rk_qfdy));
	return(0);
}

int BTIC1H_EmitDeltaQfYUVDyuv(BTIC1H_Context *ctx,
	int cy, int cuv, int cdy, int cduv)
{
	int dy, duv, ddy, dduv;
	int qdy, qdu, qdv, qdd;
	
	dy=cy-ctx->qfy;
	duv=cuv-ctx->qfuv;
	ddy=cdy-ctx->qfdy;
	dduv=cduv-ctx->qfduv;

	ctx->qfy=cy;
	ctx->qfuv=cuv;
	ctx->qfdy=cdy;
	ctx->qfduv=cduv;

	ctx->fx_qfy  =4096/cy;
	ctx->fx_qfuv =4096/cuv;
	ctx->fx_qfdy =4096/cdy;
	ctx->fx_qfduv=4096/cduv;

	BTIC1H_Rice_WriteAdSRice(ctx, dy, &(ctx->rk_qfy));
	BTIC1H_Rice_WriteAdSRice(ctx, duv, &(ctx->rk_qfuv));
	BTIC1H_Rice_WriteAdSRice(ctx, ddy, &(ctx->rk_qfdy));
	BTIC1H_Rice_WriteAdSRice(ctx, dduv, &(ctx->rk_qfduv));
	return(0);
}

int BTIC1H_EmitDeltaQfAD(BTIC1H_Context *ctx,
	int cy, int cd)
{
	int dy, duv, dd;
	int qdy, qdu, qdv, qdd;
	
	dy=cy-ctx->qfy;
	dd=cd-ctx->qfd;

	ctx->qfy=cy;
	ctx->qfd=cd;

	ctx->fx_qfy=4096/cy;
	ctx->fx_qfd=4096/cd;

	BTIC1H_Rice_WriteAdSRice(ctx, dy, &(ctx->rk_qfy));
	BTIC1H_Rice_WriteAdSRice(ctx, dd, &(ctx->rk_qfdy));
	return(0);
}


int BTIC1H_CheckZeroDeltaYUV(BTIC1H_Context *ctx,
	int cy, int cu, int cv)
{
	int bit0;
	int dy, du, dv, dd;
	int qdy, qdu, qdv, qdd;
	
	dy=cy-ctx->cy;
	du=cu-ctx->cu;
	dv=cv-ctx->cv;

	qdy=(dy*ctx->fx_qfy +2048)>>12;
	qdu=(du*ctx->fx_qfuv+2048)>>12;
	qdv=(dv*ctx->fx_qfuv+2048)>>12;

	if(qdy || qdu || qdv)
		return(0);
	return(1);
}

int BTIC1H_CheckZeroDeltaYUVD(BTIC1H_Context *ctx,
	int cy, int cu, int cv, int cd)
{
	int bit0;
	int dy, du, dv, dd;
	int sdy, sdu, sdv, sdd;
	int qdy, qdu, qdv, qdd;
	int qdy1, qdu1, qdv1, qdd1;
	
	dy=cy-ctx->cy;
	du=cu-ctx->cu;
	dv=cv-ctx->cv;
	dd=cd-ctx->cd;

	qdy=(dy*ctx->fx_qfy +2048)>>12;
	qdu=(du*ctx->fx_qfuv+2048)>>12;
	qdv=(dv*ctx->fx_qfuv+2048)>>12;
	qdd=(dd*ctx->fx_qfd +2048)>>12;

#if 0
	sdy=dy*ctx->fx_qfy;		sdu=du*ctx->fx_qfuv;
	sdv=dv*ctx->fx_qfuv;	sdd=dd*ctx->fx_qfd;
	qdy=(sdy+2048)>>12;		qdu=(sdu+2048)>>12;
	qdv=(sdv+2048)>>12;		qdd=(sdd+2048)>>12;
	qdy1=sdy>>12;			qdu1=sdu>>12;
	qdv1=sdv>>12;			qdd1=sdd>>12;
	if(!qdy1)qdy=qdy1;
	if(!qdu1)qdu=qdu1;
	if(!qdv1)qdv=qdv1;
	if(!qdd1)qdd=qdd1;
#endif

	if(qdy || qdu || qdv || qdd)
		return(0);
	return(1);
}

int BTIC1H_CheckRefDeltaYUV(BTIC1H_Context *ctx,
	int *rcy, int *rcu, int *rcv,
	int cy, int cu, int cv)
{
	int bit0;
	int dy, du, dv, dd;
	int qdy, qdu, qdv, qdd;
	
	dy=cy-(*rcy);
	du=cu-(*rcu);
	dv=cv-(*rcv);

	qdy=(dy*ctx->fx_qfy +2048)>>12;
	qdu=(du*ctx->fx_qfuv+2048)>>12;
	qdv=(dv*ctx->fx_qfuv+2048)>>12;

	if(qdy || qdu || qdv)
	{
		*rcy+=qdy*ctx->qfy;
		*rcu+=qdu*ctx->qfuv;
		*rcv+=qdv*ctx->qfuv;
		return(1);
	}
	return(0);
}

int btic1h_errfrac2(int a, int b)
{
	static byte tab[4][4]={
		{0,1,4,8},	{1,0,1,4},
		{4,1,0,1},	{8,4,1,0}};
	return(tab[a][b]);
}

static byte btic1h_errfrac4_tab[16][16];
static int btic1h_errfrac4_init=0;

int btic1h_errfrac4(int a, int b)
{
	int i, j;
	return(btic1h_errfrac4_tab[a][b]);
}

int btic1h_errfrac8(int a, int b)
{
	return(
		btic1h_errfrac4(a>>4, b>>4)+
		btic1h_errfrac4(a&15, b&15));
}

#if 0
int BTIC1H_CalcBlockErrPix(byte *blka, byte *blkb)
{
	byte pxa[4], pxb[4];
	int u0, u1, ue;
	int i0, i1, i2, i3;
	int a, b, c, d;
	int i, j, k, l;
	
	a=blka[0];	b=blka[1];
	c=a-b; d=a+b;
	if(c<0)c=0;
	if(d>255)d=255;
	pxa[0]=c;				pxa[1]=(11*c+5*d)>>4;
	pxa[2]=(5*c+11*d)>>4;	pxa[3]=d;

	a=blkb[0];	b=blkb[1];
	c=a-b; d=a+b;
	if(c<0)c=0;
	if(d>255)d=255;
	pxb[0]=c;				pxb[1]=(11*c+5*d)>>4;
	pxb[2]=(5*c+11*d)>>4;	pxb[3]=d;

	u0=blka[2];
	u1=blkb[2];
	ue=u0-u1; ue=ue*ue;

//	d=0;
	d=ue*16;
	for(i=0; i<4; i++)
	{
		i0=blka[4+i];
		i1=blkb[4+i];
		for(j=0; j<4; j++)
		{
			i2=(i0>>(j*2))&3;
			i3=(i1>>(j*2))&3;
			a=pxa[i2];
			b=pxb[i3];
			c=a-b;
			d=d+c*c;
		}
	}

	return(sqrt(d/16));
}
#endif

int BTIC1H_CheckSkip(BTIC1H_Context *ctx,
	byte *blk, byte *lblk, int blksz,
	int qf, int dyem, int dyen,
	int duvem, int duven,
	int *rerr)
{
	int cy, cu, cv, cd, cm;
	int cy1, cu1, cv1, cd1, cm1;
	int ca, cb, ca1, cb1;
	int e0, e1, e2, e3, edy, euv;
	int mcy, ncy, dcy;
	int i, j, k, l;

	if(!memcmp(blk, lblk, blksz))
	{
		if(rerr)*rerr=0;
		return(2);
	}

	if(!btic1h_errfrac4_init)
	{
		for(i=0; i<16; i++)
			for(j=0; j<16; j++)
		{
			btic1h_errfrac4_tab[i][j]=
				btic1h_errfrac2(i>>2, j>>2)+
				btic1h_errfrac2(i&3, j&3);
		}
		btic1h_errfrac4_init=1;
	}

//	return(0);

	cy=blk[0]; cu=blk[1]; cv=blk[2]; cd=blk[3];
	cy1=lblk[0]; cu1=lblk[1]; cv1=lblk[2]; cd1=lblk[3];

	cm=0; cm1=0;

	if(cd==0)
	{
		cm=blk[4];
//		if((cm==1) || (cm==2))
		if(cm!=0)
			{ cd=blk[5]; }
			
		if((cm==16) || (cm==17))
			return(0);
	}

	if(cd1==0)
	{
		cm1=lblk[4];
//		if((cm1==1) || (cm1==2))
		if(cm1!=0)
			{ cd1=lblk[5]; }

		if((cm1==16) || (cm1==17))
			return(0);
	}

#if 0
	if((btic1h_subfold(cy1, cy)>dye) ||
		(btic1h_subfold(cd1, cd)>dye) ||
		(btic1h_subfold(cu1, cu)>duve) ||
		(btic1h_subfold(cv1, cv)>duve))
			return(0);

	ca=cy+(cd>>1);		cb=cy-(cd>>1);
	ca1=cy1+(cd1>>1);	cb1=cy1-(cd1>>1);

//	ca=cy+cd;		cb=cy-cd;
//	ca1=cy1+cd1;	cb1=cy1-cd1;

	if((btic1h_subfold(ca1, cb)<dye) &&
		(btic1h_subfold(cb1, ca)<dye))
			return(1);
#endif

#if 1
	e0=btic1h_subfold(cy1, cy);
	e1=btic1h_subfold(cd1, cd);
	e2=btic1h_subfold(cu1, cu);
	e3=btic1h_subfold(cv1, cv);
	
	edy=e0;
	euv=e2+e3;
	
//	if((e0>dyen) || (e1>dyen) || (e2>duven) || (e3>duven))
	if((e0>dyen) || (e1>dyen) || (euv>duven))
	{
//		e3=e2+e3;
		if(rerr)*rerr=e0+(e1>>1)+euv;
		return(0);
	}
#endif

#if 1
//	ca=cy+cd;		cb=cy-cd;
//	ca1=cy1+cd1;	cb1=cy1-cd1;

	ca=cy+(cd>>1);		cb=cy-(cd>>1);
	ca1=cy1+(cd1>>1);	cb1=cy1-(cd1>>1);

//	e2=btic1g_subfold(cu1, cu);
//	e3=btic1g_subfold(cv1, cv);
	e0=btic1h_subfold(ca1, cb);
	e1=btic1h_subfold(cb1, ca);

//	if((e0<dyem) && (e1<dyem) && (e2<duvem) && (e3<duvem))
	if((e0<dyem) && (e1<dyem) && (euv<duvem))
	{
//		e3=e2+e3;
//		e3=(e2>e3)?e2:e3;
		e2=(e0>e1)?e0:e1;
//		e2=(e0<e1)?e0:e1;
		if(rerr)*rerr=e2+euv;
		return(2);
	}
#endif

#if 1
	mcy=ca; ncy=ca;
	if(cb<mcy)mcy=cb;
	if(cb>ncy)ncy=cb;
	if(ca1<mcy)mcy=ca1;
	if(ca1>ncy)ncy=ca1;
	if(cb1<mcy)mcy=cb1;
	if(cb1>ncy)ncy=cb1;
	
	dcy=ncy-mcy;
#endif

#if 1
	if(cm!=cm1)
	{
//		e0=btic1h_subfold(cy1, cy);
//		e1=(cd+cd1)>>1;
//		e3=16*9;
//		e2=e0+((e1*e3)>>7);
//		e2=e0+e1;
		e2=dcy;
		if(rerr)*rerr=e2+euv;
//		return(e2<dyem);
		return(0);
	}
#endif

#if 1
	switch(cm)
	{
	case 0:
		e3=	btic1h_errfrac8(blk[4], lblk[4])+
			btic1h_errfrac8(blk[5], lblk[5])+
			btic1h_errfrac8(blk[6], lblk[6])+
			btic1h_errfrac8(blk[7], lblk[7]);
		break;
	case 1:
		e3=btic1h_errfrac8(blk[6], lblk[6])*4;
		break;
//	case 2:
//		e3=16*9;
//		break;
	case 8:		case 9:
		e3=btic1h_errfrac4(blk[6]&15, lblk[6]&15)*8;
		break;
	case 10:	case 11:
	case 12:	case 13:
		e3=(btic1h_errfrac8(blk[6], lblk[6])+
			btic1h_errfrac8(blk[7], lblk[7]))*2;
		break;
	default:
		e3=16*9;
		break;
	}

//	e0=btic1h_subfold(cy1, cy);
//	e1=(cd>cd1)?cd:cd1;
//	e2=e0+((e1*e3)>>7);
//	e1=cd+cd1;
//	e2=(e0-cd-cd1)+((e1*e3)>>7);

//	e1=(cd+cd1)>>1;
//	e2=e0+((e1*e3)>>7);
	e2=edy+((dcy*e3)>>7);

	if(rerr)*rerr=e2+euv;
	return(e2<dyem);
#endif

//	return(0);
}

int BTIC1H_EncodeCopyBlocks(
	byte *dblk, byte *sblk, int nblks, int stride)
{
	byte *ct, *cs, *cse;
	if(stride==32)
	{
		ct=dblk; cs=sblk; cse=sblk+nblks*32;
		while(cs<cse)
		{
			((u32 *)ct)[0]=((u32 *)cs)[0];
			((u32 *)ct)[1]=((u32 *)cs)[1];
			((u32 *)ct)[2]=((u32 *)cs)[2];
			((u32 *)ct)[3]=((u32 *)cs)[3];
			((u32 *)ct)[4]=((u32 *)cs)[4];
			((u32 *)ct)[5]=((u32 *)cs)[5];
#ifdef BT1H_ENABLE_AX
			if((cs[3]==0) && (cs[4]==7))
			{	((u32 *)ct)[6]=((u32 *)cs)[6];
				((u32 *)ct)[7]=((u32 *)cs)[7];	}
#else
			((u32 *)ct)[6]=((u32 *)cs)[6];
			((u32 *)ct)[7]=((u32 *)cs)[7];
#endif
			ct+=32; cs+=32;
		}
	}else
	{
		memcpy(dblk, sblk, nblks*stride);
	}
	return(0);
}

int BTIC1H_EncodeBlocksCtx(BTIC1H_Context *ctx,
	byte *blks, byte *lblks, int nblks, int stride,
	int *rnblks, int qf)
{
	static const int hilbert4x4[16]=
		{ 0, 1, 5, 4, 8, 12, 13, 9, 10, 14, 15, 11, 7, 6, 2, 3 };
	static const int hilbert2x2[4]=
		{ 0, 2, 3, 1 };
	static const int voffs[6*4][2]={
		{-1, 0},{ 0,-1},{1, 0},{ 0, 1},
		{-1, 1},{ 1, 1},{1,-1},{-1,-1},
		{-2, 0},{ 0,-2},{2, 0},{ 0, 2},
		{-2, 1},{ 1,-2},{2, 1},{ 1, 2},
		{-2,-1},{-1,-2},{2,-1},{-1, 2},
		{-2, 2},{ 2, 2},{2,-2},{-2,-2}
		};

	byte *cs2, *csl2;
	byte *cs, *cse, *csl, *csle, *cs1, *cs1e;
	int bit0;
	int cy, cu, cv, cd, cm;
//	int dye, duve, qr;
	int dyem, duvem, qr;
	int dyen, duven;
	int cy1, cu1, cv1, cd1, cm1;
	int cy2, cu2, cv2;
	int i0, i1, i2, i3;
	int vx, vy, vo;
	int i, j, k, l;
	
//	BTIC1H_InitQuant();

	qr=100-(qf&127);
	if(qr<0)qr=0;

//	dye=qr*0.5;
//	duve=qr*0.75;
//	dye=qr*0.3;
//	duve=qr*0.4;

	dyem=qr*0.2;
	duvem=qr*0.3;
//	duvem=qr*0.4;
	dyen=qr*0.4;
	duven=qr*0.6;
//	duven=qr*0.7;

	cs=blks; cse=cs+nblks*stride;
	csl=lblks; csle=lblks+nblks*stride;
	
//	cy=(100-qf)/4;
//	cu=(100-qf)/3;
//	cy=(100-qf)/8;
//	cu=(100-qf)/6;

//	cy=(100-qf)/6.5;
//	cu=(100-qf)/6;
//	cd=(100-qf)/5;

	cy=qr/6.5;
//	cu=qr/6;
	cu=qr/4;
	cd=qr/5;

	if(cy<1)cy=1;
	if(cu<1)cu=1;
	if(cd<1)cd=1;
	
	BTIC1H_EmitCommandCode(ctx, 0x04);
	BTIC1H_EmitDeltaQfYUVD(ctx, cy, cu, cd);

	BTIC1H_EmitCommandCode(ctx, 0x08);
	BTIC1H_EmitDeltaQfYUVDyuv(ctx, cy, cu, cd, cu);

	while(cs<cse)
	{
		if(lblks)
		{
			for(i=0; i<32; i++)
			{
				cs2=cs+i*stride;
				csl2=csl+i*stride;
				if(cs2>=cse)break;
//				if(!BTIC1H_CheckSkip(ctx, cs2, csl2, stride, qf, dye, duve))
//					break;
				if(BTIC1H_CheckSkip(ctx, cs2, csl2, stride,
						qf, dyem, dyen, duvem, duven, &k)<=0)
					break;
			}
			
			if(i>0)
			{
//				*ct++=0x80+(i-1);
				BTIC1H_EmitCommandCode(ctx, 0x21);
				BTIC1H_Rice_WriteAdRice(ctx, i, &(ctx->rk_cmdcnt));

				BTIC1H_EncodeCopyBlocks(cs, csl, i, stride);
//				memcpy(cs, csl, i*stride);

				cs+=i*stride;
				csl+=i*stride;
//				t1=GPIO_GetTimeUS();
				continue;
			}

#ifdef BT1H_USETSKIP
			i=0;
//			for(j=0; j<4; j++)
			for(j=0; j<BT1H_TSKIP_MAX; j++)
			{
				vx=voffs[j][0];
				vy=voffs[j][1];

				vo=(vy*ctx->xbsz+vx)*stride;
				for(i=0; i<32; i++)
				{
					cs2=cs+i*stride;
					csl2=csl+i*stride+vo;
					if(cs2>=cse)break;
					if((csl2<lblks) || (csl2>=csle))
						break;
					if(BTIC1H_CheckSkip(ctx, cs2, csl2, stride,
							qf, dyem, dyen, duvem, duven, &k)<=0)
						break;
				}
				if(i>0)break;
			}

			if(i>0)
			{
				BTIC1H_EmitCommandCode(ctx, 0x22);
				BTIC1H_Rice_WriteAdRice(ctx, i, &(ctx->rk_cmdcnt));
				BTIC1H_Rice_WriteAdSRice(ctx, vx, &(ctx->rk_parmxy));
				BTIC1H_Rice_WriteAdSRice(ctx, vy, &(ctx->rk_parmxy));

				csl2=csl+vo;

				BTIC1H_EncodeCopyBlocks(cs, csl2, i, stride);
//				memcpy(cs, csl2, i*stride);

				cs+=i*stride;
				csl+=i*stride;
//				t1=GPIO_GetTimeUS();
				continue;
			}
#endif
		}

		cy=cs[0]; cu=cs[1]; cv=cs[2]; cd=cs[3];
		
		if(cd)
		{
#if 1
			if(BTIC1H_CheckZeroDeltaYUVD(ctx, cy, cu, cv, cd))
			{
				cs1=cs+stride;
				cs1e=cs+256*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					cy1=cs1[0]; cu1=cs1[1]; cv1=cs1[2]; cd1=cs1[3];
					if(!cd1)
						break;
					if(!BTIC1H_CheckZeroDeltaYUVD(ctx, cy1, cu1, cv1, cd1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;

				BTIC1H_EmitCommandCode(ctx, 0x15);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
				{
					BTIC1H_Rice_Write16Bits(ctx, (cs[4]<<8)|cs[5]);
					BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
					ctx->bits_pix4x4+=32;
					cs+=stride;
					csl+=stride;
				}
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x05);
			BTIC1H_EmitDeltaYUVD(ctx, cy, cu, cv, cd);
			BTIC1H_Rice_Write16Bits(ctx, (cs[4]<<8)|cs[5]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
			ctx->bits_pix4x4+=32;
			cs+=stride;
			csl+=stride;
			continue;
		}
		
		cm=cs[4];
		cd=cs[5];
		
		if(cm==0)
		{
#if 1
			if(BTIC1H_CheckZeroDeltaYUV(ctx, cy, cu, cv))
			{
				cs1=cs+stride;
				cs1e=cs+256*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[3] || (cs1[4]!=0))
						break;
					cy1=cs1[0]; cu1=cs1[1]; cv1=cs1[2];
					if(!BTIC1H_CheckZeroDeltaYUV(ctx, cy1, cu1, cv1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;
				BTIC1H_EmitCommandCode(ctx, 0x10);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				cs+=l*stride;
				csl+=l*stride;
				continue;
			}
#endif

#if 1
			cs1=cs+stride;
			cs1e=cs+256*stride;
			if(cse<cs1e)cs1e=cse;
			cy2=ctx->cy; cu2=ctx->cu; cv2=ctx->cv; k=0;
			while(cs1<cs1e)
			{
				if(cs1[3] || (cs1[4]!=0))
					break;
				cy1=cs1[0]; cu1=cs1[1]; cv1=cs1[2];
				if(!BTIC1H_CheckRefDeltaYUV(ctx,
						&cy2, &cu2, &cv2, cy1, cu1, cv1))
					{ if((++k)>=3)break; }
				else
					{ k=0; }
				cs1+=stride;
			}
			
			cs1-=k*stride;
			l=(cs1-cs)/stride;
			if(l>=3)
			{
				BTIC1H_EmitCommandCode(ctx, 0x1B);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
				{
					BTIC1H_EmitDeltaYUV(ctx,
						cs[0], cs[1], cs[2]);
					cs+=stride;
					csl+=stride;
				}
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x00);
			BTIC1H_EmitDeltaYUV(ctx, cy, cu, cv);
			cs+=stride;
			csl+=stride;
			continue;
		}

		if(cm==1)
		{
#if 1
			if(BTIC1H_CheckZeroDeltaYUVD(ctx, cy, cu, cv, cd))
			{
				cs1=cs+stride;
				cs1e=cs+256*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[3] || (cs1[4]!=1))
						break;
					cy1=cs1[0]; cu1=cs1[1]; cv1=cs1[2]; cd1=cs1[5];
					if(!BTIC1H_CheckZeroDeltaYUVD(ctx, cy1, cu1, cv1, cd1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;
				BTIC1H_EmitCommandCode(ctx, 0x11);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
				{
					BTIC1H_Rice_Write8Bits(ctx, cs[6]);
					ctx->bits_pix2x2+=8;
					cs+=stride;
					csl+=stride;
				}
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x01);
			BTIC1H_EmitDeltaYUVD(ctx, cy, cu, cv, cd);
			BTIC1H_Rice_Write8Bits(ctx, cs[6]);
			ctx->bits_pix2x2+=8;
			cs+=stride;
			csl+=stride;
			continue;
		}

		if(cm==2)
		{
#if 1
			if(BTIC1H_CheckZeroDeltaYUVD(ctx, cy, cu, cv, cd))
			{
				cs1=cs+stride;
				cs1e=cs+256*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[3] || (cs1[4]!=2))
						break;
					cy1=cs1[0]; cu1=cs1[1]; cv1=cs1[2]; cd1=cs1[5];
					if(!BTIC1H_CheckZeroDeltaYUVD(ctx, cy1, cu1, cv1, cd1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;
				BTIC1H_EmitCommandCode(ctx, 0x1A);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
				{
					BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
					ctx->bits_pix4x4x1+=16;
					cs+=stride;
					csl+=stride;
				}
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x2A);
			BTIC1H_EmitDeltaYUVD(ctx, cy, cu, cv, cd);
			BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
			ctx->bits_pix4x4x1+=16;
			cs+=stride;
			csl+=stride;
			continue;
		}

		if(cm==5)
		{
#if 1
			if(BTIC1H_CheckZeroDeltaYUVD(ctx, cy, cu, cv, cd))
			{
				cs1=cs+stride;
				cs1e=cs+256*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[3] || (cs1[4]!=5))
						break;
					cy1=cs1[0]; cu1=cs1[1]; cv1=cs1[2]; cd1=cs1[5];
					if(!BTIC1H_CheckZeroDeltaYUVD(ctx, cy1, cu1, cv1, cd1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;
				BTIC1H_EmitCommandCode(ctx, 0x19);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
				{
					BTIC1H_Rice_Write4Bits(ctx, cs[6]);
					ctx->bits_pix4x4x1+=4;
					cs+=stride;
					csl+=stride;
				}
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x2B);
			BTIC1H_EmitDeltaYUVD(ctx, cy, cu, cv, cd);
			BTIC1H_Rice_Write4Bits(ctx, cs[6]);
			ctx->bits_pix4x4x1+=4;
			cs+=stride;
			csl+=stride;
			continue;
		}

		if(cm==6)
		{
			BTIC1H_EmitCommandCode(ctx, 0x2C);
			BTIC1H_EmitDeltaYUVD(ctx, cy, cu, cv, cd);
			BTIC1H_Rice_Write16Bits(ctx, (cs[10]<<8)|cs[11]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[12]<<8)|cs[13]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[14]<<8)|cs[15]);
			ctx->bits_pix4x4x3+=48;
			cs+=stride;
			csl+=stride;
			continue;
		}

#ifdef BT1H_USEHV
		if((cm==8) || (cm==9))
		{
#if 1
			if(BTIC1H_CheckZeroDeltaYUVD(ctx, cy, cu, cv, cd))
			{
				cs1=cs+stride;
				cs1e=cs+256*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[3] || (cs1[4]!=cm))
						break;
					cy1=cs1[0]; cu1=cs1[1]; cv1=cs1[2]; cd1=cs1[5];
					if(!BTIC1H_CheckZeroDeltaYUVD(ctx, cy1, cu1, cv1, cd1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;
				BTIC1H_EmitCommandCode(ctx, 0x12+(cm-8));
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
				{
					BTIC1H_Rice_Write4Bits(ctx, cs[6]);
					ctx->bits_pix2x1+=4;
					cs+=stride;
					csl+=stride;
				}
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x02+(cm-8));
			BTIC1H_EmitDeltaYUVD(ctx, cy, cu, cv, cd);
			BTIC1H_Rice_Write4Bits(ctx, cs[6]);
			ctx->bits_pix2x1+=4;
			cs+=stride;
			csl+=stride;
			continue;
		}
#endif

#ifdef BT1H_USEHV
		if((cm==10) || (cm==11))
		{
#if 1
			if(BTIC1H_CheckZeroDeltaYUVD(ctx, cy, cu, cv, cd))
			{
				cs1=cs+stride;
				cs1e=cs+256*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[3] || (cs1[4]!=cm))
						break;
					cy1=cs1[0]; cu1=cs1[1]; cv1=cs1[2]; cd1=cs1[5];
					if(!BTIC1H_CheckZeroDeltaYUVD(ctx, cy1, cu1, cv1, cd1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;
				BTIC1H_EmitCommandCode(ctx, 0x16+(cm-10));
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
				{
					BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
					ctx->bits_pix4x2+=16;
					cs+=stride;
					csl+=stride;
				}
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x06+(cm-10));
			BTIC1H_EmitDeltaYUVD(ctx, cy, cu, cv, cd);
			BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
			ctx->bits_pix4x2+=16;
			cs+=stride;
			csl+=stride;
			continue;
		}
#endif

		if(cm==7)
		{
			BTIC1H_EmitCommandCode(ctx, 0x2E);
			for(i=0; i<16; i++)
			{
				BTIC1H_EmitDeltaY(ctx, cs[16+hilbert4x4[i]]);
			}
			for(i=0; i<4; i++)
			{
				BTIC1H_EmitDeltaUV(ctx,
					cs[8+hilbert2x2[i]], cs[12+hilbert2x2[i]]);
			}

			cs+=stride;
			csl+=stride;
			continue;
		}

		if(cm==17)
		{
			BTIC1H_EmitCommandCode(ctx, 0x1D);

			BTIC1H_EmitDeltaYUVD(ctx, cs[ 0], cs[ 1], cs[ 2], cs[ 5]);
			BTIC1H_EmitDeltaYUVD(ctx, cs[16], cs[17], cs[18], cs[19]);
			BTIC1H_EmitDeltaYUVD(ctx, cs[20], cs[21], cs[22], cs[23]);
			BTIC1H_EmitDeltaYUVD(ctx, cs[ 8], cs[ 9], cs[10], cs[11]);

			BTIC1H_Rice_Write16Bits(ctx, (cs[12]<<8)|cs[13]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[14]<<8)|cs[15]);
			ctx->bits_pix4x4+=32;

			cs+=stride;
			csl+=stride;
			continue;
		}

#if 1
		if(cm==20)
		{
			BTIC1H_EmitCommandCode(ctx, 0x30);

			BTIC1H_EmitDeltaYUVDyuv(ctx,
				cs[ 0], cs[ 1], cs[ 2],
				(cs[5]-128)<<1, (cs[8]-128)<<1,
				(cs[9]-128)<<1);

			BTIC1H_Rice_Write16Bits(ctx, (cs[12]<<8)|cs[13]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[14]<<8)|cs[15]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[10]<<8)|cs[11]);
			ctx->bits_pix4x4+=32;
			ctx->bits_pix2x2+=16;

			cs+=stride;
			csl+=stride;
			continue;
		}

		if(cm==21)
		{
			BTIC1H_EmitCommandCode(ctx, 0x31);

			BTIC1H_EmitDeltaYUVDyuv(ctx,
				cs[ 0], cs[ 1], cs[ 2],
				(cs[5]-128)<<1, (cs[8]-128)<<1,
				(cs[9]-128)<<1);

			BTIC1H_Rice_Write16Bits(ctx, (cs[10]<<8)|cs[11]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[12]<<8)|cs[13]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[14]<<8)|cs[15]);
			ctx->bits_pix4x4x3+=48;
			BTIC1H_Rice_Write16Bits(ctx, (cs[16]<<8)|cs[17]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[18]<<8)|cs[19]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[20]<<8)|cs[21]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[22]<<8)|cs[23]);
			ctx->bits_pix4x4+=64;

			cs+=stride;
			csl+=stride;
			continue;
		}

		if(cm==22)
		{
			BTIC1H_EmitCommandCode(ctx, 0x32);

			BTIC1H_EmitDeltaYUVDyuv(ctx,
				cs[ 0], cs[ 1], cs[ 2],
				(cs[5]-128)<<1, (cs[8]-128)<<1,
				(cs[9]-128)<<1);

			BTIC1H_Rice_Write16Bits(ctx, (cs[10]<<8)|cs[11]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[12]<<8)|cs[13]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[14]<<8)|cs[15]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
			ctx->bits_pix4x4x3+=48;
			ctx->bits_pix2x2+=16;

			cs+=stride;
			csl+=stride;
			continue;
		}
#endif

#ifdef BT1H_USEGRAD
		if(cm==18)
		{
			cs1=cs+stride;
			cs1e=cs+256*stride;
			if(cse<cs1e)cs1e=cse;
			while(cs1<cs1e)
			{
				if(cs1[3] || (cs1[4]!=18))
					break;
//				cy1=cs1[0]; cu1=cs1[1]; cv1=cs1[2]; cd1=cs1[5];
//				if(!BTIC1H_CheckZeroDeltaYUVD(ctx, cy1, cu1, cv1, cd1))
//					break;
				cs1+=stride;
			}
				
			l=(cs1-cs)/stride;
			BTIC1H_EmitCommandCode(ctx, 0x28);
			BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
			for(i=0; i<l; i++)
			{
				BTIC1H_EmitDeltaYUVD(ctx, cs[0], cs[1], cs[2], cs[5]);
				BTIC1H_Rice_Write8Bits(ctx, cs[6]);
				ctx->bits_pix2x2+=8;
				cs+=stride;
				csl+=stride;
			}
			continue;
		}
#endif

		printf("Invalid Block Type %d\n\n", cm);
		break;
	}

	BTIC1H_EmitCommandCode(ctx, 0x20);
	
	*rnblks=(cs-blks)/stride;
	return(0);
}

#ifdef BT1H_ENABLE_AX
int BTIC1H_CheckZeroDeltaA(BTIC1H_Context *ctx, int cy)
{
	int dy, dd, qdy, qdd;
	
	dy=cy-ctx->cy;
	qdy=(dy*ctx->fx_qfy+2048)>>12;

	if(qdy)
		return(0);
	return(1);
}

int BTIC1H_CheckZeroDeltaAD(BTIC1H_Context *ctx,
	int cy, int cd)
{
	int dy, dd, qdy, qdd;
	
	dy=cy-ctx->cy;		dd=cd-ctx->cd;
	qdy=(dy*ctx->fx_qfy+2048)>>12;
	qdd=(dd*ctx->fx_qfd+2048)>>12;

	if(qdy || qdd)
		return(0);
	return(1);
}

int BTIC1H_CheckAlphaSkip(BTIC1H_Context *ctx,
	byte *blk, byte *lblk, int blksz,
	int qf, int dyem, int dyen,
	int *rerr)
{
	int cy, cu, cv, cd, cm;
	int cy1, cu1, cv1, cd1, cm1;
	int ca, cb, ca1, cb1;
	int e0, e1, e2, e3, edy, euv;
	int mcy, ncy, dcy;
	int i, j, k, l;

	if(!memcmp(blk, lblk, 8))
	{
		if(rerr)*rerr=0;
		return(2);
	}

	cy=blk[0]; cd=blk[1];
	cy1=lblk[0]; cd1=lblk[1];

	cm=0; cm1=0;

	if(cd==0)
	{
		cm=blk[3];
		if((cm!=0) && (cm!=7))
			{ cd=blk[2]; }
	}

	if(cd1==0)
	{
		cm1=lblk[3];
		if((cm1!=0) && (cm1!=7))
			{ cd1=lblk[2]; }
	}

#if 0
	if((btic1h_subfold(cy1, cy)>dye) ||
		(btic1h_subfold(cd1, cd)>dye) ||
		(btic1h_subfold(cu1, cu)>duve) ||
		(btic1h_subfold(cv1, cv)>duve))
			return(0);

	ca=cy+(cd>>1);		cb=cy-(cd>>1);
	ca1=cy1+(cd1>>1);	cb1=cy1-(cd1>>1);

//	ca=cy+cd;		cb=cy-cd;
//	ca1=cy1+cd1;	cb1=cy1-cd1;

	if((btic1h_subfold(ca1, cb)<dye) &&
		(btic1h_subfold(cb1, ca)<dye))
			return(1);
#endif

#if 1
	e0=btic1h_subfold(cy1, cy);
	e1=btic1h_subfold(cd1, cd);
	edy=e0;
	
	if((e0>dyen) || (e1>dyen))
	{
		if(rerr)*rerr=e0+(e1>>1);
		return(0);
	}
#endif

#if 1
	ca=cy+(cd>>1);		cb=cy-(cd>>1);
	ca1=cy1+(cd1>>1);	cb1=cy1-(cd1>>1);

	e0=btic1h_subfold(ca1, cb);
	e1=btic1h_subfold(cb1, ca);

	if((e0<dyem) && (e1<dyem))
	{
		e2=(e0>e1)?e0:e1;
		if(rerr)*rerr=e2;
		return(2);
	}
#endif

	e2=(e0>e1)?e0:e1;
	if(rerr)*rerr=e2;

	return(0);
}

int BTIC1H_EncodeCopyAlphaBlocks(
	byte *dblk, byte *sblk, int nblks, int stride)
{
	byte *ct, *cs, *cse;
	if(stride==32)
	{
		ct=dblk; cs=sblk; cse=sblk+nblks*32;
		while(cs<cse)
		{
			((u32 *)ct)[0]=((u32 *)cs)[0];
			((u32 *)ct)[1]=((u32 *)cs)[1];
			ct+=32; cs+=32;
		}
	}else
	{
//		memcpy(dblk, sblk, nblks*stride);
	}
	return(0);
}

int BTIC1H_EncodeAlphaBlocksCtx(BTIC1H_Context *ctx,
	byte *blks, byte *lblks, int nblks, int stride,
	int *rnblks, int qf)
{
	byte *cs2, *csl2;
	byte *cs, *cse, *csl, *csle, *cs1, *cs1e;
	int bit0;
	int cy, cu, cv, cd, cm;
	int dyem, dyen, qr;
	int cy1, cu1, cv1, cd1, cm1;
	int cy2, cu2, cv2;
	int i0, i1, i2, i3;
	int vx, vy, vo;
	int i, j, k, l;
	
//	BTIC1H_InitQuant();

	qr=100-(qf&127);
	if(qr<0)qr=0;

	dyem=qr*0.2;
	dyen=qr*0.4;

	cs=blks; cse=cs+nblks*stride;
	csl=lblks; csle=lblks+nblks*stride;
	
	cy=qr/6.5;
	cd=qr/5;

	if(cy<1)cy=1;
	if(cd<1)cd=1;
	
	BTIC1H_EmitCommandCode(ctx, 0x04);
	BTIC1H_EmitDeltaQfAD(ctx, cy, cd);

	while(cs<cse)
	{
		if(lblks)
		{
			for(i=0; i<32; i++)
			{
				cs2=cs+i*stride;
				csl2=csl+i*stride;
				if(cs2>=cse)break;
				if(BTIC1H_CheckAlphaSkip(ctx, cs2, csl2, stride,
						qf, dyem, dyen, &k)<=0)
					break;
			}
			
			if(i>0)
			{
				BTIC1H_EmitCommandCode(ctx, 0x21);
				BTIC1H_Rice_WriteAdRice(ctx, i, &(ctx->rk_cmdcnt));
				
				BTIC1H_EncodeCopyAlphaBlocks(cs, csl, i, stride);
				cs+=i*stride;		csl+=i*stride;
				continue;
			}
		}

		if(cs[1]!=0)
		{
			cy=cs[0];
			cd=cs[1];

			BTIC1H_EmitCommandCode(ctx, 0x03);
			BTIC1H_EmitDeltaAD(ctx, cy, cd);
			BTIC1H_Rice_Write16Bits(ctx, (cs[2]<<8)|cs[3]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[4]<<8)|cs[5]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
			ctx->bits_pix4x4x3+=48;
			cs+=stride;		csl+=stride;
			continue;
		}

		cy=cs[0];
		cd=cs[2];
		cm=cs[3];

		if(cm==7)
		{
#if 1
			if(cy==0)
			{
				cs1=cs+stride;
				cs1e=cs+32*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[1] || (cs1[3]!=cm))
						break;
					if(cs1[2]!=0)
						break;
					cs1+=stride;
				}
			
				l=(cs1-cs)/stride;
				if(l>1)
				{
					BTIC1H_EmitCommandCode(ctx, 0x12);
					BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
					cs+=l*stride; csl+=l*stride;
					continue;
				}

				BTIC1H_EmitCommandCode(ctx, 0x11);
				cs+=stride;		csl+=stride;
				continue;
			}
#endif
		
#if 1
			if(BTIC1H_CheckZeroDeltaA(ctx, cy))
			{
				cs1=cs+stride;
				cs1e=cs+32*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[1] || (cs1[3]!=cm))
						break;
					cy1=cs1[0]; cd1=cs1[2];
					if(!BTIC1H_CheckZeroDeltaA(ctx, cy1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;
				BTIC1H_EmitCommandCode(ctx, 0x0C);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
					{ cs+=stride; csl+=stride; }
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x00);
			BTIC1H_EmitDeltaA(ctx, cy);
			cs+=stride;		csl+=stride;
			continue;
		}

		if(cm==1)
		{
#if 1
			if(BTIC1H_CheckZeroDeltaAD(ctx, cy, cd))
			{
				cs1=cs+stride;
				cs1e=cs+32*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[1] || (cs1[3]!=cm))
						break;
					cy1=cs1[0]; cd1=cs1[2];
					if(!BTIC1H_CheckZeroDeltaAD(ctx, cy1, cd1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;
				BTIC1H_EmitCommandCode(ctx, 0x0D);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
				{
					BTIC1H_Rice_Write8Bits(ctx, cs[6]);
					ctx->bits_pix2x2+=8;
					cs+=stride;		csl+=stride;
				}
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x01);
			BTIC1H_EmitDeltaAD(ctx, cy, cd);
			BTIC1H_Rice_Write8Bits(ctx, cs[6]);
			ctx->bits_pix2x2+=8;
			cs+=stride;		csl+=stride;
			continue;
		}

		if(cm==6)
		{
#if 1
			if(BTIC1H_CheckZeroDeltaAD(ctx, cy, cd))
			{
				cs1=cs+stride;
				cs1e=cs+32*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[1] || (cs1[3]!=cm))
						break;
					cy1=cs1[0]; cd1=cs1[2];
					if(!BTIC1H_CheckZeroDeltaAD(ctx, cy1, cd1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;
				BTIC1H_EmitCommandCode(ctx, 0x0E);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
				{
					BTIC1H_Rice_Write16Bits(ctx, (cs[4]<<8)|cs[5]);
					BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
					ctx->bits_pix4x4+=32;
					cs+=stride;		csl+=stride;
				}
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x02);
			BTIC1H_EmitDeltaAD(ctx, cy, cd);
			BTIC1H_Rice_Write16Bits(ctx, (cs[4]<<8)|cs[5]);
			BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
			ctx->bits_pix4x4+=32;
			cs+=stride;		csl+=stride;
			continue;
		}

		if(cm==2)
		{
#if 1
			if(BTIC1H_CheckZeroDeltaAD(ctx, cy, cd))
			{
				cs1=cs+stride;
				cs1e=cs+32*stride;
				if(cse<cs1e)cs1e=cse;
				while(cs1<cs1e)
				{
					if(cs1[1] || (cs1[3]!=cm))
						break;
					cy1=cs1[0]; cd1=cs1[2];
					if(!BTIC1H_CheckZeroDeltaAD(ctx, cy1, cd1))
						break;
					cs1+=stride;
				}
				
				l=(cs1-cs)/stride;
				BTIC1H_EmitCommandCode(ctx, 0x0F);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				for(i=0; i<l; i++)
				{
					BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
					ctx->bits_pix4x4x1+=16;
					cs+=stride;		csl+=stride;
				}
				continue;
			}
#endif

			BTIC1H_EmitCommandCode(ctx, 0x05);
			BTIC1H_EmitDeltaAD(ctx, cy, cd);
			BTIC1H_Rice_Write16Bits(ctx, (cs[6]<<8)|cs[7]);
			ctx->bits_pix4x4x1+=32;
			cs+=stride;		csl+=stride;
			continue;
		}

		if(cm==0)
		{
			cs1=cs+stride;
			cs1e=cs+32*stride;
			if(cse<cs1e)cs1e=cse;
			while(cs1<cs1e)
			{
				if(cs1[1] || (cs1[3]!=cm))
					break;
				cs1+=stride;
			}
			
			l=(cs1-cs)/stride;
			if(l>1)
			{
				BTIC1H_EmitCommandCode(ctx, 0x0B);
				BTIC1H_Rice_WriteAdRice(ctx, l, &(ctx->rk_cmdcnt));
				cs+=l*stride; csl+=l*stride;
				continue;
			}

			BTIC1H_EmitCommandCode(ctx, 0x0A);
			cs+=stride;		csl+=stride;
			continue;
		}

		printf("Invalid Alpha Block Type %d\n\n", cm);
		break;
	}

	BTIC1H_EmitCommandCode(ctx, 0x20);
	
	*rnblks=(cs-blks)/stride;
	return(0);
}
#endif

BTIC1H_Context *BTIC1H_AllocContext(void)
{
	BTIC1H_Context *tmp;
	
	tmp=malloc(sizeof(BTIC1H_Context));
	memset(tmp, 0, sizeof(BTIC1H_Context));
	return(tmp);
}

int BTIC1H_DestroyContext(BTIC1H_Context *ctx)
{
	if(ctx->blks)free(ctx->blks);
	if(ctx->lblks)free(ctx->lblks);
	free(ctx);
	return(0);
}

void BTIC1H_SetupContextInitial(BTIC1H_Context *ctx)
{
	int i;

	ctx->mark1=0x12345678;
	ctx->mark2=0x12488421;
	ctx->mark3=0x87654321;

	for(i=0; i<16; i++)
		{ ctx->cmdwin[i]=-1; }

	for(i=0; i<64; i++)
		{ ctx->cmdidx[i]=-1; }

	ctx->cmdwpos=0;

	ctx->cy=0;	ctx->cu=0;
	ctx->cv=0;	ctx->cd=0;
	
	ctx->absyuvbias=128;
	ctx->dyuv=0;
	
	ctx->cdy=0;		ctx->cdu=0;		ctx->cdv=0;

	ctx->qfy=1;		ctx->qfuv=1;	ctx->qfd=1;
	ctx->qfdy=1;	ctx->qfduv=1;

	ctx->qfay=1;	ctx->qfauv=1;	ctx->qfad=1;
	ctx->qfady=1;	ctx->qfaduv=1;

	ctx->rk_cmdidx=2;	ctx->rk_cmdabs=4;
	ctx->rk_cmdcnt=2;

	ctx->rk_parmxy=2;	ctx->rk_parmvar=2;
	ctx->rk_parmval=2;	ctx->rk_parmix=2;

	ctx->rk_dy=2;		ctx->rk_duv=2;
	ctx->rk_ddy=2;		ctx->rk_dduv=2;

	ctx->rk_ay=5;		ctx->rk_auv=5;
	ctx->rk_ady=5;		ctx->rk_aduv=5;

	ctx->rk_qfy=3;		ctx->rk_qfuv=3;
	ctx->rk_qfdy=3;		ctx->rk_qfduv=3;

#if 1
	ctx->cnt_cmds=0;		ctx->bits_cmdidx=0;
	ctx->bits_cmdabs=0;

	ctx->bits_dyuv=0;		ctx->bits_dy=0;
	ctx->bits_duv=0;		ctx->bits_ddy=0;
	ctx->bits_dduv=0;

	ctx->bits_pix4x4=0;		ctx->bits_pix4x4x1=0;
	ctx->bits_pix4x4x3=0;	ctx->bits_pix4x2=0;
	ctx->bits_pix2x2=0;		ctx->bits_pix2x1=0;
#endif
}

int BTIC1H_EncodeCtx(BTIC1H_Context *ctx,
	byte *src, byte *dst, int dsz,
	int xs, int ys, int qfl, int clrs)
{
	byte *ct, *cte;
	int sz, sz1;
	int i, n;

	if(!ctx->blks)
	{
		ctx->xbsz=(ctx->xs+3)>>2;
		ctx->ybsz=(ctx->ys+3)>>2;
		ctx->nblks=ctx->xbsz*ctx->ybsz;
		
		n=(ctx->xbsz+1)*(ctx->ybsz+1);
		ctx->blks=malloc(n*32);
		ctx->lblks=malloc(n*32);
	}

	BTIC1H_EncodeImageClrs(ctx->blks, 32, src,
		ctx->xs, ctx->flip?ctx->ys:(-ctx->ys), qfl, clrs);

	ct=dst+4; cte=dst+dsz;

	BTIC1H_SetupContextInitial(ctx);

	for(i=0; i<64; i++)
		ctx->cmdidx[i]=-1;
	
	BTIC1H_Rice_SetupWrite(ctx, dst+4, dsz-4);
	
	BTIC1H_EncodeBlocksCtx(ctx,
		ctx->blks, (qfl&BTIC1H_QFL_PFRAME)?ctx->lblks:NULL,
		ctx->nblks, 32, &i, qfl);
	
	BTIC1H_Rice_FlushBits(ctx);
	sz=ctx->bs_ct-dst;
	dst[0]=0xE1;
	dst[1]=sz>>16;
	dst[2]=sz>>8;
	dst[3]=sz;
	
#ifdef BT1H_ENABLE_AX
	if((clrs==BTIC1H_PXF_RGBA) || (clrs==BTIC1H_PXF_BGRA))
	{
		ct=dst+sz;

		BTIC1H_Rice_SetupWrite(ctx, ct+6, cte-ct-6);
	
		BTIC1H_EncodeAlphaBlocksCtx(ctx,
			ctx->blks+24, (qfl&BTIC1H_QFL_PFRAME)?(ctx->lblks+24):NULL,
			ctx->nblks, 32, &i, qfl);
	
		BTIC1H_Rice_FlushBits(ctx);
		sz1=ctx->bs_ct-ct;
		dst[0]=0xE3;
		dst[1]=sz1>>16;
		dst[2]=sz1>>8;
		dst[3]=sz1;
		dst[4]='A';
		dst[5]='X';

		sz=ctx->bs_ct-dst;
	}
#endif

	memcpy(ctx->lblks, ctx->blks, ctx->nblks*32);

	if(sz>dsz)
	{
		*(int *)-1=-1;
	}

	return(sz);
}

BTIC1H_Context btic1h_enc_tctx;

byte *BTIC1H_EncodeBlocksBuffer(byte *obuf,
	byte *blks, byte *lblks, int nblks, int stride,
	int obmsz, int *rnblks, int qf)
{
	BTIC1H_Context *ctx;
	int i;
	
	ctx=&btic1h_enc_tctx;
	memset(ctx, 0, sizeof(BTIC1H_Context));
	
	BTIC1H_SetupContextInitial(ctx);
	
	for(i=0; i<64; i++)
		ctx->cmdidx[i]=-1;
	
	BTIC1H_Rice_SetupWrite(ctx, obuf, obmsz);
	
	BTIC1H_EncodeBlocksCtx(ctx, blks, lblks, nblks, stride, rnblks, qf);
	
	BTIC1H_Rice_FlushBits(ctx);
	return(ctx->bs_ct);
}

void BTIC1H_DumpEncodeStats(BTIC1H_Context *ctx)
{
	double rfbits;
	int i;
	
	rfbits=100.0/ctx->bs_bits;
	
	printf("total_bits=%d\n", ctx->bs_bits);
	printf("ncmds=%d, bits=%d: cmdidx=%d cmdabs=%d, cmdavg=%f\n",
		ctx->cnt_cmds,
		ctx->bits_cmdidx+ctx->bits_cmdabs,
		ctx->bits_cmdidx, ctx->bits_cmdabs,
		(ctx->bits_cmdidx+ctx->bits_cmdabs)/(ctx->cnt_cmds+1.0));
	printf("dyuv=%d dy=%d duv=%d ddy=%d\n",
		ctx->bits_dyuv, ctx->bits_dy, ctx->bits_duv, ctx->bits_ddy);
	printf("4x4=%d 4x4x1=%d 4x4x3=%d 4x2=%d 2x2=%d 2x1=%d\n",
		ctx->bits_pix4x4, ctx->bits_pix4x4x1,
		ctx->bits_pix4x4x3, ctx->bits_pix4x2,
		ctx->bits_pix2x2, ctx->bits_pix2x1);
	printf("\n");

	printf("ncmds=%d, bits=%2.02f%%: cmdidx=%2.02f%% cmdabs=%2.02f%%\n",
		ctx->cnt_cmds,
		(ctx->bits_cmdidx+ctx->bits_cmdabs)*rfbits,
		ctx->bits_cmdidx*rfbits,
		ctx->bits_cmdabs*rfbits);
	printf(	"dyuvd=%2.02f%% dyuv=%2.02f%% "
			"dy=%2.02f%% duv=%2.02f%% ddy=%2.02f%%\n",
		(ctx->bits_dyuv+ctx->bits_ddy)*rfbits,
		ctx->bits_dyuv*rfbits, ctx->bits_dy*rfbits,
		ctx->bits_duv*rfbits, ctx->bits_ddy*rfbits);
	printf("pixel_data=%2.02f%%\n",
		(ctx->bits_pix4x4+ctx->bits_pix4x4x1+
		ctx->bits_pix4x4x3+ctx->bits_pix4x2+
		ctx->bits_pix2x2+ctx->bits_pix2x1)*rfbits);
	printf(	"  4x4x2=%-2.02f%% 4x4x1=%-2.02f%% 4x4x3=%-2.02f%%\n"
			"  4x2x2=%-2.02f%% 2x2x2=%-2.02f%% 2x1x2=%-2.02f%%\n",
		ctx->bits_pix4x4*rfbits, ctx->bits_pix4x4x1*rfbits,
		ctx->bits_pix4x4x3*rfbits, ctx->bits_pix4x2*rfbits,
		ctx->bits_pix2x2*rfbits, ctx->bits_pix2x1*rfbits);
	printf("\n");
}

void BTIC1H_DumpGeneralEncodeStats()
{
	BTIC1H_Context *ctx;
	int i;
	
	ctx=&btic1h_enc_tctx;
	BTIC1H_DumpEncodeStats(ctx);
}
