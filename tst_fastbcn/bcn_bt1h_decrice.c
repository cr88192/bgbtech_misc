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

int BTIC1H_Rice_NextByte(BTIC1H_Context *ctx)
{
	int i, j;

	i=*ctx->bs_cs++;
	return(i);
}

int BTIC1H_Rice_SetupRead(BTIC1H_Context *ctx, byte *buf, int szbuf)
{
	ctx->bs_pos=0;

	ctx->bs_cs=buf;
	ctx->bs_cse=buf+szbuf;
	
	ctx->bs_win=BTIC1H_Rice_NextByte(ctx);
	ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
	ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
	ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
	return(0);
}

int BTIC1H_Rice_ReadBit(BTIC1H_Context *ctx)
{
	int i;

	i=(ctx->bs_win>>(31-ctx->bs_pos))&1;
	ctx->bs_pos++;
	if(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
		ctx->bs_pos-=8;
	}
	return(i);
}

int BTIC1H_Rice_ReadNBits(BTIC1H_Context *ctx, int n)
{
	int i, j, k;

#if 0
	if(n<=0)
	{
		if(!n)
			return(0);
		return(-1);
	}
	if(n>16)
		return(-1);
#endif

//	if(!n)
//		return(0);

#if 0
	j=ctx->bs_pos;	k=ctx->bs_win;
	i=(k>>(32-n-j))&((1<<n)-1);
	j+=n;
	if(j>=8)
	{
		while(j>=8)
		{
			k=(k<<8)|BTIC1H_Rice_NextByte(ctx);
			j-=8;
		}
		ctx->bs_win=k;
	}
	ctx->bs_pos=j;
#endif

#if 1
	i=(ctx->bs_win>>(32-n-ctx->bs_pos))&((1<<n)-1);
	ctx->bs_pos+=n;
	while(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
		ctx->bs_pos-=8;
	}
#endif
	return(i);
}

int BTIC1H_Rice_ReadNBitsNoMask(BTIC1H_Context *ctx, int n)
{
	int i, j, k;

#if 1
	j=ctx->bs_pos+n; k=ctx->bs_win;
	i=k>>(32-j);
	if(j>=8)
	{
		k=(k<<8)|(*ctx->bs_cs++); j-=8;
		if(j>=8)
			{ k=(k<<8)|(*ctx->bs_cs++); j-=8; }
		ctx->bs_win=k;
	}
	ctx->bs_pos=j;
	return(i);
#endif

#if 0
	ctx->bs_pos+=n;
	i=ctx->bs_win>>(32-ctx->bs_pos);
//	i=ctx->bs_win>>(32-n-ctx->bs_pos);
//	ctx->bs_pos+=n;
	while(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
		ctx->bs_pos-=8;
	}
	return(i);
#endif
}

int BTIC1H_Rice_Read4Bits(BTIC1H_Context *ctx)
	{ return(BTIC1H_Rice_ReadNBits(ctx, 4)); }

//int BTIC1H_Rice_Read8Bits(BTIC1H_Context *ctx)
//	{ return(BTIC1H_Rice_ReadNBits(ctx, 8)); }

//int BTIC1H_Rice_Read16Bits(BTIC1H_Context *ctx)
//	{ return(BTIC1H_Rice_ReadNBits(ctx, 16)); }

int BTIC1H_Rice_Read8Bits(BTIC1H_Context *ctx)
{
	int i;

	i=(ctx->bs_win>>(24-ctx->bs_pos))&255;
	ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
	return(i);
}

int BTIC1H_Rice_Read16Bits(BTIC1H_Context *ctx)
{
	int i;

	i=(ctx->bs_win>>(16-ctx->bs_pos))&65535;
	ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
	ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
	return(i);
}

#if 0
int BTIC1H_Rice_Read16Bits(BTIC1H_Context *ctx)
{
	int i, j;
	i=BTIC1H_Rice_ReadNBits(ctx, 8);
	j=BTIC1H_Rice_ReadNBits(ctx, 8);
	return((i<<8)|j);
}
#endif

u32 BTIC1H_Rice_Read32Bits(BTIC1H_Context *ctx)
{
	int i, j;
//	i=BTIC1H_Rice_ReadNBits(ctx, 16);
//	j=BTIC1H_Rice_ReadNBits(ctx, 16);

	i=BTIC1H_Rice_Read16Bits(ctx);
	j=BTIC1H_Rice_Read16Bits(ctx);
	return((i<<16)|j);
}

u64 BTIC1H_Rice_Read48Bits(BTIC1H_Context *ctx)
{
	u64 i, j, k;
	i=BTIC1H_Rice_Read16Bits(ctx);
	j=BTIC1H_Rice_Read16Bits(ctx);
	k=BTIC1H_Rice_Read16Bits(ctx);
	return((i<<32)|(j<<16)|k);
}

void BTIC1H_Rice_SkipNBits(BTIC1H_Context *ctx, int n)
{
	ctx->bs_pos+=n;
	while(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
		ctx->bs_pos-=8;
	}
}

void BTIC1H_Rice_Skip8Bits(BTIC1H_Context *ctx)
{
	ctx->bs_win=(ctx->bs_win<<8)|BTIC1H_Rice_NextByte(ctx);
}

int BTIC1H_Rice_PeekWord(BTIC1H_Context *ctx)
{
	int i;
	i=(ctx->bs_win>>(16-ctx->bs_pos))&65535;
	return(i);
}

int BTIC1H_Rice_PeekByte(BTIC1H_Context *ctx)
{
	int i;
	i=(ctx->bs_win>>(24-ctx->bs_pos))&255;
	return(i);
}

int BTIC1H_Rice_PeekNibble(BTIC1H_Context *ctx)
{
	int i;
	i=(ctx->bs_win>>(28-ctx->bs_pos))&15;
	return(i);
}


static const byte btic1h_rice_ntab[16]={
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 2, 2, 3, 4};

static const byte btic1h_rice_ntab2[256]={
	0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,	/* 00-0F */
	0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,	/* 10-1F */
	0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,	/* 20-2F */
	0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,	/* 30-3F */
	0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,	/* 40-4F */
	0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,	/* 50-5F */
	0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,	/* 60-6F */
	0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,	/* 70-7F */
	1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,	/* 80-8F */
	1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,	/* 90-9F */
	1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,	/* A0-AF */
	1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1,	/* B0-BF */
	2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2,	/* C0-CF */
	2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2,	/* D0-DF */
	3, 3, 3, 3, 3, 3, 3, 3,  3, 3, 3, 3, 3, 3, 3, 3,	/* E0-EF */
	4, 4, 4, 4, 4, 4, 4, 4,  5, 5, 5, 5, 6, 6, 7, 8};	/* F0-FF */

int BTIC1H_Rice_ReadRiceQ(BTIC1H_Context *ctx)
{
	int i, j, k;

#if 0
	i=0;
	while(BTIC1H_Rice_ReadBit(ctx))
		i++;
	return(i);
#endif

	k=0;
	i=BTIC1H_Rice_PeekByte(ctx);
	if(i==0xFF)
	{
		k=0;
		while(i==0xFF)
		{
//			BTIC1H_Rice_ReadNBits(ctx, 8);
			BTIC1H_Rice_Skip8Bits(ctx);
			k+=8;
			i=BTIC1H_Rice_PeekByte(ctx);
		}
//		j=btic1h_rice_ntab[i>>4];
//		if(j>=4)
//			{ j+=btic1h_rice_ntab[i&15]; }
		j=btic1h_rice_ntab2[i];
//		BTIC1H_Rice_ReadNBits(ctx, j+1);
		BTIC1H_Rice_SkipNBits(ctx, j+1);
		i=k+j;
		return(i);
	}else
	{
//		j=btic1h_rice_ntab[i>>4];
//		if(j>=4)
//			{ j+=btic1h_rice_ntab[i&15]; }
		j=btic1h_rice_ntab2[i];
//		BTIC1H_Rice_ReadNBits(ctx, j+1);
		BTIC1H_Rice_SkipNBits(ctx, j+1);
		return(j);
	}
}

int BTIC1H_Rice_ReadRice(BTIC1H_Context *ctx, int k)
{
	int i, j;
	
	i=BTIC1H_Rice_ReadRiceQ(ctx);
	j=BTIC1H_Rice_ReadNBits(ctx, k);
	i=(i<<k)|j;
	return(i);
}

int BTIC1H_Rice_ReadAdRice2(BTIC1H_Context *ctx, int *rk)
{
	int i, j, k;
	
	k=*rk;
	i=BTIC1H_Rice_ReadRiceQ(ctx);
	j=BTIC1H_Rice_ReadNBits(ctx, k);
	j=(i<<k)|j;

#if 1
	if(i!=1)
	{
		if(i>1)
		{
			while(i>1)
				{ k++; i=i>>1; }
		}else
		{
			if(k>0)k--;
//			k--;
//			if(k<0)k=0;
		}
		*rk=k;
	}
#endif

	return(j);
}

#if 1
int BTIC1H_Rice_ReadAdRice(BTIC1H_Context *ctx, int *rk)
{
	int i, j, k, l;
	
	k=*rk;

	i=BTIC1H_Rice_PeekByte(ctx);
	if(i!=0xFF)
	{
		j=btic1h_rice_ntab2[i];
		
		l=j+k+1;
		if(l<=16)
		{
			i=BTIC1H_Rice_ReadNBits(ctx, l);
//			i=BTIC1H_Rice_ReadNBitsNoMask(ctx, l);
			i=(j<<k)|(i&((1<<k)-1));

			if(j!=1)
			{
				if(j>1)
				{
					while(j>1)
						{ k++; j=j>>1; }
				}else
				{
					if(k>0)k--;
				}
				*rk=k;
			}

			return(i);
		}
	}

	i=BTIC1H_Rice_ReadRiceQ(ctx);
	j=BTIC1H_Rice_ReadNBits(ctx, k);
	j=(i<<k)|j;

#if 1
	if(i!=1)
	{
		if(i>1)
		{
			while(i>1)
				{ k++; i=i>>1; }
		}else
		{
			if(k>0)k--;
		}
		*rk=k;
	}
#endif

	return(j);
}

#endif

int BTIC1H_Rice_ReadAdSRice(BTIC1H_Context *ctx, int *rk)
{
	int i;
	i=BTIC1H_Rice_ReadAdRice(ctx, rk);
	i=(i>>1)^((i<<31)>>31);
	return(i);
}
