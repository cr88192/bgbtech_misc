typedef struct BtFLZH_DecodeCtx_s BtFLZH_DecodeCtx;
struct BtFLZH_DecodeCtx_s {
byte *bits_ptr;
// u32 bits_win;
byte bits_pos;
byte tag_bits;
byte lit_bits;
byte dist_bits;

byte *obuf_ptr;

u32 htab_lt[BTFLZH_HTAB_LUSZ];	//Lookup T
u32 htab_ll[BTFLZH_HTAB_LUSZ];	//Lookup L
u32 htab_ld[BTFLZH_HTAB_LUSZ];	//Lookup D

u32 htab_st[256];	//Search T
u32 htab_sl[256];	//Search L
u32 htab_sd[256];	//Search D
};

force_inline int BtFLZH_ReadNBits(BtFLZH_DecodeCtx *ctx, int n)
{
	int v, p;
	p=ctx->bits_pos;
	v=(*(u32 *)(ctx->bits_ptr))>>p;
	p+=n;
	ctx->bits_ptr+=p>>3;
	ctx->bits_pos=p&7;
//	return(v&btflzh_pow2masktab[n]);
	return(BTFLZH_BITMASK(v, n));
}

force_inline int BtFLZH_ReadNBitsB(BtFLZH_DecodeCtx *ctx, int n)
{
	int v, p;
	p=ctx->bits_pos;
	v=(*(u32 *)(ctx->bits_ptr))>>p;
	p+=n;
	ctx->bits_ptr+=p>>3;
	ctx->bits_pos=p&7;
	return(v);
}

force_inline u32 BtFLZH_Read32Bits(BtFLZH_DecodeCtx *ctx)
{
	u32 v;
	int p;
	p=ctx->bits_pos;
	v=(*(u64 *)(ctx->bits_ptr))>>p;
	ctx->bits_ptr+=4;
	return(v);
}

force_inline u64 BtFLZH_Read56Bits(BtFLZH_DecodeCtx *ctx)
{
	u64 v;
	int p;
	p=ctx->bits_pos;
	v=(*(u64 *)(ctx->bits_ptr))>>p;
	ctx->bits_ptr+=7;
	return(v);
}

force_inline void BtFLZH_SkipNBits(BtFLZH_DecodeCtx *ctx, int n)
{
	int p;
	p=ctx->bits_pos+n;
	ctx->bits_ptr+=p>>3;
	ctx->bits_pos=p&7;
}

force_inline int BtFLZH_PeekBits(BtFLZH_DecodeCtx *ctx)
{
	int v, p;
	p=ctx->bits_pos;
	v=(*(u32 *)(ctx->bits_ptr))>>p;
	return(v);
}

force_inline u64 BtFLZH_PeekBits56(BtFLZH_DecodeCtx *ctx)
{
	u64 v;
	int p;
	p=ctx->bits_pos;
	v=(*(u64 *)(ctx->bits_ptr))>>p;
	return(v);
}

force_inline int BtFLZH_DecodeHuffTag(BtFLZH_DecodeCtx *ctx)
{
	u32 b, p, m;
	int i, j;
	
	b=BtFLZH_PeekBits(ctx);
	p=ctx->htab_lt[b&BTFLZH_HTAB_LUMASK];
	if(p&0x000F0000)
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(p&255);
	}
	
#if 1
	i=p&255;
	p=ctx->htab_st[i];
//	m=btflzh_pow2masktab[(p>>16)&15];
	m=BTFLZH_GENMASK((p>>16)&15);
	if(!((p^b)&m))
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(i);
	}
	i=p>>24;
#endif

//	i=p&255;
	while(1)
	{
		p=ctx->htab_st[i];
//		m=btflzh_pow2masktab[(p>>16)&15];
		m=BTFLZH_GENMASK((p>>16)&15);
		if(!((p^b)&m))
		{
			BtFLZH_SkipNBits(ctx, (p>>16)&15);
			return(i);
		}
		i=p>>24;
	}

	return(-1);
}

force_inline int BtFLZH_DecodeHuffLiteral(BtFLZH_DecodeCtx *ctx)
{
	u32 b, p, m;
	int i, j;
	
	b=BtFLZH_PeekBits(ctx);
	p=ctx->htab_ll[b&BTFLZH_HTAB_LUMASK];
	if(p&0x000F0000)
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(p&255);
	}
	
#if 1
	i=p&255;
	p=ctx->htab_sl[i];
//	m=btflzh_pow2masktab[(p>>16)&15];
	m=BTFLZH_GENMASK((p>>16)&15);
	if(!((p^b)&m))
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(i);
	}
	i=p>>24;
#endif

//	i=p&255;
	while(1)
	{
		p=ctx->htab_sl[i];
//		m=btflzh_pow2masktab[(p>>16)&15];
		m=BTFLZH_GENMASK((p>>16)&15);
		if(!((p^b)&m))
		{
			BtFLZH_SkipNBits(ctx, (p>>16)&15);
			return(i);
		}
		i=p>>24;
	}

	return(-1);
}

force_inline u32 BtFLZH_DecodeHuffLiteral2(BtFLZH_DecodeCtx *ctx)
{
	u32 b;
	u32 v, p;
	int s0, s1, s2, s3;
	int l0, l1, l2, l3;
	int l, ll;

#if 1
	b=BtFLZH_PeekBits(ctx);

	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
	if(!(p&0x000F0000))
	{
		s0=BtFLZH_DecodeHuffLiteral(ctx);
		s1=BtFLZH_DecodeHuffLiteral(ctx);
		v=s0|(s1<<8);
		return(v);
	}
	ll=(p>>16)&15;	s0=(p&255);		b>>=ll;

	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
	if(!(p&0x000F0000))
	{
		BtFLZH_SkipNBits(ctx, ll);
		s1=BtFLZH_DecodeHuffLiteral(ctx);
		v=s0|(s1<<8);
		return(v);
	}
	l=(p>>16)&15;	s1=(p&255);		ll+=l;	b>>=l;

	BtFLZH_SkipNBits(ctx, ll);
	v=s0|(s1<<8);
	return(v);
#endif

#if 0
	s0=BtFLZH_DecodeHuffLiteral(ctx);
	s1=BtFLZH_DecodeHuffLiteral(ctx);
	v=s0|(s1<<8);
	return(v);
#endif
}

force_inline u32 BtFLZH_DecodeHuffLiteral4(BtFLZH_DecodeCtx *ctx)
{
	u64 b;
	u32 v, p;
	int s0, s1, s2, s3, s01, s23;
	int l0, l1, l2, l3;
	int l, ll;

#if 1
	b=BtFLZH_PeekBits56(ctx);

	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
	if(p&0x000F0000)
	{
		ll=(p>>16)&15;	s0=(p&255);		b>>=ll;
		p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
		if(p&0x000F0000)
		{
			l=(p>>16)&15;	s1=(p&255);		ll+=l;	b>>=l;
			s01=s0|(s1<<8);
			p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
			if(p&0x000F0000)
			{
				l=(p>>16)&15;	s2=(p&255);		ll+=l;	b>>=l;
				p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
				if(p&0x000F0000)
				{
					l=(p>>16)&15;	s3=(p&255);		ll+=l;
					s23=s2|(s3<<8);
					BtFLZH_SkipNBits(ctx, ll);
//					v=s0|(s1<<8)|(s2<<16)|(s3<<24);
					v=s01|(s23<<16);
					return(v);
				}else
				{
					BtFLZH_SkipNBits(ctx, ll);
					s3=BtFLZH_DecodeHuffLiteral(ctx);
//					v=s0|(s1<<8)|(s2<<16)|(s3<<24);
					v=s01|(s2<<16)|(s3<<24);
					return(v);
				}
			}else
			{
				BtFLZH_SkipNBits(ctx, ll);
				s2=BtFLZH_DecodeHuffLiteral(ctx);
				s3=BtFLZH_DecodeHuffLiteral(ctx);
				v=s0|(s1<<8)|(s2<<16)|(s3<<24);
				return(v);
			}
		}else
		{
			BtFLZH_SkipNBits(ctx, ll);
			s1=BtFLZH_DecodeHuffLiteral(ctx);
//			s2=BtFLZH_DecodeHuffLiteral(ctx);
//			s3=BtFLZH_DecodeHuffLiteral(ctx);
//			v=s0|(s1<<8)|(s2<<16)|(s3<<24);
			s23=BtFLZH_DecodeHuffLiteral2(ctx);
			v=s0|(s1<<8)|(s23<<16);
			return(v);
		}
	}else
	{
		s0=BtFLZH_DecodeHuffLiteral(ctx);
		s1=BtFLZH_DecodeHuffLiteral(ctx);
//		s2=BtFLZH_DecodeHuffLiteral(ctx);
//		s3=BtFLZH_DecodeHuffLiteral(ctx);
//		v=s0|(s1<<8)|(s2<<16)|(s3<<24);
		s2=BtFLZH_DecodeHuffLiteral2(ctx);
		v=s0|(s1<<8)|(s2<<16);
		return(v);
	}
#endif

#if 0
	b=BtFLZH_PeekBits56(ctx);

	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
	if(!(p&0x000F0000)) goto fail0;
	ll=(p>>16)&15;	s0=(p&255);		b>>=ll;

	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
	if(!(p&0x000F0000)) goto fail1;
	l=(p>>16)&15;	s1=(p&255);		ll+=l;	b>>=l;

	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
	if(!(p&0x000F0000)) goto fail2;
	l=(p>>16)&15;	s2=(p&255);		ll+=l;	b>>=l;

	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
	if(!(p&0x000F0000)) goto fail3;
	l=(p>>16)&15;	s3=(p&255);		ll+=l;

	BtFLZH_SkipNBits(ctx, ll);
	v=s0|(s1<<8)|(s2<<16)|(s3<<24);
	return(v);

	fail0:
	s0=BtFLZH_DecodeHuffLiteral(ctx);
	s1=BtFLZH_DecodeHuffLiteral(ctx);
	s2=BtFLZH_DecodeHuffLiteral(ctx);
	s3=BtFLZH_DecodeHuffLiteral(ctx);
	v=s0|(s1<<8)|(s2<<16)|(s3<<24);
	return(v);

	fail1:
//	BtFLZH_SkipNBits(ctx, l0);
	BtFLZH_SkipNBits(ctx, ll);
	s1=BtFLZH_DecodeHuffLiteral(ctx);
	s2=BtFLZH_DecodeHuffLiteral(ctx);
	s3=BtFLZH_DecodeHuffLiteral(ctx);
	v=s0|(s1<<8)|(s2<<16)|(s3<<24);
//	s2=BtFLZH_DecodeHuffLiteral2(ctx);
//	v=s0|(s1<<8)|(s2<<16);
	return(v);

	fail2:
//	BtFLZH_SkipNBits(ctx, l0+l1);
	BtFLZH_SkipNBits(ctx, ll);
	s2=BtFLZH_DecodeHuffLiteral(ctx);
	s3=BtFLZH_DecodeHuffLiteral(ctx);
	v=s0|(s1<<8)|(s2<<16)|(s3<<24);
	return(v);

	fail3:
//	BtFLZH_SkipNBits(ctx, l0+l1+l2);
	BtFLZH_SkipNBits(ctx, ll);
	s3=BtFLZH_DecodeHuffLiteral(ctx);
	v=s0|(s1<<8)|(s2<<16)|(s3<<24);
	return(v);
#endif

#if 0
//	u32 v;
//	int l0, l1, l2, l3;

	s0=BtFLZH_DecodeHuffLiteral(ctx);
	s1=BtFLZH_DecodeHuffLiteral(ctx);
	s2=BtFLZH_DecodeHuffLiteral(ctx);
	s3=BtFLZH_DecodeHuffLiteral(ctx);
	v=s0|(s1<<8)|(s2<<16)|(s3<<24);
	return(v);
#endif
}

force_inline int BtFLZH_DecodeHuffDist(BtFLZH_DecodeCtx *ctx)
{
	u32 b, p, m;
	int i, j;
	
	b=BtFLZH_PeekBits(ctx);
	p=ctx->htab_ld[b&BTFLZH_HTAB_LUMASK];
	if(p&0x000F0000)
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(p&255);
	}
	
#if 1
	i=p&255;
	p=ctx->htab_sd[i];
//	m=btflzh_pow2masktab[(p>>16)&15];
	m=BTFLZH_GENMASK((p>>16)&15);
	if(!((p^b)&m))
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(i);
	}
	i=p>>24;
#endif

//	i=p&255;
	while(1)
	{
		p=ctx->htab_sd[i];
//		m=btflzh_pow2masktab[(p>>16)&15];
		m=BTFLZH_GENMASK((p>>16)&15);
		if(!((p^b)&m))
		{
			BtFLZH_SkipNBits(ctx, (p>>16)&15);
			return(i);
		}
		i=p>>24;
//		i=j;
	}

	return(-1);
}

force_inline int BtFLZH_DecodeRiceQ(BtFLZH_DecodeCtx *ctx)
{
	int b, l, n;

	b=BtFLZH_PeekBits(ctx);	
	n=0;
	while((b&255)==255)
	{
		BtFLZH_SkipNBits(ctx, 8);
		n+=8;
	}
	l=btflzh_rqtab[b&255];
	BtFLZH_SkipNBits(ctx, l+1);
	return(n+l);
}

force_inline int BtFLZH_DecodeRice(BtFLZH_DecodeCtx *ctx, int rk)
{
	int q, f, v;
	q=BtFLZH_DecodeRiceQ(ctx);
	f=BtFLZH_ReadNBits(ctx, rk);
	v=(q<<rk)|f;
	return(v);
}

force_inline int BtFLZH_DecodeDistance(BtFLZH_DecodeCtx *ctx)
{
	u64 b;
	u32 p, m;
	fastint tg, nd, ndb, nde, nl;
//	int i, j;
	
	/* Conjoined Path */
	b=BtFLZH_PeekBits56(ctx);
	p=ctx->htab_ld[((u32)b)&BTFLZH_HTAB_LUMASK];
	if(p&0x000F0000)
	{
		tg=p&255;
		nl=(p>>16)&15;
		nde=btflzh_distxtra[tg];
		ndb=btflzh_distbase[tg];
//		if((nl+nde)<=24)
		if(1)
		{
			nd=ndb+BTFLZH_BITMASK(b>>nl, nde);
			BtFLZH_SkipNBits(ctx, nl+nde);
			return(nd);
		}
	}

	/* Fallback */
	tg=BtFLZH_DecodeHuffDist(ctx);
	nd=btflzh_distbase[tg]+
		BtFLZH_ReadNBits(ctx, btflzh_distxtra[tg]);
	return(nd);

#if 0
	int tg, nd;
	tg=BtFLZH_DecodeHuffDist(ctx);
	nd=btflzh_distbase[tg]+
		BtFLZH_ReadNBits(ctx, btflzh_distxtra[tg]);
	return(nd);
#endif
}

#ifdef USE_MOV_QWORD
#define BTFLZH_COPYMATCH_MOV16 \
			lpx=((u64 *)cs)[0];		lpy=((u64 *)cs)[1]; \
			((u64 *)ct)[0]=lpx;		((u64 *)ct)[1]=lpy; \
			cs+=16; ct+=16;
#else
#define BTFLZH_COPYMATCH_MOV16 \
			px=((u32 *)cs)[0];	py=((u32 *)cs)[1];	\
			pz=((u32 *)cs)[2];	pw=((u32 *)cs)[3];	\
			((u32 *)ct)[0]=px;	((u32 *)ct)[1]=py;	\
			((u32 *)ct)[2]=pz;	((u32 *)ct)[3]=pw;	\
			cs+=16; ct+=16;
#endif

void BtFLZH_CopyMatch(byte *cto, int nl, int nd)
{
	byte *cs, *ct, *cte;
#ifdef USE_MOV_QWORD
	u64 lpx, lpy;
#endif
	u32 px, py, pz, pw;
	
	cs=cto-nd;
	ct=cto;
	cte=ct+nl;

//	while(ct<cte)
//		*ct++=*cs++;

#if 1
	if(nd<4)
	{
		if(nd==1)
		{
			px=*cs;
			px=px|(px<<8)|(px<<16)|(px<<24);
			while(ct<cte)
			{
				((u32 *)ct)[0]=px;	((u32 *)ct)[1]=px;
				((u32 *)ct)[2]=px;	((u32 *)ct)[3]=px;
				ct+=16;
			}
			return;
		}

		if(nd==2)
		{
			px=*(u16 *)cs;
			px=px|(px<<16);
			while(ct<cte)
			{
				((u32 *)ct)[0]=px;	((u32 *)ct)[1]=px;
				((u32 *)ct)[2]=px;	((u32 *)ct)[3]=px;
				ct+=16;
			}
			return;
		}

		if(nd==3)
		{
			pw=*(u32 *)cs;
			pw=pw&0x00FFFFFF;
			px=(pw<<24)|(pw    );
			py=(pw<<16)|(pw>> 8);
			pz=(pw<< 8)|(pw>>16);
			while(ct<cte)
			{
				((u32 *)ct)[0]=px;
				((u32 *)ct)[1]=py;
				((u32 *)ct)[2]=pz;
				ct+=12;
			}
			return;
		}
		
		__debugbreak();

#if 0
		px=*(u32 *)cs;
		while(ct<cte)
			{ *(u32 *)ct=px; ct+=nd; }
		return;
#endif
	}else if(nd<16)
	{
#if 1
		while(ct<cte)
		{
			px=*(u32 *)cs;
			*(u32 *)ct=px;
			cs+=4; ct+=4;
		}
		return;
#endif
	}else
	{
//		if((ct+128)>=cte)
		if(0)
		{
			BTFLZH_COPYMATCH_MOV16
			if(ct>=cte)return;
			BTFLZH_COPYMATCH_MOV16
			if(ct>=cte)return;
			BTFLZH_COPYMATCH_MOV16
			if(ct>=cte)return;
			BTFLZH_COPYMATCH_MOV16
			if(ct>=cte)return;

			BTFLZH_COPYMATCH_MOV16
			if(ct>=cte)return;
			BTFLZH_COPYMATCH_MOV16
			if(ct>=cte)return;
			BTFLZH_COPYMATCH_MOV16
			if(ct>=cte)return;
			BTFLZH_COPYMATCH_MOV16
			return;
		}else
		{
#if 0
			while((ct+64)<=cte)
			{
				BTFLZH_COPYMATCH_MOV16
				BTFLZH_COPYMATCH_MOV16
				BTFLZH_COPYMATCH_MOV16
				BTFLZH_COPYMATCH_MOV16
			}
#endif

			while(ct<cte)
			{
				BTFLZH_COPYMATCH_MOV16

#if 0
#ifdef USE_MOV_QWORD
				lpx=((u64 *)cs)[0];		lpy=((u64 *)cs)[1];
				((u64 *)ct)[0]=lpx;		((u64 *)ct)[1]=lpy;
				cs+=16; ct+=16;
#else
				px=((u32 *)cs)[0];	py=((u32 *)cs)[1];
				pz=((u32 *)cs)[2];	pw=((u32 *)cs)[3];
				((u32 *)ct)[0]=px;	((u32 *)ct)[1]=py;
				((u32 *)ct)[2]=pz;	((u32 *)ct)[3]=pw;
				cs+=16; ct+=16;
#endif
		//		cs+=8; ct+=8;
#endif
			}
		}
	}
#endif
}

void BtFLZH_DecodeRawLits(BtFLZH_DecodeCtx *ctx, byte *cto, int nl)
{
	byte *ct, *cte;
	u64 lpx;
	u32 px, py, pz, pw;
	int i, j;

//	if(!nl)
//		return;

	if(ctx->lit_bits==8)
	{
		ct=cto; cte=ct+nl;

		while((ct+7)<=cte)
		{
			lpx=BtFLZH_Read56Bits(ctx);
			*(u64 *)ct=lpx;
			ct+=7;
		}

		while((ct+4)<=cte)
		{
			px=BtFLZH_Read32Bits(ctx);
			*(u32 *)ct=px;
			ct+=4;
		}

		if(ct<cte)
		{
			j=cte-ct;
			px=BtFLZH_ReadNBitsB(ctx, j<<3);
			*(u32 *)ct=px;
			ct+=j;
		}

#if 0
		while(ct<cte)
		{
			j=BtFLZH_ReadNBitsB(ctx, 8);
//			j=btflzh_trans8tab[j];
			*ct++=j;
		}
#endif

		return;
	}
	if(ctx->lit_bits>0)
	{

		if(ctx->lit_bits==7)
		{
			ct=cto; cte=ct+nl;

			while((ct+2)<=cte)
			{
				j=BtFLZH_ReadNBits(ctx, 14);
				j=((j<<1)&0x7F00)|(j&0x007F);
				*(u16 *)ct=j;
				ct+=2;
	//			*ct++=j;
			}

			while(ct<cte)
			{
				j=BtFLZH_ReadNBits(ctx, 7);
				*ct++=j;
			}
			return;
		}else
		{
			ct=cto; cte=ct+nl;
			while(ct<cte)
			{
				j=BtFLZH_ReadNBits(ctx, ctx->lit_bits);
				*ct++=j;
			}
			return;
		}
	}

	ct=cto; cte=ct+nl;

#if 0
	while((ct+4)<=cte)
	{
		px=BtFLZH_DecodeHuffLiteral4(ctx);
		*(u32 *)ct=px;
		ct+=4;
	}
#endif

#if 1
	while((ct+8)<=cte)
	{
		((u32 *)ct)[0]=BtFLZH_DecodeHuffLiteral4(ctx);
		((u32 *)ct)[1]=BtFLZH_DecodeHuffLiteral4(ctx);
		ct+=8;
	}

	if((ct+4)<=cte)
	{
		px=BtFLZH_DecodeHuffLiteral4(ctx);
		*(u32 *)ct=px;
		ct+=4;
	}
#endif

	if((ct+2)<=cte)
	{
		px=BtFLZH_DecodeHuffLiteral2(ctx);
		*(u16 *)ct=px;
		ct+=2;
	}
	if(ct<cte)
		{ *ct++=BtFLZH_DecodeHuffLiteral(ctx); }

//	while(ct<cte)
//		{ *ct++=BtFLZH_DecodeHuffLiteral(ctx); }
}

int BtFLZH_DecodePayload(BtFLZH_DecodeCtx *ctx)
{
	byte *ct, *cti;
	fastint tg, tgr, tgl;
	fastint lnl, lnd;
	fastint nrb, nlb, nre, nle;
	fastint nr, nl, nd;
	byte tgnb;
	int i;
	
	tgnb=ctx->tag_bits;
	ct=ctx->obuf_ptr;
	cti=ct;
	while(1)
	{
//		if(tgnb)
//			tg=BtFLZH_ReadNBits(ctx, tgnb);
//		else
			tg=BtFLZH_DecodeHuffTag(ctx);
#ifdef BTFLZH_LEN44
		tgr=(tg>>4)&15;
		tgl=tg&15;
#else
		tgl=tg&31;
		tgr=(tg>>5)&7;
#endif
		if(tgl>2)
		{
#if 0
			nr=btflzh_rawbase[tgr]+
				BtFLZH_ReadNBits(ctx, btflzh_rawxtra[tgr]);
			nl=btflzh_lenbase[tgl]+
				BtFLZH_ReadNBits(ctx, btflzh_lenxtra[tgl]);
#endif

#if 1

#ifdef BTFLZH_LEN44
			nrb=btflzh_len2base[tgr];	nlb=btflzh_len2base[tgl];
			nre=btflzh_len2xtra[tgr];	nle=btflzh_len2xtra[tgl];
#else
			nrb=btflzh_rawbase[tgr];	nlb=btflzh_lenbase[tgl];
			nre=btflzh_rawxtra[tgr];	nle=btflzh_lenxtra[tgl];
#endif
			i=BtFLZH_ReadNBitsB(ctx, nre+nle);
//			i=BtFLZH_ReadNBits(ctx, nre+nle);
//			i=BtFLZH_PeekBits(ctx);
//			nr=nrb+((i     )&btflzh_pow2masktab[nre]);
//			nl=nlb+((i>>nre)&btflzh_pow2masktab[nle]);
			nr=nrb+BTFLZH_BITMASK((i     ), nre);
			nl=nlb+BTFLZH_BITMASK((i>>nre), nle);
//			nl=nlb+(i>>nre);
//			BtFLZH_SkipNBits(ctx, nre+nle);
#endif

			nd=BtFLZH_DecodeDistance(ctx);
			if(!nd)nd=lnd;
			lnl=nl;
			lnd=nd;
			
//			i=nr;
//			while(i--)
//				{ *ct++=BtFLZH_DecodeHuffLiteral(ctx); }
			if(nr)
			{
				BtFLZH_DecodeRawLits(ctx, ct, nr);
				ct+=nr;
			}
			BtFLZH_CopyMatch(ct, nl, nd);
			ct+=nl;
			continue;
		}else if(tgl==1)
		{
#ifdef BTFLZH_LEN44
			nr=btflzh_len2base[tgr]+
				BtFLZH_ReadNBits(ctx, btflzh_len2xtra[tgr]);
#else
			nr=btflzh_rawbase[tgr]+
				BtFLZH_ReadNBits(ctx, btflzh_rawxtra[tgr]);
#endif
			nd=BtFLZH_DecodeDistance(ctx);
			if(!nd)nd=lnd;
			lnd=nd;
//			i=nr;
//			while(i--)
//				{ *ct++=BtFLZH_DecodeHuffLiteral(ctx); }
			if(nr)
			{
				BtFLZH_DecodeRawLits(ctx, ct, nr);
				ct+=nr;
			}
			BtFLZH_CopyMatch(ct, lnl, lnd);
			ct+=lnl;
			continue;
		}else if(tgl==2)
		{
#ifdef BTFLZH_LEN44
			nr=btflzh_len2base[tgr]+
				BtFLZH_ReadNBits(ctx, btflzh_len2xtra[tgr]);
#else
			nr=btflzh_rawbase[tgr]+
				BtFLZH_ReadNBits(ctx, btflzh_rawxtra[tgr]);
#endif
//			i=nr;
//			while(i--)
//				{ *ct++=BtFLZH_DecodeHuffLiteral(ctx); }
			if(nr)
			{
				BtFLZH_DecodeRawLits(ctx, ct, nr);
				ct+=nr;
			}
			BtFLZH_CopyMatch(ct, lnl, lnd);
			ct+=lnl;
			continue;
		}else
		{
			nr=BtFLZH_DecodeDistance(ctx);
			nl=BtFLZH_DecodeDistance(ctx);
			if(nl!=2)
				nd=BtFLZH_DecodeDistance(ctx);
			else
				nd=0;
			if(!(nr|nl|nd))	break;
			if(nl|nd)
			{
//				if(!nl)nl=lnl;
				if(nl<3)nl=lnl;
				if(!nd)nd=lnd;
				lnl=nl;
				lnd=nd;
			}
//			i=nr;
//			while(i--)
//				{ *ct++=BtFLZH_DecodeHuffLiteral(ctx); }
			if(nr)
			{
				BtFLZH_DecodeRawLits(ctx, ct, nr);
				ct+=nr;
			}
			if(nd)
			{
				BtFLZH_CopyMatch(ct, nl, nd);
				ct+=nl;
			}else
			{
				break;
			}
			continue;
		}
	}
	ctx->obuf_ptr=ct;
	return(ct-cti);
}

int BtFLZH_DecTransposeByte(int val)
{
	return(btflzh_trans8tab[val&255]);
}

int BtFLZH_DecTransposeWord(int val)
{
	return((btflzh_trans8tab[val&255]<<8) |
		btflzh_trans8tab[(val>>8)&255]);
}

#if (BTFLZH_HTAB_LUNB==8)
void BtFLZH_DecInitTransposeLuNb(void)
{
}

int BtFLZH_DecTransposeLuNb(int val)
{
	return(btflzh_trans8tab[val&255]);
}
#else

static u16 btflzh_transnbtab[BTFLZH_HTAB_LUSZ];
static byte btflzh_transnbtab_init=0;

void BtFLZH_DecInitTransposeLuNb(void)
{
	int i, j;
	if(btflzh_transnbtab_init)
		return;
	btflzh_transnbtab_init=1;
	
	for(i=0; i<BTFLZH_HTAB_LUSZ; i++)
	{
		j=BtFLZH_DecTransposeWord(i<<(16-BTFLZH_HTAB_LUNB));
		btflzh_transnbtab[i]=j;
	}
}

int BtFLZH_DecTransposeLuNb(int val)
{
	return(btflzh_transnbtab[val&BTFLZH_HTAB_LUMASK]);
}

#if 0
int BtFLZH_DecTransposeLuNb(int val)
{
	if(BTFLZH_HTAB_LUNB==8)
	{
		return(btflzh_trans8tab[val&255]);
	}else
	{
		return(
			BtFLZH_DecTransposeWord(
				val<<(16-BTFLZH_HTAB_LUNB))&
				BTFLZH_HTAB_LUMASK);
	}
}
#endif
#endif

int BtFLZH_SetupHuffLengths(BtFLZH_DecodeCtx *ctx, byte *clbuf,
	u32 *ltab, u32 *stab)
{
	u16 tc[256];	//symbol bit-code
//	u16 tm[256];
	byte tl[256];	//symbol length
	u16 tn[256];	//symbol next
	u32 ti[BTFLZH_HTAB_LUSZ];	//symbol index
	
	int cnt[16], nc[16], nn[16], nf[16];
	int i, j, k, k2, l, ff;
	int ncl;
	
	ncl=256;

	BtFLZH_DecInitTransposeLuNb();

	for(i=0; i<16; i++) { cnt[i]=0; nc[i]=0; nn[i]=-1; nf[i]=-1; }
	for(i=0; i<BTFLZH_HTAB_LUSZ; i++)ti[i]=0xFFFF;

	for(i=0; i<ncl; i++)cnt[clbuf[i]]++;
	cnt[0]=0;

#if 1
	j=clbuf[0];
	for(i=15; i>=1; i--)
		if(cnt[i])break;
	if(j>i)j=i;
	if(!i)
	{
		printf("BtFLZH_SetupHuffLengths: Empty Table\n");
		return(-12);	//no codes
	}

	j=1;
	for(i=1; i<16; i++)
	{
		j<<=1;
		j-=cnt[i];

		if(j<0)
		{
			printf("BtFLZH_SetupHuffLengths: Over Subscribed\n");
			return(-10);	//over subscribed
		}
	}
	if((j>0) && ((ncl-cnt[0])!=1))
	{
		printf("BtFLZH_SetupHuffLengths: Incomplete Set\n");
		return(-11);	//incomplete set
	}
#endif

	j=0;
	for(i=1; i<16; i++)
	{
		j=(j+cnt[i-1])<<1;
		nc[i]=j;
	}

	for(i=0; i<ncl; i++)
	{
		l=clbuf[i];
		tl[i]=l;
		if(!l)continue;

//		tm[i]=(1<<l)-1;
		tc[i]=nc[l]++;

		if(nn[l]>=0)
		{
			tn[nn[l]]=i;
			nn[l]=i;
		}else
		{
			nf[l]=i;
			nn[l]=i;
		}
	}

	j=-1; ff=-1;
	for(i=1; i<16; i++)
	{
		if(nf[i]>=0)
		{
			if(ff<0)ff=nf[i];
			if(j>=0)tn[j]=nf[i];
			j=nn[i];
		}
	}
	if(j>=0)tn[j]=0xFFFF;

	for(i=ff; i!=0xFFFF; i=tn[i])
	{
		l=clbuf[i];
		if(l<=BTFLZH_HTAB_LUNB)
		{
			k2=tc[i]<<(BTFLZH_HTAB_LUNB-l);
			j=1<<(BTFLZH_HTAB_LUNB-l);
			while(j--)
			{
//				k=BtFLZH_DecTransposeByte(k2++);
				k=BtFLZH_DecTransposeLuNb(k2++);
				ti[k]=i;
			}
		}else
		{
			k2=tc[i]>>(l-BTFLZH_HTAB_LUNB);
//			k=BtFLZH_DecTransposeByte(k2);
			k=BtFLZH_DecTransposeLuNb(k2);
			if(ti[k]==0xFFFF)
				ti[k]=i;
		}

		k2=(tc[i])<<(16-l);
		k=BtFLZH_DecTransposeWord(k2++);
		tc[i]=k;
	}

#if 1
	for(i=0; i<BTFLZH_HTAB_LUSZ; i++)
		if(ti[i]==0xFFFF)
	{
		printf("BtFLZH_SetupHuffLengths: Table bad index %d\n", i);
		return(-9);
	}
#endif

	for(i=0; i<256; i++)
	{
		j=tn[i];
		k=tc[i];
		l=tl[i];
		stab[i]=k | ((l&15)<<16) | ((j&255)<<24);
	}

	for(i=0; i<BTFLZH_HTAB_LUSZ; i++)
	{
		j=ti[i];
		l=tl[j];
		if(l<=BTFLZH_HTAB_LUNB)
			{ ti[i]|=(l&15)<<16; }
		ltab[i]=ti[i];
	}

	return(0);
}

int BtFLZH_DecodeHuffLengths(BtFLZH_DecodeCtx *ctx, byte *obuf)
{
	byte stf[32];
	int i, j, k, l, j0, k0, j1, k1, ll, n;

//	memset(obuf, 0, 256);
	memcpy(stf, btflzh_initstf, 32);

	ll=0; i=0;
	while(i<256)
	{
#if 1
		j0=BtFLZH_DecodeRice(ctx, BTFLZH_STF_RK);
		if(j0>>5)
			__debugbreak();
		k0=stf[j0];
//		j1=(j0*3)/4;
//		j1=(j0*13)/16;
		j1=BTFLZH_STFSWAP(j0);
		k1=stf[j1];
		stf[j0]=k1;
		stf[j1]=k0;
		
		l=k0;
#endif

//		l=BtFLZH_ReadNBits(ctx, 5);
		
		if(l<16)
		{
			ll=l;
			obuf[i++]=l;
			continue;
		}
		
		if(l==19)
			break;
		
		switch(l)
		{
		case 16:
			n=BtFLZH_ReadNBits(ctx, 3)+2;
			while((n--) && (i<256))
				obuf[i++]=ll;
			break;
		case 17:
			n=BtFLZH_ReadNBits(ctx, 5)+10;
			while((n--) && (i<256))
				obuf[i++]=ll;
			break;
		case 18:
			n=BtFLZH_ReadNBits(ctx, 3)+2;
			while((n--) && (i<256))
				obuf[i++]=0;
			break;
		default:
			__debugbreak();
		}
	}

	while(i<256)
		{ obuf[i++]=0; }
	
	return(0);
}

int BtFLZH_DecodeFixedLengths(BtFLZH_DecodeCtx *ctx, byte *clbuf, int ti)
{
	int i;

	for(i=0; i<256; i++)	clbuf[i]=0;
	switch(ti)
	{
	case 0:
		for(i=0; i<256; i++)	clbuf[i]=8;
		break;
	case 1:
		for(i=0; i<128; i++)	clbuf[i]=7;
		break;
	case 2:
		for(i=0; i< 64; i++)	clbuf[i]=6;
		break;
	case 3:
		for(i=0; i< 32; i++)	clbuf[i]=5;
		break;
	case 4:
		for(i=0; i< 16; i++)	clbuf[i]=4;
		break;
	case 5:
		for(i=0; i<  8; i++)	clbuf[i]=3;
		break;
	case 6:
		for(i=0; i<  4; i++)	clbuf[i]=2;
		break;
	case 7:
		for(i=0; i<  4; i++)	clbuf[i]=1;
		break;
	default:
		__debugbreak();
	}
	return(0);
}

int BtFLZH_DecodeHuffTable(BtFLZH_DecodeCtx *ctx)
{
	byte clbuf[256];
	u32 *ltab, *stab;
	int tn, tf, ti, nb;
	int i;

	tn=BtFLZH_ReadNBits(ctx, 2);
	tf=BtFLZH_ReadNBits(ctx, 2);

//	printf("BtFLZH_DecodeHuffTable %d %d\n", tn, tf);

	switch(tn)
	{
	case 1:
		ltab=ctx->htab_lt;
		stab=ctx->htab_st;
		ctx->tag_bits=0;
		break;
	case 2:
		ltab=ctx->htab_ll;
		stab=ctx->htab_sl;
		ctx->lit_bits=0;
		break;
	case 3:
		ltab=ctx->htab_ld;
		stab=ctx->htab_sd;
		ctx->dist_bits=0;
		break;
	}

	if(tf==0)
	{
		ti=BtFLZH_ReadNBits(ctx, 6);
		
		nb=8-ti;
		if(nb<0)nb=0;

		if(nb==0)
		{
			BtFLZH_DecodeFixedLengths(ctx, clbuf, ti);
			BtFLZH_SetupHuffLengths(ctx, clbuf, ltab, stab);
		}else
		{
#if 0
			for(i=0; i<256; i++)
			{
				ltab[i]=i|(8<<16);
				stab[i]=i|(8<<16);
			}
#endif

#if 1
			for(i=0; i<BTFLZH_HTAB_LUSZ; i++)
				{ ltab[i]=(i&((1<<nb)-1))|(nb<<16); }
			for(i=0; i<256; i++)
				{ stab[i]=0; }
			for(i=0; i<(1<<nb); i++)
				stab[i]=i|(nb<<16);
#endif
		}
		
		if(tn==1)
			{ ctx->tag_bits=nb; }
		if(tn==2)
			{ ctx->lit_bits=nb; }
		if(tn==3)
			{ ctx->dist_bits=nb; }
		
	}else if(tf==1)
	{
		BtFLZH_DecodeHuffLengths(ctx, clbuf);
		BtFLZH_SetupHuffLengths(ctx, clbuf, ltab, stab);
	}else
	{
		__debugbreak();
	}
	return(0);
}

int BtFLZH_DecodeStream(BtFLZH_DecodeCtx *ctx,
	byte *ibuf, int ibsz,
	byte *obuf, int obsz)
{
	byte *ibufe, *obufe;
	int tg, bn;

	bn=1;
	ctx->bits_ptr=ibuf;
	ctx->bits_pos=0;
	
	ctx->obuf_ptr=obuf;
	ibufe=ibuf+ibsz;
	obufe=obuf+obsz;
	
	while(1)
	{
		if(ctx->bits_ptr>=ibufe)
			__debugbreak();
	
		tg=BtFLZH_ReadNBits(ctx, 4);
		
//		printf("BtFLZH_DecodeStream: Tg=%d Bn=%d\n", tg, bn);
		
		if(!tg)break;
		
		if(tg==1)
		{
			if(bn==54)
			{
				tg=1;
			}
		
			BtFLZH_DecodePayload(ctx);
			bn++;
			continue;
		}

		if(tg==2)
		{
			BtFLZH_DecodeHuffTable(ctx);
			continue;
		}

		__debugbreak();
		break;
	}
	
	return(ctx->obuf_ptr-obuf);
}


BtFLZH_DecodeCtx *BtFLZH_AllocDecodeContext(void)
{
	BtFLZH_DecodeCtx *ctx;

	ctx=malloc(sizeof(BtFLZH_DecodeCtx));
	memset(ctx, 0, sizeof(BtFLZH_DecodeCtx));
	return(ctx);
}

void BtFLZH_FreeDecodeContext(BtFLZH_DecodeCtx *ctx)
{
	free(ctx);
}
