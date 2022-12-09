#define TKDELZ_HASH_SZ	4096
#define TKDELZ_HASH_LVL	64

#define TKDELZ_MAX_DIST_LG2		18
#define TKDELZ_MAX_DIST			(1<<TKDELZ_MAX_DIST_LG2)
#define TKDELZ_MAX_MATCH		(1<<16)

#define TKDELZ_TBLK_LG2SZ	16

#define TKDELZ_TBLK_SZ	(1<<TKDELZ_TBLK_LG2SZ)
#define TKDELZ_TBUF_SZ	((1<<TKDELZ_TBLK_LG2SZ)+4096)

#define TKDELZ_LVL_FASTDEC	16

#ifndef TKDELZ_BYTE
#define TKDELZ_BYTE
typedef unsigned char		byte;
typedef unsigned short	u16;
typedef unsigned int		u32;

typedef signed char		sbyte;
typedef signed short		s16;
typedef signed int		s32;
#endif

typedef struct TkDeLz_EncState_s TkDeLz_EncState;

struct TkDeLz_EncState_s {
	u32 magic1;

	byte *ct;
	u32 win;
	sbyte pos;
	byte status;
	byte lvl;
	int blktag;

	byte **hash_pos;
	byte *hash_rov;
	
//	byte tbuf[TKDELZ_TBUF_SZ];

	u32 hetab_t[256];
	u32 hetab_l[256];
	u32 hetab_d[256];
	u32 hetab_e[256];

	byte hltab_t[256];
	byte hltab_l[256];
	byte hltab_d[256];
	byte hltab_e[256];

	int hstab_t[256];
	int hstab_l[256];
	int hstab_d[256];
	int hstab_e[256];
	
//	byte hparm_t;
//	byte hparm_l;
//	byte hparm_d;

	TkDeLz_BihState *bih_t;
	TkDeLz_BihState *bih_l;
	TkDeLz_BihState *bih_d;
	TkDeLz_BihState *bih_e;

	TkDeLz_BihState t_bih_t;
	TkDeLz_BihState t_bih_l;
	TkDeLz_BihState t_bih_d;
	TkDeLz_BihState t_bih_e;
	
	u32 magic2;

	byte *bbuf_t;
	byte *bbuf_l;
	byte *bbuf_d;
	byte *bbuf_e;

	byte *ct_t;
	byte *ct_l;
	byte *ct_d;
	byte *ct_e;

	u32 magic3;
	
	int tot_rlen;
	int tot_mlen;
	int cnt_rlen;
	int cnt_mlen;
};



int TkDeLz_HashForBytePos(TkDeLz_EncState *ctx, byte *cs)
{
	int h;

	h=cs[0];			h=(h*251)+cs[1];
	h=(h*251)+cs[2];	h=h*251+1;
	h&=(TKDELZ_HASH_SZ-1);
	return(h);
}

void TkDeLz_EncHashBytePos(TkDeLz_EncState *ctx, byte *cs)
{
	int h, r;
	
	h=TkDeLz_HashForBytePos(ctx, cs);
	r=ctx->hash_rov[h];
	r=(r-1)&(TKDELZ_HASH_LVL-1);
	ctx->hash_pos[(h*TKDELZ_HASH_LVL)+r]=cs;
	ctx->hash_rov[h]=r;
}

void TkDeLz_EncHashByteRun(TkDeLz_EncState *ctx, byte *cs, int len)
{
	int i;
	
	for(i=0; i<len; i++)
		TkDeLz_EncHashBytePos(ctx, cs+i);
}

int TkDeLz_LookupMatch(TkDeLz_EncState *ctx,
	byte *cs, byte *cse, int *rml, int *rmd)
{
	byte *ct, *cs1, *ct1, *cs1e;
	int bl, bd;
	int h, r, d;
	int i, j;

	h=TkDeLz_HashForBytePos(ctx, cs);
	r=ctx->hash_rov[h];
	
	bl=0; bd=0;
	for(i=0; i<TKDELZ_HASH_LVL; i++)
	{
		j=(r+i)&(TKDELZ_HASH_LVL-1);
		ct=ctx->hash_pos[(h*TKDELZ_HASH_LVL)+j];
		if(!ct)break;
		
		cs1=cs; ct1=ct;
		cs1e=cs+TKDELZ_MAX_MATCH;
		if(cse<cs1e)
			cs1e=cse;
		while(cs1<cs1e)
		{
			if((*cs1)!=(*ct1))
				break;
			cs1++; ct1++;
		}

		j=cs1-cs;
		d=cs-ct;

		if(d>=4096)
			if(j<5)
				j=0;
		
		if(d>=16384)
			if(j<7)
				j=0;		
		
		if((j>bl) && (d>0) && (d<TKDELZ_MAX_DIST))
			{ bl=j; bd=d; }
	}

	*rml=bl;
	*rmd=bd;
	return(bl>=3);
}

int TkDeLz_TransEncodeBlock(TkDeLz_EncState *ctx,
	byte *src, int ssz)
{
	byte *ct_t, *ct_l, *ct_d, *ct_e;

	byte *cs, *cse, *csl;
	int ml, md, ll, tg;
	int ml1, md1;
	int ml2, md2;
	int lml, lmd;
	int i0, i1, i2;
	
	cs=src; cse=src+ssz;
	csl=src;
	lml=-1; lmd=-1;
	
	ct_t=ctx->ct_t;
	ct_l=ctx->ct_l;
	ct_d=ctx->ct_d;
	ct_e=ctx->ct_e;
	
	while(cs<cse)
	{
		i0=TkDeLz_LookupMatch(ctx, cs, cse, &ml, &md);
		i1=TkDeLz_LookupMatch(ctx, cs+1, cse, &ml1, &md1);
		i2=TkDeLz_LookupMatch(ctx, cs+2, cse, &ml2, &md2);
	
//		i0=0;
		
		if(i0 && i1 && (ml1>(ml+1)))
			i0=0;
		if(i0 && i2 && (ml2>(ml+2)))
			i0=0;
	
		if(i0)
		{
			ll=cs-csl;
			
			ctx->cnt_rlen++;
			ctx->cnt_mlen++;

			ctx->tot_rlen+=ll;
			ctx->tot_mlen+=ml;
			
			tg=((ll<<5)&0xE0)|((ml-3)&0x1F);
			if(ll>=7)
				tg|=0xE0;
			if(ml>=34)
				tg|=0x1F;
				
			*ct_t++=tg;

			if(ll>=7)
			{
				if(ll<128)
				{
					*ct_d++=ll;
				}else if(ll<16384)
				{
					*ct_d++=0x80|(ll>>8);
					*ct_e++=ll&0xFF;
				}else if(ll<2097152)
				{
					*ct_d++=0xC0|(ll>>16);
					*ct_e++=(ll>>8)&0xFF;
					*ct_e++=ll&0xFF;
				}
			}

			if(ml>=34)
			{
				if(ml<128)
				{
					*ct_d++=ml;
				}else if(ml<16384)
				{
					*ct_d++=0x80|(ml>>8);
					*ct_e++=ml&0xFF;
				}else if(ml<2097152)
				{
					*ct_d++=0xC0|(ml>>16);
					*ct_e++=(ml>>8)&0xFF;
					*ct_e++=ml&0xFF;
				}
			}

			if(md<128)
			{
				*ct_d++=md;
			}else if(md<16384)
			{
				*ct_d++=0x80|(md>>8);
				*ct_e++=md&0xFF;
			}else if(md<2097152)
			{
				*ct_d++=0xC0|(md>>16);
				*ct_e++=(md>>8)&0xFF;
				*ct_e++=md&0xFF;
			}
						
			while(csl<cs)
				{ *ct_l++=*csl++; }

			TkDeLz_EncHashByteRun(ctx, cs, ml);
			cs+=ml;
			csl=cs;
			
			lml=ml;
			lmd=md;
		}else
		{
			TkDeLz_EncHashBytePos(ctx, cs);
			cs++;
		}
	}

	ll=cs-csl;

//	if(ll>=7)
	if(1)
	{
		tg=0xFF;

		*ct_t++=tg;

		if(ll<128)
		{
			*ct_d++=ll;
		}else if(ll<16384)
		{
			*ct_d++=0x80|(ll>>8);
			*ct_e++=ll&0xFF;
		}else if(ll<2097152)
		{
			*ct_d++=0xC0|(ll>>16);
			*ct_e++=(ll>>8)&0xFF;
			*ct_e++=ll&0xFF;
		}
		*ct_d++=0;
		*ct_d++=0;
	}else
	{
		tg=0x1F|(ll<<5);
		*ct_t++=tg;
		*ct_d++=0;
		*ct_d++=0;
	}

	while(csl<cs)
		{ *ct_l++=*csl++; }

	ctx->ct_t=ct_t;
	ctx->ct_l=ct_l;
	ctx->ct_d=ct_d;
	ctx->ct_e=ct_e;
	
	return(0);
}

int TkDeLz_EncodeBlock(TkDeLz_EncState *ctx,
	byte *src, int ssz)
{
	byte *ct;
	int h_tag_t, h_parm_t;
	int h_tag_l, h_parm_l;
	int h_tag_d, h_parm_d;
	int tsz, tg;
	int i;

	for(i=0; i<256; i++)
	{
		ctx->hstab_t[i]=0;
		ctx->hstab_l[i]=0;
		ctx->hstab_d[i]=0;
		ctx->hstab_e[i]=0;
	}

	ctx->ct_t=ctx->bbuf_t;
	ctx->ct_l=ctx->bbuf_l;
	ctx->ct_d=ctx->bbuf_d;
	ctx->ct_e=ctx->bbuf_e;

	TkDeLz_TransEncodeBlock(ctx, src, ssz);
	
	ct=ctx->ct;

	tkdelz_setu16le(ct, ctx->blktag);
	ct+=2;

	ct=TkDeLz_EncodeBihChunk(ctx->bih_t, ct, ctx->bbuf_t, ctx->ct_t-ctx->bbuf_t);
	ct=TkDeLz_EncodeBihChunk(ctx->bih_l, ct, ctx->bbuf_l, ctx->ct_l-ctx->bbuf_l);
	ct=TkDeLz_EncodeBihChunk(ctx->bih_d, ct, ctx->bbuf_d, ctx->ct_d-ctx->bbuf_d);
	ct=TkDeLz_EncodeBihChunk(ctx->bih_e, ct, ctx->bbuf_e, ctx->ct_e-ctx->bbuf_e);

	ctx->ct=ct;

	return(0);
}

int TkDeLz_SetupEncodeBuffer(TkDeLz_EncState *ctx,
	byte *dst, int dsz)
{
	int btag;

	if((ctx->magic1!=0x12345678) || (ctx->magic3!=0x12345678))
	{
		memset(ctx, 0, sizeof(TkDeLz_EncState));
		ctx->magic1=0x12345678;
		ctx->magic2=0x12345678;
		ctx->magic3=0x12345678;
	}

	ctx->ct=dst;
	ctx->win=0;
	ctx->pos=0;
	ctx->status=0;

	if(!ctx->hash_pos)
	{
		ctx->hash_pos=malloc(TKDELZ_HASH_SZ*TKDELZ_HASH_LVL*sizeof(byte *));
		ctx->hash_rov=malloc(TKDELZ_HASH_SZ*sizeof(byte));
	}
	
	memset(ctx->hash_pos, 0, TKDELZ_HASH_SZ*TKDELZ_HASH_LVL*sizeof(byte *));
	memset(ctx->hash_rov, 0, TKDELZ_HASH_SZ*sizeof(byte));

	ctx->bih_t=&ctx->t_bih_t;
	ctx->bih_l=&ctx->t_bih_l;
	ctx->bih_d=&ctx->t_bih_d;
	ctx->bih_e=&ctx->t_bih_e;

	btag=0x1000;
	btag|=(TKDELZ_TBLK_LG2SZ-8)<<8;
	btag|=(TKDELZ_TBLK_LG2SZ-8)<<4;
	btag|=(TKDELZ_MAX_DIST_LG2-8)<<0;
	
	ctx->blktag=btag;

	TkDeLz_InitBihContext(ctx->bih_t, ((btag>>4)&15)+8);
	TkDeLz_InitBihContext(ctx->bih_l, ((btag>>4)&15)+8);
	TkDeLz_InitBihContext(ctx->bih_d, ((btag>>4)&15)+8);
	TkDeLz_InitBihContext(ctx->bih_e, ((btag>>4)&15)+8);

	ctx->bbuf_t=ctx->bih_t->bbuf;
	ctx->bbuf_l=ctx->bih_l->bbuf;
	ctx->bbuf_d=ctx->bih_d->bbuf;
	ctx->bbuf_e=ctx->bih_e->bbuf;

	return(0);
}

int TkDeLz_EncodeBuffer(TkDeLz_EncState *ctx,
	byte *dst, byte *src, int dsz, int ssz, int lvl)
{
	byte *cs, *cse;
	int bsz;
	
	TkDeLz_SetupEncodeBuffer(ctx, dst, dsz);
	ctx->lvl=lvl;
	
	cs=src; cse=cs+ssz;
	while(cs<cse)
	{
		bsz=cse-cs;
		if(bsz>TKDELZ_TBLK_SZ)
			bsz=TKDELZ_TBLK_SZ;
		TkDeLz_EncodeBlock(ctx, cs, bsz);
		cs+=bsz;
	}

	*ctx->ct++=0x00;
	*ctx->ct++=0x00;

	return(ctx->ct-dst);
}

int TkDeLz_EncodeBufferNoCtx(
	byte *dst, byte *src, int dsz, int ssz, int lvl)
{
	static TkDeLz_EncState *ctx=NULL;
	
	if(!ctx)
	{
		ctx=malloc(sizeof(TkDeLz_EncState));
		memset(ctx, 0, sizeof(TkDeLz_EncState));
	}
	return(TkDeLz_EncodeBuffer(ctx, dst, src, dsz, ssz, lvl));
}
