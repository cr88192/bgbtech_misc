/*
 * RP2 Post Encoder: Huffman
 * Magic: 20 00
 *
 * This version consists of a bitstream, which is broken up into chunks.
 * This bitstream will be encoded in LSB first order.
 *
 * 4b chunk tag:
 *
 *   0: End of Stream
 *
 *   1: Raw Bytes Chunk
 *     len: VLI(4)
 *     data: len*8 bits
 *       Data will remain in bitstram, so bytes may be misaligned.
 *
 *   2: Single-Table Compressed Chunk
 *     tti: 2-bits  //Table Index (0=T / 1=L / 2=D)
 *     ttg: 2-bits  //Table Type Tag
 *       if(ttg==1)
 *         Packed-Symbol-Lengths
 *       if(ttg==2)
 *         Reuse existing lengths.
 *     len: VLI(4)
 *     data: Huff compressed XBlob (table=TTI)
 *
 *   3: Multi-Table Compressed Chunk
 *     ttg_t: 2b  //Table Type, Tag
 *       if(ttg_t==1)
 *         Packed-Symbol-Lengths, Tag Table
 *     ttg_l: 2b  //Table Type, Literals
 *       if(ttg_l==1)
 *         Packed-Symbol-Lengths, Literal Table
 *     ttg_d: 2b  //Table Type, Distance
 *       if(ttg_d==1)
 *         Packed-Symbol-Lengths, Distance Table
 *     len_t: VLI(4)
 *     len_l: VLI(4)
 *     len_d: VLI(4)
 *     data_t: Huff compressed XBlob, Tag
 *     data_l: Huff compressed XBlob, Literal
 *     data_d: Huff compressed XBlob, Distance
 *
 * Table Types:
 *   0: Raw Bytes (8-bits per byte within bitstream)
 *     Doesn't modify prior Hufftab for this index.
 *     Simply causes the blob to decode as fixed 8 bits.
 *   1: Packed Lengths, Update Table
 *     Encode Packed lengths for a new version of the table.
 *   2: Reuse Last Table
 *     No new lengths, keep previous table.
 *   3: Reserved for Now.
 *
 * VLI(K): Will be encoded as an K-Bit prefix giving the length of the suffix.
 * This will then read the (N-1) bit suffix holding the rest of the value.
 * This encoding strips the highest '1' bit from the value, which is added back.
 *
 * XBlob:
 *   ty: 2b  //Encoding Type
 *   if(ty==0)
 *     Linearly encoded Huffman Symbols
 *   if(ty==1)
 *     bits0: VLI(5)	//Total Number of Bits, Plane 0
 *     bits1: VLI(5)	//Total Number of Bits, Plane 1
 *     bits2: VLI(5)	//Total Number of Bits, Plane 2
 *     bits3: VLI(5)	//Total Number of Bits, Plane 3
 *     Huffman Symbols, Plane 0
 *     Huffman Symbols, Plane 1
 *     Huffman Symbols, Plane 2
 *     Huffman Symbols, Plane 3
 *   2/3: Reserved
 *
 * The XBlob format may encode the Huffman coded stream into 4 planes.
 * This can potentially allow for faster decoding by reducing dependencies.
 * If the number of symbols was not evenly divisible by 4,
 * it will be encoded as-if padded up to the next multiple of 4.
 *
 * Huffman Tables:
 * Will encode the tables as a series of lengths in "Canonical Huffman" style.
 * Will use 4-bits for lengths.
 *   0: represents an unused symbol
 *   1..D: Lengths
 *   E: 4b follows, run of 3..18 zeroes.
 *   F: 2b follows:
 *     00: 6b follows, run of 19..82 zeroes
 *     01: 6b follows, run of 4..66 repeats of last value.
 *       Bias is 3, but the 6b is 1 or greater.
 *       If the 6b value is 0, this is an EOT marker.
 *         Rest of table filled with zeroes.
 *     10/11: Reserved
 *
 * Note that Blobs may impose a size limit of a little over 4K.
 * The encoder may not exceed this size limit.
 */

// #define POSTRP2HUFF_LEN13

#ifdef POSTRP2HUFF_LEN13
#define POSTRP2HUFF_HTABSZ 8192
#define POSTRP2HUFF_HTABNB 13
#else
#define POSTRP2HUFF_HTABSZ 4096
#define POSTRP2HUFF_HTABNB 12
#endif

#define POSTRP2HUFF_HTABMSK		(POSTRP2HUFF_HTABSZ-1)

#define POSTRP2HUFF_MAXBLOB		(4096+48)

#define POSTRP2HUFF_STATUS_BADTAG		1
#define POSTRP2HUFF_STATUS_BADHUFF		2
#define POSTRP2HUFF_STATUS_BADBLOB		3


typedef struct PostRp2Huff_DecState_s PostRp2Huff_DecState;

struct PostRp2Huff_DecState_s {
	byte *cs;
	byte *ct;
	byte pos;
	byte status;

	u16 hufftab[3][POSTRP2HUFF_HTABSZ];

	byte *ttgbuf;
	byte *ttrbuf;
	byte *ttdbuf;
};

static const byte tkulz_trans4[16]={
	0x0,0x8,0x4,0xC, 0x2,0xA,0x6,0xE,
	0x1,0x9,0x5,0xD, 0x3,0xB,0x7,0xF};

static const byte tkulz_qtab[16]={
	0, 1, 0, 2,  0, 1, 0, 3,  0, 1, 0, 2,  0, 1, 0, 4 };

void PostRp2Huff_SkipBits(PostRp2Huff_DecState *ctx, int bits)
{
	byte *cs;
	int k;

	k=ctx->pos+bits;
	ctx->pos=k&7;
	ctx->cs=(ctx->cs)+(k>>3);
}

u64 PostRp2Huff_PeekBits(PostRp2Huff_DecState *ctx, int bits)
{
	byte *cs;
	u64 w, b;
	int p, k;

	cs=ctx->cs;
	p=ctx->pos;
	w=gfxedit_getu64(cs);
	b=(w>>p)&((1LL<<bits)-1);
	return(b);
}

u64 PostRp2Huff_ReadBits(PostRp2Huff_DecState *ctx, int bits)
{
	u64 b;
	b=PostRp2Huff_PeekBits(ctx, bits);
	PostRp2Huff_SkipBits(ctx, bits);
	return(b);
}

int TKuLZ_ReadPackVLI(PostRp2Huff_DecState *ctx, int pfsz)
{
	int e, v;
	e=PostRp2Huff_ReadBits(ctx, pfsz)-1;
	if(e<0)
		return(0);
	v=(1<<e)|PostRp2Huff_ReadBits(ctx, e);
	return(v);
}

int PostRp2Huff_ReadPackedLengths(PostRp2Huff_DecState *ctx, byte *cls)
{
	byte *t, *te;
	int i, j, c, nz, zc, lc;
	
	t=cls; te=cls+256; lc=0;
	while(t<te)
	{
		c=PostRp2Huff_ReadBits(ctx, 4); zc=0; nz=0;
		if(c<14)
			{ *t++=c; lc=c; }
		else if(c==14)
			{ nz=PostRp2Huff_ReadBits(ctx, 4)+3; }
		else if(c==15)
		{
			j=PostRp2Huff_ReadBits(ctx, 2);
			if(j==0)
				{ nz=PostRp2Huff_ReadBits(ctx, 6)+19; }
			else if(j==1)
			{
				nz=PostRp2Huff_ReadBits(ctx, 6)+3; zc=lc;
				if(nz==3)
					{ nz=te-t; zc=0; }
			}
			else
				{ ctx->status=POSTRP2HUFF_STATUS_BADHUFF; }
		}
		while(nz>0)
			{ *t++=zc; nz--; }
	}
	
	if(t>te)
		{ ctx->status=POSTRP2HUFF_STATUS_BADHUFF; }
	return(0);
}


byte PostRp2Huff_TransposeByte(byte v)
{
	return((tkulz_trans4[v&15]<<4)|tkulz_trans4[(v>>4)&15]);
}

u16 PostRp2Huff_TransposeWord(u16 v)
{
	int n0, n1, n2, n3, nv, nv1;
	n0=tkulz_trans4[(v    )&15];	n1=tkulz_trans4[(v>> 4)&15];
	n2=tkulz_trans4[(v>> 8)&15];	n3=tkulz_trans4[(v>>12)&15];
	nv=(n0<<12)|(n1<<8)|(n2<<4)|n3;
	return(nv);
}

void PostRp2Huff_SetupTableLengths(PostRp2Huff_DecState *ctx,
	u16 *htab, byte *cls)
{
	short idx[16];
	short chn[256];
	int c, l, tc, tb, td, tbe, ts, ts3;
	int i, j, k;

	for(i=0; i<16; i++)
		idx[i]=-1;
	for(i=255; i>=0; i--)
		{ j=cls[i]; chn[i]=idx[j]; idx[j]=i; }
	
	c=0;
	for(l=1; l<(POSTRP2HUFF_HTABNB+1); l++)
	{
		i=idx[l];
		ts=1<<l;
		ts3=ts<<3;
		while(i>=0)
		{
			tc=c<<(POSTRP2HUFF_HTABNB-l);
			tb=PostRp2Huff_TransposeWord(tc)>>(16-POSTRP2HUFF_HTABNB);
			td=(l<<12)|i;
			while((tb+ts3)<POSTRP2HUFF_HTABSZ)
			{	htab[tb]=td; tb+=ts;	htab[tb]=td; tb+=ts;
				htab[tb]=td; tb+=ts;	htab[tb]=td; tb+=ts;
				htab[tb]=td; tb+=ts;	htab[tb]=td; tb+=ts;
				htab[tb]=td; tb+=ts;	htab[tb]=td; tb+=ts;	}
			while(tb<POSTRP2HUFF_HTABSZ)
				{ htab[tb]=td; tb+=ts; }
			i=chn[i];
			c++;			
		}
		c=c<<1;
	}
}

void PostRp2Huff_ReadSymbolBlob(
	PostRp2Huff_DecState *ctx, int tab, byte *dst, int len)
{
	byte *cs, *ct;
	u16 *htab;
	u64 win;
	int pos, hte, hti, l;

	cs=ctx->cs;
	pos=ctx->pos;
	htab=ctx->hufftab[tab];
	ct=dst;

	l=len;

#if 1
	while(l>=5)
	{
		win=gfxedit_getu64(cs);

		hti=(win>>pos)&POSTRP2HUFF_HTABMSK;
		hte=htab[hti];
		pos+=hte>>12;
		ct[0]=hte;

		hti=(win>>pos)&POSTRP2HUFF_HTABMSK;
		hte=htab[hti];
		pos+=hte>>12;
		ct[1]=hte;

		hti=(win>>pos)&POSTRP2HUFF_HTABMSK;
		hte=htab[hti];
		pos+=hte>>12;
		ct[2]=hte;

		hti=(win>>pos)&POSTRP2HUFF_HTABMSK;
		hte=htab[hti];
		pos+=hte>>12;
		ct[3]=hte;

		hti=(win>>pos)&POSTRP2HUFF_HTABMSK;
		hte=htab[hti];
		pos+=hte>>12;
		ct[4]=hte;

		cs+=(pos>>3);
		pos&=7;
		ct+=5;
		l-=5;
	}
#endif

	while(l)
	{
		win=gfxedit_getu64(cs);
		hte=htab[(win>>pos)&POSTRP2HUFF_HTABMSK];
		pos+=hte>>12;
		*ct++=hte;
		cs+=(pos>>3);
		pos&=7;
		l--;
	}

	ctx->cs=cs;
	ctx->pos=pos;
}

void PostRp2Huff_ReadSymbolBlob4W(
	PostRp2Huff_DecState *ctx, int tab, byte *dst, int len)
{
	byte *cs0, *cs1, *cs2, *cs3, *ct;
	u16 *htab;
	u64 win0, win1, win2, win3;
	int nbi0, nbi1, nbi2, nbi3;
	int pos0, pos1, pos2, pos3, hte0, hte1, hte2, hte3;
	int hti0, hti1, hti2, hti3, l;

	nbi0=TKuLZ_ReadPackVLI(ctx, 5);
	nbi1=TKuLZ_ReadPackVLI(ctx, 5);
	nbi2=TKuLZ_ReadPackVLI(ctx, 5);
	nbi3=TKuLZ_ReadPackVLI(ctx, 5);

#if 0
	if(	((nbi0<64) || (nbi0>((POSTRP2HUFF_MAXBLOB/4)*POSTRP2HUFF_HTABNB)))	||
		((nbi1<64) || (nbi1>((POSTRP2HUFF_MAXBLOB/4)*POSTRP2HUFF_HTABNB)))	||
		((nbi2<64) || (nbi2>((POSTRP2HUFF_MAXBLOB/4)*POSTRP2HUFF_HTABNB)))	||
		((nbi3<64) || (nbi3>((POSTRP2HUFF_MAXBLOB/4)*POSTRP2HUFF_HTABNB)))	)
	{
		ctx->status=POSTRP2HUFF_STATUS_BADBLOB;
		return;
	}
#endif

	cs0=ctx->cs;
	pos0=ctx->pos;

	pos1=pos0+nbi0;
	cs1=cs0+(pos1>>3);
	pos1&=7;

	pos2=pos1+nbi1;
	cs2=cs1+(pos2>>3);
	pos2&=7;

	pos3=pos2+nbi2;
	cs3=cs2+(pos3>>3);
	pos3&=7;

	htab=ctx->hufftab[tab];
	ct=dst;

//	l=len>>2;
	l=(len+3)>>2;

#if 1
//	while(l>=5)
	while(l>=4)
	{
		win0=gfxedit_getu64(cs0);	win1=gfxedit_getu64(cs1);
		win2=gfxedit_getu64(cs2);	win3=gfxedit_getu64(cs3);

		hti0=(win0>>pos0)&POSTRP2HUFF_HTABMSK;
		hti1=(win1>>pos1)&POSTRP2HUFF_HTABMSK;
		hti2=(win2>>pos2)&POSTRP2HUFF_HTABMSK;
		hti3=(win3>>pos3)&POSTRP2HUFF_HTABMSK;
		hte0=htab[hti0];			hte1=htab[hti1];
		hte2=htab[hti2];			hte3=htab[hti3];
		pos0+=hte0>>12;				pos1+=hte1>>12;
		pos2+=hte2>>12;				pos3+=hte3>>12;
		ct[0]=hte0;					ct[1]=hte1;
		ct[2]=hte2;					ct[3]=hte3;

		hti0=(win0>>pos0)&POSTRP2HUFF_HTABMSK;
		hti1=(win1>>pos1)&POSTRP2HUFF_HTABMSK;
		hti2=(win2>>pos2)&POSTRP2HUFF_HTABMSK;
		hti3=(win3>>pos3)&POSTRP2HUFF_HTABMSK;
		hte0=htab[hti0];			hte1=htab[hti1];
		hte2=htab[hti2];			hte3=htab[hti3];
		pos0+=hte0>>12;				pos1+=hte1>>12;
		pos2+=hte2>>12;				pos3+=hte3>>12;
		ct[4]=hte0;					ct[5]=hte1;
		ct[6]=hte2;					ct[7]=hte3;

		hti0=(win0>>pos0)&POSTRP2HUFF_HTABMSK;
		hti1=(win1>>pos1)&POSTRP2HUFF_HTABMSK;
		hti2=(win2>>pos2)&POSTRP2HUFF_HTABMSK;
		hti3=(win3>>pos3)&POSTRP2HUFF_HTABMSK;
		hte0=htab[hti0];			hte1=htab[hti1];
		hte2=htab[hti2];			hte3=htab[hti3];
		pos0+=hte0>>12;				pos1+=hte1>>12;
		pos2+=hte2>>12;				pos3+=hte3>>12;
		ct[ 8]=hte0;				ct[ 9]=hte1;
		ct[10]=hte2;				ct[11]=hte3;

		hti0=(win0>>pos0)&POSTRP2HUFF_HTABMSK;
		hti1=(win1>>pos1)&POSTRP2HUFF_HTABMSK;
		hti2=(win2>>pos2)&POSTRP2HUFF_HTABMSK;
		hti3=(win3>>pos3)&POSTRP2HUFF_HTABMSK;
		hte0=htab[hti0];			hte1=htab[hti1];
		hte2=htab[hti2];			hte3=htab[hti3];
		pos0+=hte0>>12;				pos1+=hte1>>12;
		pos2+=hte2>>12;				pos3+=hte3>>12;
		ct[12]=hte0;				ct[13]=hte1;
		ct[14]=hte2;				ct[15]=hte3;

#if 0
		hti0=(pos0+13);		hti1=(pos1+13);
		hti2=(pos2+13);		hti3=(pos3+13);
		if((hti0|hti1|hti2|hti3)&0x40)
		{
			cs0+=(pos0>>3);				cs1+=(pos1>>3);
			cs2+=(pos2>>3);				cs3+=(pos3>>3);
			pos0&=7;					pos1&=7;
			pos2&=7;					pos3&=7;
			ct+=16;						l-=4;
			continue;
		}

		hti0=(win0>>pos0)&POSTRP2HUFF_HTABMSK;
		hti1=(win1>>pos1)&POSTRP2HUFF_HTABMSK;
		hti2=(win2>>pos2)&POSTRP2HUFF_HTABMSK;
		hti3=(win3>>pos3)&POSTRP2HUFF_HTABMSK;
		hte0=htab[hti0];			hte1=htab[hti1];
		hte2=htab[hti2];			hte3=htab[hti3];
		pos0+=hte0>>12;				pos1+=hte1>>12;
		pos2+=hte2>>12;				pos3+=hte3>>12;
		ct[16]=hte0;				ct[17]=hte1;
		ct[18]=hte2;				ct[19]=hte3;
#endif

		cs0+=(pos0>>3);				cs1+=(pos1>>3);
		cs2+=(pos2>>3);				cs3+=(pos3>>3);
		pos0&=7;					pos1&=7;
		pos2&=7;					pos3&=7;
//		ct+=20;						l-=5;
		ct+=16;						l-=4;
	}
#endif

	while(l>0)
	{
		win0=gfxedit_getu64(cs0);		win1=gfxedit_getu64(cs1);
		win2=gfxedit_getu64(cs2);		win3=gfxedit_getu64(cs3);

		hti0=(win0>>pos0)&POSTRP2HUFF_HTABMSK;
		hti1=(win1>>pos1)&POSTRP2HUFF_HTABMSK;
		hti2=(win2>>pos2)&POSTRP2HUFF_HTABMSK;
		hti3=(win3>>pos3)&POSTRP2HUFF_HTABMSK;
		hte0=htab[hti0];				hte1=htab[hti1];
		hte2=htab[hti2];				hte3=htab[hti3];
		pos0+=hte0>>12;					pos1+=hte1>>12;
		pos2+=hte2>>12;					pos3+=hte3>>12;
		ct[0]=hte0;						ct[1]=hte1;
		ct[2]=hte2;						ct[3]=hte3;

		cs0+=(pos0>>3);					cs1+=(pos1>>3);
		cs2+=(pos2>>3);					cs3+=(pos3>>3);
		pos0&=7;						pos1&=7;
		pos2&=7;						pos3&=7;
		l--;							ct+=4;
	}

	ctx->cs=cs3;
	ctx->pos=pos3;
}

// static int printblob_harr[512];
static int printblob_rov;

void PostRp2Huff_PrintBlobCheck(
	int tab, byte *dst, int len)
{
#if 0
	u32 h0, h1, h, hi;
	int hne;
	byte *cs, *cse;
	
	cs=dst; cse=dst+len; h0=1; h1=0;
	while(cs<cse)
		{ h0+=*cs++; h1+=h0; }
	h=h0^h1;
	
	hi=printblob_rov++;
	
	hne=(h!=printblob_harr[hi]);
	printblob_harr[hi]=h;
	
	printf("  %d %4dB %08X, %d\n", tab, len, h, hne);
#endif
}

void PostRp2Huff_ReadSymbolXBlob(
	PostRp2Huff_DecState *ctx, int tab, byte *dst, int len)
{
	int ti;

	ti=PostRp2Huff_ReadBits(ctx, 2);
	
	if(ti==0)
	{
		PostRp2Huff_ReadSymbolBlob(ctx, tab, dst, len);
	}else
		if(ti==1)
	{
		PostRp2Huff_ReadSymbolBlob4W(ctx, tab, dst, len);
	}else
	{
		ctx->status=POSTRP2HUFF_STATUS_BADTAG;
//		printf("PostRp2Huff_ReadSymbolXBlob: Bad Type %d\n", ti);
		return;
	}
	
	PostRp2Huff_PrintBlobCheck(tab, dst, len);
}

void PostRp2Huff_ReadRawBytesBlob(
	PostRp2Huff_DecState *ctx, byte *dst, int len)
{
	byte *cs, *ct;
	u64 win;
	int pos, l;

	cs=ctx->cs;
	pos=ctx->pos;
	ct=dst;

	if(!pos)
	{
		l=len;
		while(l>=8)
		{
			win=gfxedit_getu64(cs);
			gfxedit_setu64(ct, win);
			cs+=8; ct+=8; l-=8;
		}

		if(l)
		{
			win=gfxedit_getu64(cs);
			gfxedit_setu64(ct, win);
			cs+=l; ct+=l;
		}
		ctx->cs=cs;
		return;
	}

	l=len;
	while(l>=7)
	{
		win=gfxedit_getu64(cs);
		gfxedit_setu64(ct, win>>pos);
		cs+=7;
		ct+=7;
		l-=7;
	}
	if(l)
	{
		win=gfxedit_getu64(cs);
		gfxedit_setu64(ct, win>>pos);
		cs+=l; ct+=l;
	}
	ctx->cs=cs;
	ctx->pos=pos;
}


void PostRp2Huff_UnpackRp2BlobSingleInner(
	PostRp2Huff_DecState *ctx)
{
	byte tgcls[256+96];
	byte *ct, *cs_tg, *cs_tge, *cs_tr, *cs_rd;
	int pos_r, pos_d;
	int ntb, nrb, ndb, tg, ti, nd;
	int i, j, k, l;

	ti=PostRp2Huff_ReadBits(ctx, 2);
	tg=PostRp2Huff_ReadBits(ctx, 2);

	if((ti==3) || (tg==3))
	{
		ctx->status=POSTRP2HUFF_STATUS_BADTAG;
		return;
	}

	if(tg==0)
	{
		ntb=TKuLZ_ReadPackVLI(ctx, 4);
		PostRp2Huff_ReadRawBytesBlob(ctx, ctx->ct, ntb);
		ctx->ct+=ntb;
		return;
	}

	if(tg==1)
	{
		PostRp2Huff_ReadPackedLengths(ctx, tgcls);
		PostRp2Huff_SetupTableLengths(ctx, ctx->hufftab[ti], tgcls);	
	}

	ntb=TKuLZ_ReadPackVLI(ctx, 4);
	PostRp2Huff_ReadSymbolXBlob(ctx, ti, ctx->ct, ntb);
	ctx->ct+=ntb;
}

void PostRp2Huff_UnpackRp2BlobMultiInner(
	PostRp2Huff_DecState *ctx)
{
	byte tgcls[256+96];
	byte trcls[256+96];
	byte tdcls[256+96];
	byte *ttgbuf;
	byte *ttrbuf;
	byte *ttdbuf;
	byte *ct, *cs_tg, *cs_tge, *cs_tr, *cs_td;
	int pos_r, pos_d;
	int tg_t, tg_l, tg_d;
	int ntb, nrb, ndb, ntp, nrp, ndp, tg, ti, nd, nr;
	int i, j, k, l;
	
	ttgbuf=ctx->ttgbuf;
	ttrbuf=ctx->ttrbuf;
	ttdbuf=ctx->ttdbuf;
	
	if(!ttgbuf)
	{
		ttgbuf=malloc((POSTRP2HUFF_MAXBLOB+64)*3);
		ttrbuf=ttgbuf+(POSTRP2HUFF_MAXBLOB+16);
		ttdbuf=ttrbuf+(POSTRP2HUFF_MAXBLOB+16);
		ctx->ttgbuf=ttgbuf;
		ctx->ttrbuf=ttrbuf;
		ctx->ttdbuf=ttdbuf;
	}
	
	tg_t=PostRp2Huff_ReadBits(ctx, 2);
	if(tg_t==1)
	{
		PostRp2Huff_ReadPackedLengths(ctx, tgcls);
		if(ctx->status)
			return;
		PostRp2Huff_SetupTableLengths(ctx, ctx->hufftab[0], tgcls);
		if(ctx->status)
			return;
	}

	tg_l=PostRp2Huff_ReadBits(ctx, 2);
	if(tg_l==1)
	{
		PostRp2Huff_ReadPackedLengths(ctx, tgcls);
		if(ctx->status)
			return;
		PostRp2Huff_SetupTableLengths(ctx, ctx->hufftab[1], tgcls);
		if(ctx->status)
			return;
	}

	tg_d=PostRp2Huff_ReadBits(ctx, 2);
	if(tg_d==1)
	{
		PostRp2Huff_ReadPackedLengths(ctx, tgcls);
		if(ctx->status)
			return;
		PostRp2Huff_SetupTableLengths(ctx, ctx->hufftab[2], tgcls);
		if(ctx->status)
			return;
	}

	if((tg_t==3) || (tg_l==3) || (tg_d==3))
	{
		ctx->status=POSTRP2HUFF_STATUS_BADTAG;
		return;
	}

	ntb=TKuLZ_ReadPackVLI(ctx, 4);
	nrb=TKuLZ_ReadPackVLI(ctx, 4);
	ndb=TKuLZ_ReadPackVLI(ctx, 4);

	if(	(ntb>POSTRP2HUFF_MAXBLOB) ||
		(nrb>POSTRP2HUFF_MAXBLOB) ||
		(ndb>POSTRP2HUFF_MAXBLOB))
	{
		ctx->status=POSTRP2HUFF_STATUS_BADBLOB;
		return;
	}

	if(tg_t)
		{ PostRp2Huff_ReadSymbolXBlob(ctx, 0, ttgbuf, ntb); }
	else
		{ PostRp2Huff_ReadRawBytesBlob(ctx, ttgbuf, ntb); }

	if(tg_l)
		{ PostRp2Huff_ReadSymbolXBlob(ctx, 1, ttrbuf, nrb); }
	else
		{ PostRp2Huff_ReadRawBytesBlob(ctx, ttrbuf, nrb); }

	if(tg_d)
		{ PostRp2Huff_ReadSymbolXBlob(ctx, 2, ttdbuf, ndb); }
	else
		{ PostRp2Huff_ReadRawBytesBlob(ctx, ttdbuf, ndb); }
	
	ct=ctx->ct;
	
	cs_tg=ttgbuf;
	cs_tr=ttrbuf;
	cs_td=ttdbuf;
	cs_tge=cs_tg+ntb;
//	pos_r=0; pos_d=0;
//	for(i=0; i<ntb; i++)
	while(cs_tg<cs_tge)
	{
		tg=*cs_tg++;
		*ct++=tg;
		if(!(tg&0x01))
		{
			nd=1;
			nr=(tg>>1)&7;
		}else
			if(!(tg&0x02))
		{
			nd=2;
			nr=(tg>>2)&7;
		}else
			if(!(tg&0x04))
		{
			nd=3;
			nr=(tg>>3)&7;
		}else
			if(!(tg&0x08))
		{
			nd=0;
			nr=(((tg>>4)&15)+1)*8;
		}else
			if(!(tg&0x10))
		{
			ti=*cs_td++;
			*ct++=ti;
			nd=(ti&1);
			nr=(tg>>5)&7;
		}else
			if(!(tg&0x20))
		{
			nd=0;
			nr=(tg>>6)&3;
		}else
			if(!(tg&0x40))
		{
			ti=*cs_td++;
			*ct++=ti;
			nd=0;
			j=((tg>>7)&1)|(ti<<1);
			nr=(j+1)*8;
		}else
			if(!(tg&0x80))
		{
			ti=*cs_td++;
			*ct++=ti;
			if(ti&1)
				{ nd=4; nr=(ti>>1)&7; }
			else
				{ nd=0; nr=0; }
		}else
		{
			ti=*cs_td++;
			*ct++=ti;
			
			if(!(ti&1))
				{ nd=3; nr=(ti>>1)&7; }
		}

		if(nd)
		{
			memcpy(ct, cs_td, nd);
			cs_td+=nd;
			ct+=nd;
		}
		if(nr)
		{
			memcpy(ct, cs_tr, nr);
			cs_tr+=nr;
			ct+=nr;
		}
	}

	ctx->ct=ct;
}

int PostRp2Huff_UnpackRp2BlobCtx(
	PostRp2Huff_DecState *ctx, byte *obuf, byte *ibuf)
{
	int tag, nrb;

	ctx->cs=ibuf+2;
	ctx->ct=obuf;
	ctx->pos=0;
	ctx->status=0;
	
	tag=PostRp2Huff_ReadBits(ctx, 4);
	while(tag)
	{
		if(tag==1)
		{
//			nrb=TKuLZ_ReadRiceVLN(ctx, 3);
			nrb=TKuLZ_ReadPackVLI(ctx, 4);
			PostRp2Huff_ReadRawBytesBlob(ctx, ctx->ct, nrb);
			ctx->ct+=nrb;
		}else
			if(tag==2)
		{
			PostRp2Huff_UnpackRp2BlobSingleInner(ctx);
		}else
			if(tag==3)
		{
			PostRp2Huff_UnpackRp2BlobMultiInner(ctx);
		}else
		{
			ctx->status=POSTRP2HUFF_STATUS_BADTAG;
			printf("PostRp2Huff_UnpackRp2BlobCtx: Bad Tag %d\n", tag);
			break;
		}
		if(ctx->status)
		{
			printf("PostRp2Huff_UnpackRp2BlobCtx: Status %d\n", ctx->status);
			break;
		}
		
		tag=PostRp2Huff_ReadBits(ctx, 4);
	}
	
	if(ctx->status)
		return(-1);
	
	return(ctx->ct-obuf);
}

int PostRp2Huff_DecodeBufferPostRp2(byte *obuf, byte *ibuf, int ibsz)
{
	static PostRp2Huff_DecState *st_ctx;
	int sz;

	if(!st_ctx)
	{
		st_ctx=malloc(sizeof(PostRp2Huff_DecState));
		memset(st_ctx, 0, sizeof(PostRp2Huff_DecState));
	}

//	memset(st_ctx, 0, sizeof(PostRp2Huff_DecState));

	sz=PostRp2Huff_UnpackRp2BlobCtx(st_ctx, obuf, ibuf);
	return(sz);
}

int PostRp2Huff_CheckPostRp2Blob(byte *ibuf)
{
	if((ibuf[0]==0x20) && (ibuf[1]==0x00))
		return(1);
	return(0);
}
