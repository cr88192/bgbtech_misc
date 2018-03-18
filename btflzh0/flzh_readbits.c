typedef struct BtFLZH_DecodeCtx_s BtFLZH_DecodeCtx;
struct BtFLZH_DecodeCtx_s {
byte *bits_ptr;
byte bits_pos;
byte tag_bits;
byte lit_bits;
byte dist_bits;

byte tag_luopt:1;		//tags can use lookup-only opt
byte lit_luopt:1;		//literals can use lookup-only opt
byte dist_luopt:1;		//distance, can use lookup-only opt
byte dist_noprn:1;		//distance, no prior N predictor

byte *obuf_ptr;

u32 htab_lt[BTFLZH_HTAB_LUSZ];	//Lookup T
u32 htab_ll[BTFLZH_HTAB_LUSZ];	//Lookup L
u32 htab_ld[BTFLZH_HTAB_LUSZ];	//Lookup D

u32 htab_st[256];	//Search T
u32 htab_sl[256];	//Search L
u32 htab_sd[256];	//Search D
};

/** Read up to 24 bits from bitstream, masks results. */
force_inline int BtFLZH_ReadNBits(BtFLZH_DecodeCtx *ctx, int n)
{
	int v, p;
	p=ctx->bits_pos;
	v=BTFLZH_GETU32(ctx->bits_ptr)>>p;
	p+=n;
	ctx->bits_ptr+=p>>3;
	ctx->bits_pos=p&7;
	return(BTFLZH_BITMASK(v, n));
}

/** Read up to 24 bits from bitstream, Does not mask results. */
force_inline int BtFLZH_ReadNBitsB(BtFLZH_DecodeCtx *ctx, int n)
{
	int v, p;
	p=ctx->bits_pos;
	v=BTFLZH_GETU32(ctx->bits_ptr)>>p;
	p+=n;
	ctx->bits_ptr+=p>>3;
	ctx->bits_pos=p&7;
	return(v);
}

/* Read a fixed 32-bits from bitstream. */
force_inline u32 BtFLZH_Read32Bits(BtFLZH_DecodeCtx *ctx)
{
	u32 v;
	int p;
	p=ctx->bits_pos;
	v=(u32)(BTFLZH_GETU64(ctx->bits_ptr)>>p);
	ctx->bits_ptr+=4;
	return(v);
}

/** Read 56 bits from bitstream. */
force_inline u64 BtFLZH_Read56Bits(BtFLZH_DecodeCtx *ctx)
{
	u64 v;
	int p;
	p=ctx->bits_pos;
	v=BTFLZH_GETU64(ctx->bits_ptr)>>p;
	ctx->bits_ptr+=7;
	return(v);
}

/** Skip over following N bits. */
force_inline void BtFLZH_SkipNBits(BtFLZH_DecodeCtx *ctx, int n)
{
	int p;
	p=ctx->bits_pos+n;
	ctx->bits_ptr+=p>>3;
	ctx->bits_pos=p&7;
}

/** Peek up to 24 bits from bitstream. */
force_inline int BtFLZH_PeekBits(BtFLZH_DecodeCtx *ctx)
{
	int v, p;
	p=ctx->bits_pos;
	v=BTFLZH_GETU32(ctx->bits_ptr)>>p;
	return(v);
}

/** Peek up to 56 bits from bitstream. */
force_inline u64 BtFLZH_PeekBits56(BtFLZH_DecodeCtx *ctx)
{
	u64 v;
	int p;
	p=ctx->bits_pos;
	v=BTFLZH_GETU64(ctx->bits_ptr)>>p;
	return(v);
}

/*
Stuff for reading Huffman coded symbols and copying memory is sort of a mess.
These are fairly high-activity paths, and so dominate overall performance.

A lot of this could be expressed in much more compact way, but
such is the cost of trying to have fast performance...
 */

/** Decode a Huffman coded Tag byte from bitstream. */
force_inline byte BtFLZH_DecodeHuffTag(BtFLZH_DecodeCtx *ctx)
{
	u32 b, p, m;
	byte c;
	
	/* First, see if the lookup has the symbol directly. */
	b=BtFLZH_PeekBits(ctx);
	p=ctx->htab_lt[b&BTFLZH_HTAB_LUMASK];
	if(p&0x000F0000)
	{
		/* Most of the time we are here. */
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(p&255);
	}
	
#if 1
	/* Check if first symbol is a match. */
	/* Folded out because marginally faster. */
	c=p&255;
	p=ctx->htab_st[c];
	m=BTFLZH_GENMASK((p>>16)&15);
	if(!((p^b)&m))
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(c);
	}
	c=p>>24;
#endif

	/* Search for a matching symbol. */
	while(1)
	{
		p=ctx->htab_st[c];
		m=BTFLZH_GENMASK((p>>16)&15);
		if(!((p^b)&m))
		{
			BtFLZH_SkipNBits(ctx, (p>>16)&15);
			return(c);
		}
		c=p>>24;
	}

	return(0);
}

/** Decode a Huffman coded Tag byte from bitstream, Lookup Optimized. */
force_inline byte BtFLZH_DecodeHuffTagLuO(BtFLZH_DecodeCtx *ctx)
{
	u32 b, p;
	/* In LuO case, lookup will always succeed. */
	b=BtFLZH_PeekBits(ctx);
	p=ctx->htab_lt[b&BTFLZH_HTAB_LUMASK];
	BtFLZH_SkipNBits(ctx, (p>>16)&15);
	return(p&255);
}

/* Decode a Huffman coded Raw/Literal byte from bitstream. */
force_inline byte BtFLZH_DecodeHuffLiteral(BtFLZH_DecodeCtx *ctx)
{
	u32 b, p, m;
	byte c;
	
	b=BtFLZH_PeekBits(ctx);
	p=ctx->htab_ll[b&BTFLZH_HTAB_LUMASK];
	if(p&0x000F0000)
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(p&255);
	}
	
#if 1
	c=p&255;
	p=ctx->htab_sl[c];
	m=BTFLZH_GENMASK((p>>16)&15);
	if(!((p^b)&m))
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(c);
	}
	c=p>>24;
#endif

	while(1)
	{
		p=ctx->htab_sl[c];
		m=BTFLZH_GENMASK((p>>16)&15);
		if(!((p^b)&m))
		{
			BtFLZH_SkipNBits(ctx, (p>>16)&15);
			return(c);
		}
		c=p>>24;
	}

	return(0);
}

/** Decode a pair of Huffman coded Raw/Literal bytes from bitstream. */
force_inline u16 BtFLZH_DecodeHuffLiteral2(BtFLZH_DecodeCtx *ctx)
{
	u32 b;
	u32 p;
	u16 v;
	u16 s0, s1;
	int l, ll;

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
}

/** Decode four Huffman coded Raw/Literal bytes from bitstream. */
force_inline u32 BtFLZH_DecodeHuffLiteral4(BtFLZH_DecodeCtx *ctx)
{
	u64 b;
	u32 v, p;
	int s0, s1, s2, s3, s01, s23;
//	int l0, l1, l2, l3;
	int l, ll;

#ifdef BTFLZH_NATIVE_QWORD
	/* Faster path if target has native quadwords. */

	b=BtFLZH_PeekBits56(ctx);

	/* Try fast path first, fall back if lookup fails. */
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
					v=s01|(s23<<16);
					return(v);
				}else
				{
					BtFLZH_SkipNBits(ctx, ll);
					s3=BtFLZH_DecodeHuffLiteral(ctx);
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
			s23=BtFLZH_DecodeHuffLiteral2(ctx);
			v=s0|(s1<<8)|(s23<<16);
			return(v);
		}
	}else
	{
		s0=BtFLZH_DecodeHuffLiteral(ctx);
		s1=BtFLZH_DecodeHuffLiteral(ctx);
		s23=BtFLZH_DecodeHuffLiteral2(ctx);
		v=s0|(s1<<8)|(s23<<16);
		return(v);
	}
#else
	s01=BtFLZH_DecodeHuffLiteral2(ctx);
	s23=BtFLZH_DecodeHuffLiteral2(ctx);
	v=s01|(s23<<16);
	return(v);
#endif
}


#if 1
/* Decode a Huffman coded Raw/Literal byte from bitstream, LuOpt. */
force_inline byte BtFLZH_DecodeHuffLiteralLuO(BtFLZH_DecodeCtx *ctx)
{
	u32 b, p;
	b=BtFLZH_PeekBits(ctx);
	p=ctx->htab_ll[b&BTFLZH_HTAB_LUMASK];
	BtFLZH_SkipNBits(ctx, (p>>16)&15);
	return(p&255);
}

/** Decode a pair of Huffman coded Raw/Literal bytes from bitstream. */
force_inline u16 BtFLZH_DecodeHuffLiteral2LuO(BtFLZH_DecodeCtx *ctx)
{
	u32 b;
	u32 p;
	u16 v;
	u16 s0, s1;
	int l, ll;

	b=BtFLZH_PeekBits(ctx);

	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
	ll=(p>>16)&15;	s0=(p&255);		b>>=ll;
	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
	l=(p>>16)&15;	s1=(p&255);		ll+=l;	b>>=l;

	BtFLZH_SkipNBits(ctx, ll);
	v=s0|(s1<<8);
	return(v);
}

/** Decode four Huffman coded Raw/Literal bytes from bitstream. */
force_inline u32 BtFLZH_DecodeHuffLiteral4LuO(BtFLZH_DecodeCtx *ctx)
{
//	byte *cs;
	u64 b;
	u32 v, p;
//	int s0, s1, s2, s3, s01, s23;
	int l, ll;

#ifdef BTFLZH_NATIVE_QWORD
//	__debugbreak();

	ll=ctx->bits_pos;
	b=BTFLZH_GETU64(ctx->bits_ptr)>>ll;
//	cs=ctx->bits_ptr;
//	b=BTFLZH_GETU64(cs)>>ll;
//	b=BtFLZH_PeekBits56(ctx); ll=0;
	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
//	ll=(p>>16)&15;	s0=(p&255);		b>>=ll;
//	l=(p>>16)&15;	s0=(p&255);		ll+=l;	b>>=l;
	l=(p>>16)&15;	v=(p&255);		ll+=l;	b>>=l;
	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
//	l=(p>>16)&15;	s1=(p&255);		ll+=l;	b>>=l;
	l=(p>>16)&15;	v|=(p&255)<<8;	ll+=l;	b>>=l;
//	s01=s0|(s1<<8);
	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
//	l=(p>>16)&15;	s2=(p&255);		ll+=l;	b>>=l;
	l=(p>>16)&15;	v|=(p&255)<<16;	ll+=l;	b>>=l;
	p=ctx->htab_ll[((u32)b)&BTFLZH_HTAB_LUMASK];
//	l=(p>>16)&15;	s3=(p&255);		ll+=l;
	l=(p>>16)&15;	v|=(p&255)<<24;	ll+=l;	b>>=l;
//	s23=s2|(s3<<8);
//	BtFLZH_SkipNBits(ctx, ll);
	ctx->bits_ptr+=ll>>3;
//	ctx->bits_ptr=cs+(ll>>3);
//	v=s01|(s23<<16);
	ctx->bits_pos=ll&7;
	return(v);
#else
	s01=BtFLZH_DecodeHuffLiteral2(ctx);
	s23=BtFLZH_DecodeHuffLiteral2(ctx);
	v=s01|(s23<<16);
	return(v);
#endif
}
#endif


/** Decode a Huffman coded distance prefix from bitstream. */
force_inline byte BtFLZH_DecodeHuffDist(BtFLZH_DecodeCtx *ctx)
{
	u32 b, p, m;
	byte c;
	
	b=BtFLZH_PeekBits(ctx);
	p=ctx->htab_ld[b&BTFLZH_HTAB_LUMASK];
	if(p&0x000F0000)
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(p&255);
	}
	
#if 1
	c=p&255;
	p=ctx->htab_sd[c];
	m=BTFLZH_GENMASK((p>>16)&15);
	if(!((p^b)&m))
	{
		BtFLZH_SkipNBits(ctx, (p>>16)&15);
		return(c);
	}
	c=p>>24;
#endif

	while(1)
	{
		p=ctx->htab_sd[c];
		m=BTFLZH_GENMASK((p>>16)&15);
		if(!((p^b)&m))
		{
			BtFLZH_SkipNBits(ctx, (p>>16)&15);
			return(c);
		}
		c=p>>24;
	}

	return(0);
}

/** Decode Rice-code Q prefix. */
force_inline int BtFLZH_DecodeRiceQ(BtFLZH_DecodeCtx *ctx)
{
	int b, l, n;

	b=BtFLZH_PeekBits(ctx);	
	n=0;
	while((b&255)==255)
	{
		BtFLZH_SkipNBits(ctx, 8);
		b=BtFLZH_PeekBits(ctx);	
		n+=8;
	}
	l=btflzh_rqtab[b&255];
	BtFLZH_SkipNBits(ctx, l+1);
	return(n+l);
}

/** Decode a rice-coded symbol. */
force_inline int BtFLZH_DecodeRice(BtFLZH_DecodeCtx *ctx, int rk)
{
	int q, f, v;
	/* Meh, not very fast, but Rice isn't high-volume here. */
	q=BtFLZH_DecodeRiceQ(ctx);
	f=BtFLZH_ReadNBits(ctx, rk);
	v=(q<<rk)|f;
	return(v);
}

/** Decode a distance from the bitstream. */
force_inline int BtFLZH_DecodeDistance(BtFLZH_DecodeCtx *ctx)
{
	fastint tg, tgs, nd;

#ifdef BTFLZH_NATIVE_QWORD
	u64 b;
	u32 p;
	fastint ndb, nde, nl, sg;

	/* Conjoined Path, Try reading symbol and extra bits together. */
	b=BtFLZH_PeekBits56(ctx);
	p=ctx->htab_ld[((u32)b)&BTFLZH_HTAB_LUMASK];
	if(p&0x000F0000)
	{
//		tg=p&255;
		tg=p&127;
//		sg=((sbyte)p)>>8;
		nl=(p>>16)&15;
		nde=btflzh_distxtra[tg];
		ndb=btflzh_distbase[tg];
		nd=ndb+BTFLZH_BITMASK(b>>nl, nde);
		BtFLZH_SkipNBits(ctx, nl+nde);
//		nd^=sg;
		return(nd);
	}
#endif

	/* Fallback */
	tgs=BtFLZH_DecodeHuffDist(ctx);
	tg=tgs&127;
//	sg=((sbyte)tgs)>>8;
	nd=btflzh_distbase[tg]+
		BtFLZH_ReadNBits(ctx, btflzh_distxtra[tg]);
//	nd^=sg;
	return(nd);
}

/** Decode a distance from the bitstream. */
force_inline int BtFLZH_DecodeDistanceLuO(BtFLZH_DecodeCtx *ctx)
{
#ifdef BTFLZH_NATIVE_QWORD
	fastint tg, nd;
	u64 b;
	u32 p;
	fastint ndb, nde, nl, sg, ll;

	/* Conjoined Path, Try reading symbol and extra bits together. */
//	b=BtFLZH_PeekBits56(ctx);
	ll=ctx->bits_pos;
	b=BTFLZH_GETU64(ctx->bits_ptr)>>ll;

	p=ctx->htab_ld[((u32)b)&BTFLZH_HTAB_LUMASK];
	tg=p&255;
//	tg=p&127;
//	sg=((sbyte)p)>>8;
	nl=(p>>16)&15;
	nde=btflzh_distxtra[tg];
	ndb=btflzh_distbase[tg];
	nd=ndb+BTFLZH_BITMASK(b>>nl, nde);
//	nd=ndb+((b>>nl)&((1<<nde)-1));
//	BtFLZH_SkipNBits(ctx, nl+nde);
	ll+=nl+nde;
	ctx->bits_ptr+=ll>>3;
	ctx->bits_pos=ll&7;

//	nd^=sg;
	return(nd);
#else
	return(BtFLZH_DecodeDistance(ctx));
#endif
}

#ifdef BTFLZH_USES_MEMCPY
#define BTFLZH_COPYMATCH_MOV16 \
			memcpy(ct, cs, 16); cs+=16; ct+=16;

#else
#ifdef USE_MOV_QWORD
#define BTFLZH_COPYMATCH_MOV8 \
			lpx=BTFLZH_GETU64(cs);		\
			BTFLZH_SETU64(ct, lpx);		\
			cs+=8; ct+=8;
#define BTFLZH_COPYMATCH_MOV16 \
			lpx=BTFLZH_GETU64IX(cs, 0);		lpy=BTFLZH_GETU64IX(cs, 1); \
			BTFLZH_SETU64IX(ct, 0, lpx);	BTFLZH_SETU64IX(ct, 1, lpy); \
			cs+=16; ct+=16;
#else
#define BTFLZH_COPYMATCH_MOV8 \
			px=BTFLZH_GETU32IX(cs, 0);	py=BTFLZH_GETU32IX(cs, 1);	\
			BTFLZH_SETU32IX(ct, 0, px);	BTFLZH_SETU32IX(ct, 1, py);	\
			cs+=8; ct+=8;
#define BTFLZH_COPYMATCH_MOV16 \
			px=BTFLZH_GETU32IX(cs, 0);	py=BTFLZH_GETU32IX(cs, 1);	\
			pz=BTFLZH_GETU32IX(cs, 2);	pw=BTFLZH_GETU32IX(cs, 3);	\
			BTFLZH_SETU32IX(ct, 0, px);	BTFLZH_SETU32IX(ct, 1, py);	\
			BTFLZH_SETU32IX(ct, 2, pz);	BTFLZH_SETU32IX(ct, 3, pw);	\
			cs+=16; ct+=16;
#endif
#endif

#define BTFLZH_COPYMATCH_MOV8OL \
			px=BTFLZH_GETU32IX(cs, 0);	BTFLZH_SETU32IX(ct, 0, px);	\
			py=BTFLZH_GETU32IX(cs, 1);	BTFLZH_SETU32IX(ct, 1, py);	\
			cs+=8; ct+=8;
#define BTFLZH_COPYMATCH_MOV16OL \
			px=BTFLZH_GETU32IX(cs, 0);	BTFLZH_SETU32IX(ct, 0, px);	\
			py=BTFLZH_GETU32IX(cs, 1);	BTFLZH_SETU32IX(ct, 1, py);	\
			px=BTFLZH_GETU32IX(cs, 2);	BTFLZH_SETU32IX(ct, 2, px);	\
			py=BTFLZH_GETU32IX(cs, 3);	BTFLZH_SETU32IX(ct, 3, py);	\
			cs+=16; ct+=16;

/** Do a copy for an LZ match. May write past the end of the output. */
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
#ifdef USE_MOV_QWORD
			lpx=px | ((u64)px)<<32;
			BTFLZH_SETU64IX(ct, 0, lpx);
			BTFLZH_SETU64IX(ct, 1, lpx);
			ct+=16;
			while(ct<cte)
			{
				BTFLZH_SETU64IX(ct, 0, lpx);
				BTFLZH_SETU64IX(ct, 1, lpx);
				ct+=16;
			}
#else
			while(ct<cte)
			{
				BTFLZH_SETU32IX(ct, 0, px);
				BTFLZH_SETU32IX(ct, 1, px);
				BTFLZH_SETU32IX(ct, 2, px);
				BTFLZH_SETU32IX(ct, 3, px);
				ct+=16;
			}
#endif
			return;
		}

		if(nd==2)
		{
			px=BTFLZH_GETU16(cs);
			px=px|(px<<16);
			while(ct<cte)
			{
				BTFLZH_SETU32IX(ct, 0, px);
				BTFLZH_SETU32IX(ct, 1, px);
				BTFLZH_SETU32IX(ct, 2, px);
				BTFLZH_SETU32IX(ct, 3, px);
				ct+=16;
			}
			return;
		}

		if(nd==3)
		{
			pw=BTFLZH_GETU32(cs);
			pw=pw&0x00FFFFFF;
			px=(pw<<24)|(pw    );
			py=(pw<<16)|(pw>> 8);
			pz=(pw<< 8)|(pw>>16);
			while(ct<cte)
			{
				BTFLZH_SETU32IX(ct, 0, px);
				BTFLZH_SETU32IX(ct, 1, py);
				BTFLZH_SETU32IX(ct, 2, pz);
				ct+=12;
			}
			return;
		}
		
		BTFLZH_DEBUGBREAK
	}else if(nd<16)
	{
		/*
		Input and output blocks may overlap.
		*/

//		BTFLZH_COPYMATCH_MOV8OL
		BTFLZH_COPYMATCH_MOV16OL
		while(ct<cte)
		{
//			px=BTFLZH_GETU32(cs);
//			BTFLZH_SETU32(ct, px);
//			cs+=4; ct+=4;
//			BTFLZH_COPYMATCH_MOV8OL
			BTFLZH_COPYMATCH_MOV16OL
		}
		return;
	}else
	{
		/*
		High activity area, but hard to optimize further.
		Separating based on size or using "slides" didn't gain much here.
		Or, at least relative to the massive ugliness involved.
		*/

		BTFLZH_COPYMATCH_MOV16
//		BTFLZH_COPYMATCH_MOV8
		while(ct<cte)
		{
			BTFLZH_COPYMATCH_MOV16
//			BTFLZH_COPYMATCH_MOV8
		}
		
//		while(ct<cte)
//		do
//		{
//			BTFLZH_COPYMATCH_MOV16
//		}
//		while(ct<cte);
	}
#endif
}

/* Decode a sequence of Raw/Literal bytes. */
void BtFLZH_DecodeRawLits(BtFLZH_DecodeCtx *ctx, byte *cto, int nl)
{
	byte *ct, *cte;
	u64 lpx;
	u32 px, py;
	u16 sx;
	int j;

	if(ctx->lit_bits==8)
	{
		ct=cto; cte=ct+nl;

#ifdef BTFLZH_NATIVE_QWORD
		while((ct+7)<=cte)
		{
			lpx=BtFLZH_Read56Bits(ctx);
			BTFLZH_SETU64(ct, lpx);
			ct+=7;
		}
#endif

		while((ct+4)<=cte)
		{
			px=BtFLZH_Read32Bits(ctx);
			BTFLZH_SETU32(ct, px);
			ct+=4;
		}

		if(ct<cte)
		{
			j=(int)(cte-ct);
			px=BtFLZH_ReadNBitsB(ctx, j<<3);
			BTFLZH_SETU32(ct, px);
			ct+=j;
		}

		return;
	}
	if(ctx->lit_bits>0)
	{

		if(ctx->lit_bits==7)
		{
			ct=cto; cte=ct+nl;

#ifdef BTFLZH_NATIVE_QWORD
// #if 0
			while((ct+8)<=cte)
			{
				lpx=BtFLZH_Read56Bits(ctx);

				px=	((lpx    )&0x0000007F)|
					((lpx<< 1)&0x00007F00)|
					((lpx<< 2)&0x007F0000)|
					((lpx<< 3)&0x7F000000);
				py=	((lpx>>28)&0x0000007F)|
					((lpx>>27)&0x00007F00)|
					((lpx>>26)&0x007F0000)|
					((lpx>>25)&0x7F000000);
				BTFLZH_SETU32IX(ct, 0, px);
				BTFLZH_SETU32IX(ct, 1, py);
				ct+=8;
			}
#endif

			while((ct+2)<=cte)
			{
				j=BtFLZH_ReadNBitsB(ctx, 14);
				sx=((j<<1)&0x7F00)|(j&0x007F);
				BTFLZH_SETU16(ct, sx);
				ct+=2;
			}

			while(ct<cte)
			{
				j=BtFLZH_ReadNBitsB(ctx, 7);
				*ct++=j&127;
			}
			return;
		}else
		{
			ct=cto; cte=ct+nl;
			while(ct<cte)
			{
				j=BtFLZH_ReadNBits(ctx, ctx->lit_bits);
				*ct++=(byte)j;
			}
			return;
		}
	}

	if(ctx->lit_luopt)
	{
		ct=cto; cte=ct+nl;

#if 0
		while((ct+4)<=cte)
		{
			px=BtFLZH_DecodeHuffLiteral4LuO(ctx);
			BTFLZH_SETU32(ct, px);
			ct+=4;
		}
#endif

#if 1
		while((ct+8)<=cte)
		{
			px=BtFLZH_DecodeHuffLiteral4LuO(ctx);
			BTFLZH_SETU32IX(ct, 0, px);
			py=BtFLZH_DecodeHuffLiteral4LuO(ctx);
			BTFLZH_SETU32IX(ct, 1, py);
			ct+=8;
		}

		if((ct+4)<=cte)
		{
			px=BtFLZH_DecodeHuffLiteral4LuO(ctx);
			BTFLZH_SETU32(ct, px);
			ct+=4;
		}
#endif

		if((ct+2)<=cte)
		{
			sx=BtFLZH_DecodeHuffLiteral2LuO(ctx);
			BTFLZH_SETU16(ct, sx);
			ct+=2;
		}
		if(ct<cte)
			{ *ct++=(byte)(BtFLZH_DecodeHuffLiteralLuO(ctx)); }

		return;
	}

	ct=cto; cte=ct+nl;

#if 0
	while((ct+4)<=cte)
	{
		px=BtFLZH_DecodeHuffLiteral4(ctx);
		BTFLZH_SETU32(ct, px);
		ct+=4;
	}
#endif

#if 1
	while((ct+8)<=cte)
	{
		px=BtFLZH_DecodeHuffLiteral4(ctx);
		BTFLZH_SETU32IX(ct, 0, px);
		py=BtFLZH_DecodeHuffLiteral4(ctx);
		BTFLZH_SETU32IX(ct, 1, py);
		ct+=8;
	}

	if((ct+4)<=cte)
	{
		px=BtFLZH_DecodeHuffLiteral4(ctx);
		BTFLZH_SETU32(ct, px);
		ct+=4;
	}
#endif

	if((ct+2)<=cte)
	{
		sx=BtFLZH_DecodeHuffLiteral2(ctx);
		BTFLZH_SETU16(ct, sx);
		ct+=2;
	}
	if(ct<cte)
		{ *ct++=(byte)(BtFLZH_DecodeHuffLiteral(ctx)); }
}

/** Major loop for deocoding LZ compressed payload. */
int BtFLZH_DecodePayloadDfl(BtFLZH_DecodeCtx *ctx)
{
	fastint lndt[16];
	byte lndr;
	byte *ct, *cti;
	fastint tg, tgr, tgl;
	fastint lnl, lnd;
	fastint nrb, nlb, nre, nle;
	fastint nr, nl;
	int nd;
//	byte tgnb;
	int i;
	
//	tgnb=ctx->tag_bits;
	ct=ctx->obuf_ptr;
	cti=ct;
	while(1)
	{
		tg=BtFLZH_DecodeHuffTag(ctx);
		tgl=tg&31;
		tgr=(tg>>5)&7;
		if(tgl>2)
		{
			/* Decode the suffix bits for the tag. */
			nrb=btflzh_rawbase[tgr];	nlb=btflzh_lenbase[tgl];
			nre=btflzh_rawxtra[tgr];	nle=btflzh_lenxtra[tgl];
			i=BtFLZH_ReadNBitsB(ctx, nre+nle);
			nr=nrb+BTFLZH_BITMASK((i     ), nre);
			nl=nlb+BTFLZH_BITMASK((i>>nre), nle);

			nd=BtFLZH_DecodeDistance(ctx);
//			if(!nd)nd=lnd;
			if(nd<=0)
			{
				if(!nd)	nd=lnd;
				else	nd=lndt[(lndr+nd)&15];
			}else
			{
				lndt[lndr]=lnd;
				lndr=(lndr+1)&15;
			}

			lnl=nl;
			lnd=nd;
			
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
			nr=btflzh_rawbase[tgr]+
				BtFLZH_ReadNBits(ctx, btflzh_rawxtra[tgr]);
			nd=BtFLZH_DecodeDistance(ctx);
//			if(!nd)nd=lnd;
			if(nd<=0)
			{
				if(!nd)	nd=lnd;
				else	nd=lndt[(lndr+nd)&15];
			}else
			{
				lndt[lndr]=lnd;
				lndr=(lndr+1)&15;
			}

			lnd=nd;

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
			nr=btflzh_rawbase[tgr]+
				BtFLZH_ReadNBits(ctx, btflzh_rawxtra[tgr]);
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
				if(nl<3)nl=lnl;
//				if(!nd)nd=lnd;
				if(nd<=0)
				{
					if(!nd)	nd=lnd;
					else	nd=lndt[(lndr+nd)&15];
				}else
				{
					lndt[lndr]=lnd;
					lndr=(lndr+1)&15;
				}
				lnl=nl;
				lnd=nd;
			}
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
	return((int)(ct-cti));
}

/*
   8.. 0, Length base
  15.. 9, Raw base
  19..16, Length extra
  23..20, Raw extra
*/
u32 btflzh_tagalttab[256];
byte btflzh_tagalttab_init=0;

void BtFLZH_DecodeInitTagAltLuO(BtFLZH_DecodeCtx *ctx)
{
	fastint tg, tgl, tgr;
	fastint nrb, nlb, nre, nle;
//	fastint nr, nl;
	u32 tge;
	
	if(btflzh_tagalttab_init)
		return;
	btflzh_tagalttab_init=1;
	
	for(tg=0; tg<256; tg++)
	{
		tgl=tg&31;
		tgr=(tg>>5)&7;
		if(tgl>2)
		{
			nrb=btflzh_rawbase[tgr];	nlb=btflzh_lenbase[tgl];
			nre=btflzh_rawxtra[tgr];	nle=btflzh_lenxtra[tgl];
			tge=nlb|(nrb<<9)|(nle<<16)|(nre<<20);
			btflzh_tagalttab[tg]=tge;
		}else if(tgl>0)
		{
			nrb=btflzh_rawbase[tgr];	nlb=0;
			nre=btflzh_rawxtra[tgr];	nle=0;
			tge=nlb|(nrb<<9)|(nle<<16)|(nre<<20);
			btflzh_tagalttab[tg]=tge;
		}else
		{
			tge=0;
			btflzh_tagalttab[tg]=tge;
		}
	}
}

#if 1
force_inline void BtFLZH_DecodeTagAltLuO(BtFLZH_DecodeCtx *ctx,
	fastint *rtgl, fastint *rnr, fastint *rnl)
{
#ifdef BTFLZH_NATIVE_QWORD
	u64 b;
	u32 p;
	fastint l, ll;
#endif
	fastint tg, tgl;
	fastint nrb, nlb, nre, nle;
	fastint nrm, nlm;
	fastint nr, nl;
	u32 tge;
	int i;
	
#ifdef BTFLZH_NATIVE_QWORD
	ll=ctx->bits_pos;
	b=BTFLZH_GETU64(ctx->bits_ptr)>>ll;
//	b=BtFLZH_PeekBits56(ctx);
	p=ctx->htab_lt[b&BTFLZH_HTAB_LUMASK];
	l=(p>>16)&15;
	tg=(p&255);
	tgl=tg&31;
	tge=btflzh_tagalttab[tg];
	nle=(tge>>16)&15;
	nre=(tge>>20)&15;
	ll+=l;
//	b>>=l;
	i=(int)(b>>l);
	ll+=nre+nle;
	nrm=(1<<nre)-1;
	nrb=(tge>>9)&127;
	nlm=(1<<nle)-1;
	nlb=tge&511;
	nr=nrb+(i&nrm);
	nl=nlb+((i>>nre)&nlm);

//	nlb=tge&511;
//	nrb=(tge>>9)&127;
//	nr=nrb+BTFLZH_BITMASK((i     ), nre);
//	nl=nlb+BTFLZH_BITMASK((i>>nre), nle);
//	BtFLZH_SkipNBits(ctx, l+nre+nle);
	ctx->bits_ptr+=ll>>3;
	ctx->bits_pos=ll&7;
#else
	tg=BtFLZH_DecodeHuffTagLuO(ctx);
	tgl=tg&31;
	tgr=(tg>>5)&7;
	
	tge=btflzh_tagalttab[tg];
	nlb=tge&511;		nrb=(tge>>9)&127;
	nle=(tge>>16)&15;	nre=(tge>>20)&15;
	i=BtFLZH_ReadNBitsB(ctx, nre+nle);
	nr=nrb+BTFLZH_BITMASK((i     ), nre);
	nl=nlb+BTFLZH_BITMASK((i>>nre), nle);
#endif

	*rtgl=tgl;
	*rnr=nr;
	*rnl=nl;
}
#endif

/** Major loop for deocoding LZ compressed payload, Lookup Optimized. */
int BtFLZH_DecodePayloadLuO(BtFLZH_DecodeCtx *ctx)
{
	fastint lndt[16];
	byte lndr;
	byte *ct, *cti;
//	fastint tg, tgr, tgl;
	fastint tgl;
	fastint lnl, lnd;
//	fastint nrb, nlb, nre, nle;
	fastint nr, nl;
//	u32 tge;
	int nd;
//	byte tgnb;
//	int i;
	
	BtFLZH_DecodeInitTagAltLuO(ctx);
	
	lndr=0;
//	tgnb=ctx->tag_bits;
	ct=ctx->obuf_ptr;
	cti=ct;
	while(1)
	{
		BtFLZH_DecodeTagAltLuO(ctx, &tgl, &nr, &nl);

		if(tgl>2)
		{
			/* Decode the suffix bits for the tag. */
//			nrb=btflzh_rawbase[tgr];	nlb=btflzh_lenbase[tgl];
//			nre=btflzh_rawxtra[tgr];	nle=btflzh_lenxtra[tgl];
//			i=BtFLZH_ReadNBitsB(ctx, nre+nle);
//			nr=nrb+BTFLZH_BITMASK((i     ), nre);
//			nl=nlb+BTFLZH_BITMASK((i>>nre), nle);

			nd=BtFLZH_DecodeDistanceLuO(ctx);
//			if(!nd)nd=lnd;
			if(nd<=0)
			{
				if(!nd)	nd=lnd;
				else	nd=lndt[(lndr+nd)&15];
			}else
			{
				lndt[lndr]=lnd;
				lndr=(lndr+1)&15;
			}

			lnl=nl;
			lnd=nd;
			
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
//			nr=btflzh_rawbase[tgr]+
//				BtFLZH_ReadNBits(ctx, btflzh_rawxtra[tgr]);
//			nr=nrb+BtFLZH_ReadNBits(ctx, nre);
			nd=BtFLZH_DecodeDistanceLuO(ctx);
//			if(!nd)nd=lnd;
			if(nd<=0)
			{
				if(!nd)	nd=lnd;
				else	nd=lndt[(lndr+nd)&15];
			}else
			{
				lndt[lndr]=lnd;
				lndr=(lndr+1)&15;
			}
			lnd=nd;

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
//			nr=btflzh_rawbase[tgr]+
//				BtFLZH_ReadNBits(ctx, btflzh_rawxtra[tgr]);
//			nr=nrb+BtFLZH_ReadNBits(ctx, nre);
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
			nr=BtFLZH_DecodeDistanceLuO(ctx);
			nl=BtFLZH_DecodeDistanceLuO(ctx);
			if(nl!=2)
				nd=BtFLZH_DecodeDistanceLuO(ctx);
			else
				nd=0;
			if(!(nr|nl|nd))	break;
			if(nl|nd)
			{
				if(nl<3)nl=lnl;
//				if(!nd)nd=lnd;
				if(nd<=0)
				{
					if(!nd)	nd=lnd;
					else	nd=lndt[(lndr+nd)&15];
				}else
				{
					lndt[lndr]=lnd;
					lndr=(lndr+1)&15;
				}

				lnl=nl;
				lnd=nd;
			}
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
	return((int)(ct-cti));
}

/** Major loop for deocoding LZ compressed payload.
	Lookup Optimized, No Distance Prior-N Predictor. */
int BtFLZH_DecodePayload_LuONp(BtFLZH_DecodeCtx *ctx)
{
	byte *ct, *cti;
	fastint tg, tgr, tgl;
	fastint lnl, lnd;
	fastint nrb, nlb, nre, nle;
	fastint nr, nl;
	int nd;
//	byte tgnb;
	int i;
	
//	tgnb=ctx->tag_bits;
	ct=ctx->obuf_ptr;
	cti=ct;
	while(1)
	{
		tg=BtFLZH_DecodeHuffTagLuO(ctx);
		tgl=tg&31;
		tgr=(tg>>5)&7;
		if(tgl>2)
		{
			/* Decode the suffix bits for the tag. */
			nrb=btflzh_rawbase[tgr];	nlb=btflzh_lenbase[tgl];
			nre=btflzh_rawxtra[tgr];	nle=btflzh_lenxtra[tgl];
			i=BtFLZH_ReadNBitsB(ctx, nre+nle);
			nr=nrb+BTFLZH_BITMASK((i     ), nre);
			nl=nlb+BTFLZH_BITMASK((i>>nre), nle);

			nd=BtFLZH_DecodeDistanceLuO(ctx);
			if(!nd)nd=lnd;
			lnl=nl;
			lnd=nd;
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
			nr=btflzh_rawbase[tgr]+
				BtFLZH_ReadNBits(ctx, btflzh_rawxtra[tgr]);
			nd=BtFLZH_DecodeDistanceLuO(ctx);
			if(!nd)nd=lnd;
			lnd=nd;

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
			nr=btflzh_rawbase[tgr]+
				BtFLZH_ReadNBits(ctx, btflzh_rawxtra[tgr]);
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
			nr=BtFLZH_DecodeDistanceLuO(ctx);
			nl=BtFLZH_DecodeDistanceLuO(ctx);
			if(nl!=2)
				nd=BtFLZH_DecodeDistanceLuO(ctx);
			else
				nd=0;
			if(!(nr|nl|nd))	break;
			if(nl|nd)
			{
				if(nl<3)nl=lnl;
				if(!nd)nd=lnd;
				lnl=nl;
				lnd=nd;
			}
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
	return((int)(ct-cti));
}

/** Major loop for deocoding LZ compressed payload. */
int BtFLZH_DecodePayload(BtFLZH_DecodeCtx *ctx)
{
	if(ctx->tag_luopt && ctx->dist_luopt)
	{
		if(ctx->dist_noprn)
			return(BtFLZH_DecodePayload_LuONp(ctx));
		return(BtFLZH_DecodePayloadLuO(ctx));
	}
	return(BtFLZH_DecodePayloadDfl(ctx));
}

/** Reverse the bits in a byte, yeilding a transposed value. */
int BtFLZH_DecTransposeByte(int val)
{
	return(btflzh_trans8tab[val&255]);
}

/** Reverse the bits in a word, yeilding a transposed value. */
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

/** Set up the table for transposing a table value. */
void BtFLZH_DecInitTransposeLuNb(void)
{
	int i, j;
	if(btflzh_transnbtab_init)
		return;
	btflzh_transnbtab_init=1;
	
	for(i=0; i<BTFLZH_HTAB_LUSZ; i++)
	{
		j=BtFLZH_DecTransposeWord(i<<(16-BTFLZH_HTAB_LUNB));
		btflzh_transnbtab[i]=(u16)j;
	}
}

/** Transpose a value the width of a Huffman lookup. */
int BtFLZH_DecTransposeLuNb(int val)
{
	return(btflzh_transnbtab[val&BTFLZH_HTAB_LUMASK]);
}
#endif

/** Set up the Huffman tables, this is older copy/pasted code. */
int BtFLZH_SetupHuffLengths(BtFLZH_DecodeCtx *ctx, byte *clbuf,
	u32 *ltab, u32 *stab)
{
	u16 tc[256];	//symbol bit-code
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
		tl[i]=(byte)l;
		if(!l)continue;

//		tm[i]=(1<<l)-1;
		tc[i]=(u16)(nc[l]++);

		if(nn[l]>=0)
		{
			tn[nn[l]]=(u16)i;
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
			if(ff<0)ff=(u16)nf[i];
			if(j>=0)tn[j]=(u16)nf[i];
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
		tc[i]=(u16)k;
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

/**
Decode a table of Huffman-table symbol lengths.
These are Rice-coded as per the format spec.
*/
int BtFLZH_DecodeHuffLengths(BtFLZH_DecodeCtx *ctx, byte *obuf)
{
	byte stf[32];
	byte k0, k1, l, ll;
	int i, j0, j1, n;

	memcpy(stf, btflzh_initstf, 32);

	ll=0; i=0;
	while(i<256)
	{
#if 1
		j0=BtFLZH_DecodeRice(ctx, BTFLZH_STF_RK);
		if(j0>>5)
			{ BTFLZH_DEBUGBREAK }
		k0=stf[j0];
		j1=BTFLZH_STFSWAP(j0);
		k1=stf[j1];
		stf[j0]=k1;
		stf[j1]=k0;
		
		l=k0;
#endif

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
			BTFLZH_DEBUGBREAK
			break;
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
		BTFLZH_DEBUGBREAK
		break;
	}
	return(0);
}

int BtFLZH_DecodeHuffTable(BtFLZH_DecodeCtx *ctx)
{
	byte clbuf[256];
	u32 *ltab, *stab;
	byte tn, tf, ti, nb;
	int i;

	tn=(byte)(BtFLZH_ReadNBits(ctx, 2));
	tf=(byte)(BtFLZH_ReadNBits(ctx, 2));

//	printf("BtFLZH_DecodeHuffTable %d %d\n", tn, tf);

	switch(tn)
	{
	case 1:
		ltab=ctx->htab_lt;
		stab=ctx->htab_st;
		ctx->tag_bits=0;
		ctx->tag_luopt=0;
		break;
	case 2:
		ltab=ctx->htab_ll;
		stab=ctx->htab_sl;
		ctx->lit_bits=0;
		ctx->lit_luopt=0;
		break;
	case 3:
		ltab=ctx->htab_ld;
		stab=ctx->htab_sd;
		ctx->dist_bits=0;
		ctx->dist_luopt=0;
		ctx->dist_noprn=0;
		break;
	}

	if(tf==0)
	{
		ti=(byte)(BtFLZH_ReadNBits(ctx, 6));
		
		nb=8-ti;
		if(nb<0)nb=0;

		if(nb==0)
		{
			BtFLZH_DecodeFixedLengths(ctx, clbuf, ti);
			BtFLZH_SetupHuffLengths(ctx, clbuf, ltab, stab);
		}else
		{
			for(i=0; i<BTFLZH_HTAB_LUSZ; i++)
				{ ltab[i]=(i&((1<<nb)-1))|(nb<<16); }
			for(i=0; i<256; i++)
				{ stab[i]=0; }
			for(i=0; i<(1<<nb); i++)
				stab[i]=i|(nb<<16);
		}
		
		if(tn==1)
			{ ctx->tag_bits=nb; ctx->tag_luopt=1; }
		if(tn==2)
			{ ctx->lit_bits=nb; ctx->lit_luopt=1; }
		if(tn==3)
		{
			ctx->dist_bits=nb; ctx->dist_luopt=1;
//			if(nb && (nb<8))
			if(nb && (nb<7) || ((nb<8) && ctx->tag_bits))
				{ ctx->dist_noprn=1; }
		}
		
	}else if(tf==1)
	{
		BtFLZH_DecodeHuffLengths(ctx, clbuf);
		BtFLZH_SetupHuffLengths(ctx, clbuf, ltab, stab);

		if(tn==3)
		{
			ctx->dist_noprn=1;
			for(i=120; i<128; i++)
			{
				if(clbuf[i])
					{ ctx->dist_noprn=0; break; }
			}
		}

		for(i=0; i<256; i++)
			if(clbuf[i]>BTFLZH_HTAB_LUNB)
				break;
		if(i>=256)
		{
			for(i=0; i<BTFLZH_HTAB_LUSZ; i++)
			{
				if(!(ltab[i]&0xF0000))
					{ BTFLZH_DEBUGBREAK }
			}

			if(tn==1)
				{ ctx->tag_luopt=1; }
			if(tn==2)
				{ ctx->lit_luopt=1; }
			if(tn==3)
				{ ctx->dist_luopt=1; }
		}
	}else
	{
		BTFLZH_DEBUGBREAK
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
			{ BTFLZH_DEBUGBREAK }
	
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

		BTFLZH_DEBUGBREAK
		break;
	}
	
	return((int)(ctx->obuf_ptr-obuf));
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
