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

void BTIC1H_Rice_SetupWrite(BTIC1H_Context *ctx, byte *obuf, int osz)
{
	ctx->bs_win=0;
	ctx->bs_pos=0;
	ctx->bs_bits=0;

	ctx->bs_ct=obuf;
	ctx->bs_cte=obuf+osz;
}

void BTIC1H_Rice_EmitByte(BTIC1H_Context *ctx, int i)
{
	*ctx->bs_ct++=i;
}

void BTIC1H_Rice_WriteBit(BTIC1H_Context *ctx, int i)
{
	ctx->bs_bits++;

	ctx->bs_win|=(i&1)<<(31-ctx->bs_pos);
	ctx->bs_pos++;
	if(ctx->bs_pos>=8)
	{
		i=(ctx->bs_win>>24)&0xFF;
		BTIC1H_Rice_EmitByte(ctx, i);
		ctx->bs_win<<=8;
		ctx->bs_pos-=8;
	}
}

void BTIC1H_Rice_WriteNBits(BTIC1H_Context *ctx, int v, int n)
{
	int i, j, k;

#if 0
	if(n<=0)
	{
		if(!n)
			return;
		return;
	}
	if(n>16)
	{
		*(int *)-1=-1;
	}
#endif

	ctx->bs_bits+=n;

	v&=(1<<n)-1;

	j=ctx->bs_pos+n;
	k=ctx->bs_win|(v<<(32-j));
#if 1
	if(j>=8)
	{
		*ctx->bs_ct++=(k>>24); k<<=8; j-=8;
		if(j>=8)
			{ *ctx->bs_ct++=(k>>24); k<<=8; j-=8; }
	}
#endif
#if 0
	while(j>=8)
	{
		i=(k>>24)&0xFF;
//		*ctx->bs_ct++=i;
		BTIC1H_Rice_EmitByte(ctx, i);
		k<<=8;
		j-=8;
	}
#endif
	ctx->bs_pos=j;
	ctx->bs_win=k;
}

void BTIC1H_Rice_WriteNBitsH(BTIC1H_Context *ctx, u32 i, int n)
{
	if(n>16)
	{
		BTIC1H_Rice_WriteNBits(ctx, i>>(n-16), 16);
//		i=i>>16;
		n-=16;
	}
	BTIC1H_Rice_WriteNBits(ctx, i, n);
}

void BTIC1H_Rice_WriteNBitsL(BTIC1H_Context *ctx, s64 i, int n)
{
	while(n>16)
	{
		BTIC1H_Rice_WriteNBits(ctx, i>>(n-16), 16);
//		BTIC1H_Rice_WriteNBits(ctx, i&65535, 16);
//		i=i>>16;
		n-=16;
	}
	BTIC1H_Rice_WriteNBits(ctx, i, n);
}

void BTIC1H_Rice_Write2Bits(BTIC1H_Context *ctx, int i)
	{ BTIC1H_Rice_WriteNBits(ctx, i, 2); }
void BTIC1H_Rice_Write3Bits(BTIC1H_Context *ctx, int i)
	{ BTIC1H_Rice_WriteNBits(ctx, i, 3); }
void BTIC1H_Rice_Write4Bits(BTIC1H_Context *ctx, int i)
	{ BTIC1H_Rice_WriteNBits(ctx, i, 4); }

//void BTIC1H_Rice_Write8Bits(BTIC1H_Context *ctx, int i)
//	{ BTIC1H_Rice_WriteNBits(ctx, i, 8); }

void BTIC1H_Rice_Write8Bits(BTIC1H_Context *ctx, int v)
{
	int i, j, k;

	ctx->bs_bits+=8;
	k=ctx->bs_win|((v&255)<<(24-ctx->bs_pos));
	*ctx->bs_ct++=k>>24;
	ctx->bs_win=k<<8;
}

void BTIC1H_Rice_Write16Bits(BTIC1H_Context *ctx, int v)
{
	int i, j, k;

	ctx->bs_bits+=16;
	k=ctx->bs_win|((v&65535)<<(16-ctx->bs_pos));
	*ctx->bs_ct++=k>>24;
	*ctx->bs_ct++=k>>16;
	ctx->bs_win=k<<16;
}

void BTIC1H_Rice_WriteRice(BTIC1H_Context *ctx, int v, int k)
{
	int j;
	
	j=v>>k;
	while(j--)
	 { BTIC1H_Rice_WriteBit(ctx, 1); }
	BTIC1H_Rice_WriteBit(ctx, 0);
	j=v&((1<<k)-1);
	BTIC1H_Rice_WriteNBits(ctx, j, k);
}

int BTIC1H_Rice_CountWriteRice(BTIC1H_Context *ctx, int v, int k)
{
	int j;
	j=v>>k;
	return(j+k);
}

void BTIC1H_Rice_WriteSRice(BTIC1H_Context *ctx, int v, int k)
{
	BTIC1H_Rice_WriteRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), k);
}

int BTIC1H_Rice_CountWriteSRice(BTIC1H_Context *ctx, int v, int k)
{
	return(BTIC1H_Rice_CountWriteRice(ctx,
		(v>=0)?(v<<1):(((-v)<<1)-1), k));
}

#if 0
void BTIC1H_Rice_WriteExpRice(BTIC1H_Context *ctx, int v, int k)
{
	int i, j;
	
	if(v<=0)
	{
		BTIC1H_Rice_WriteRice(ctx, 0, k);
		return;
	}
	
	i=v; j=0;
	while(i>0) { i>>=1; j++; }
	BTIC1H_Rice_WriteRice(ctx, j, k);
	j=v&((1<<k)-1);
	BTIC1H_Rice_WriteNBitsL(ctx, j, i);
}

void BTIC1H_Rice_WriteExpSRice(BTIC1H_Context *ctx, int v, int k)
{
	BTIC1H_Rice_WriteExpRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), k);
}
#endif

void BTIC1H_Rice_WriteAdRice(BTIC1H_Context *ctx, int v, int *rk)
{
	int p, b, n, bp, bw;
	int i, j, k;
	
	k=*rk;

#if 0
	if(!v)
	{
		BTIC1H_Rice_WriteNBits(ctx, 0, k+1);
		if(k>0)k--;
		*rk=k;
		return;
	}
#endif
	
	p=v>>k;

#if 1
	i=p+k+1;
	if(i<=16)
	{
		j=v&((1<<k)-1);
		BTIC1H_Rice_WriteNBits(ctx, (0xFFFE<<k)|j, i);

#if 0
		n=i;

//		ctx->bs_bits+=n;

		b=v&((1<<k)-1);
		b=((0xFFFE<<k)|b)&((1<<n)-1);

		bp=ctx->bs_pos+n;
		bw=ctx->bs_win|(b<<(32-bp));
		if(bp>=8)
		{
			*ctx->bs_ct++=(bw>>24);
			bw<<=8;		bp-=8;

			while(bp>=8)
			{
				*ctx->bs_ct++=(bw>>24);
				bw<<=8;		bp-=8;
			}
		}
		ctx->bs_pos=bp;
		ctx->bs_win=bw;
#endif

		if(p!=1)
		{
			if(!p)
				{ if(k>0)k--; }
			else if(p>1)
			{
				j=p;
				while(j>1)
					{ k++; j>>=1; }
			}
			*rk=k;
		}
		return;
	}
#endif

	j=p;

#if 1
	while(j>=16)
		{ BTIC1H_Rice_WriteNBits(ctx, 0xFFFF, 16); j-=16; }
	BTIC1H_Rice_WriteNBits(ctx, 0xFFFE, j+1);
#endif

#if 0
	while(j--)
		{ BTIC1H_Rice_WriteBit(ctx, 1); }
	BTIC1H_Rice_WriteBit(ctx, 0);
#endif

//	j=v&((1<<k)-1);
//	BTIC1H_Rice_WriteNBits(ctx, j, k);
	BTIC1H_Rice_WriteNBits(ctx, v, k);

#if 1
	if(p!=1)
	{
		if(p>1)
		{
			j=p;
			while(j>1)
				{ k++; j>>=1; }
		}else
		{
			if(k>0)k--;
		}
		*rk=k;
	}
#endif

#if 0	
	if(!p && (k>0))k--;
	if(p>1)
	{
		j=p;
		while(j>1)
			{ k++; j>>=1; }
	}
	*rk=k;
#endif
}

int BTIC1H_Rice_CountWriteAdRice(BTIC1H_Context *ctx, int v, int *rk)
{
	int j, k, p, n;
	
	k=*rk;
	p=v>>k;
	n=p+k;

	if(!p && (k>0))k--;
	if(p>1)
	{
		j=p;
		while(j>1)
			{ k++; j>>=1; }
	}
	*rk=k;
	
	return(n);
}

void BTIC1H_Rice_WriteAdSRice(BTIC1H_Context *ctx, int v, int *rk)
{
	BTIC1H_Rice_WriteAdRice(ctx, (v<<1)^(v>>31), rk);
//	BTIC1H_Rice_WriteAdRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), rk);
}

void BTIC1H_Rice_WriteAdExpRice(BTIC1H_Context *ctx, int v, int *rk)
{
	int i, j, k;
	
	if(v<=0)
	{
		BTIC1H_Rice_WriteAdRice(ctx, 0, rk);
		return;
	}
	
	i=v; j=0; k=*rk;
	while(i>0) { i>>=1; j++; }
	BTIC1H_Rice_WriteAdRice(ctx, j, rk);
	j=v&((1<<k)-1);
	BTIC1H_Rice_WriteNBitsL(ctx, j, i);
}

void BTIC1H_Rice_WriteAdExpSRice(BTIC1H_Context *ctx, int v, int *rk)
{
	BTIC1H_Rice_WriteAdExpRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), rk);
}


#if 1

int BTIC1H_Rice_CountWriteAdRiceDc(BTIC1H_Context *ctx, int v, int *rk)
{
	int i, j, k, p, n;
	
	k=*rk;
	p=v>>k;
	n=p+k;

//	if(p>=8)
	if(p>=(8+k))
	{
		j=v; i=1;

//		while(j>=256)
//			{ j=j>>8; i++; }
//		n=8+9*i;

//		while(j>=128)
//			{ j=j>>7; i++; }
//		n=8+8*i;

//		while(j>=32)
//			{ j=j>>5; i++; }
//		n=8+6*i;

		while(j>=(1<<(5+k)))
			{ j=j>>(5+k); i++; }
		n=8+(6+k)*i;

//		while(j>=(1<<(4+(k>>1))))
//			{ j=j>>(4+(k>>1)); i++; }
//		n=8+(4+(k>>1))*i;

//		while(j>=(1<<(4+k)))
//			{ j=j>>(4+k); i++; }
//		n=8+(5+k)*i;

//		while(j>=8)
//			{ j=j>>3; i++; }
//		n=8+4*i;

//		n=8+8*i;
		k=k+2*i;
		if(k>15)k=15;
		*rk=k;
		return(n);
	}

	if(!p && (k>0))k--;
	if(p>1)
	{
		j=p;
		while(j>1)
			{ k++; j>>=1; }
//		k++;
	}
	*rk=k;
	
	return(n);
}

int BTIC1H_Rice_CountWriteAdSRiceDc(BTIC1H_Context *ctx, int v, int *rk)
	{ return(BTIC1H_Rice_CountWriteAdRiceDc(ctx, (v<<1)^(v>>31), rk)); }


void BTIC1H_Rice_WriteAdRiceDc(BTIC1H_Context *ctx, int v, int *rk)
{
	int p, b, n, bp, bw;
	int i, j, k;
	
	k=*rk;

	p=v>>k;

//	if(p>=8)
	if(p>=(8+k))
	{
		j=v; i=1;
//		while(j>=128)
//			{ j=j>>7; i++; }
//		while(j>=32)
//			{ j=j>>5; i++; }

		while(j>=(1<<(5+k)))
			{ j=j>>(5+k); i++; }

		BTIC1H_Rice_WriteNBitsH(ctx, 0xFFFFFFFEUL, 8+k+i);
		BTIC1H_Rice_WriteNBitsH(ctx, v, i*(5+k));

//		BTIC1H_Rice_WriteNBits(ctx, 0xFFFE, 8+i);
//		BTIC1H_Rice_WriteNBits(ctx, v, i*5);
		k+=2*i;
		if(k>15)k=15;
		*rk=k;
		return;
	}

#if 1
	i=p+k+1;
	if(i<=16)
	{
		j=v&((1<<k)-1);
		BTIC1H_Rice_WriteNBits(ctx, (0xFFFE<<k)|j, i);

		if(p!=1)
		{
			if(!p)
				{ if(k>0)k--; }
			else if(p>1)
			{
				j=p;
				while(j>1)
					{ k++; j>>=1; }
				if(k>15)k=15;
			}
			*rk=k;
		}
		return;
	}
#endif

	j=p;

#if 1
	while(j>=16)
		{ BTIC1H_Rice_WriteNBits(ctx, 0xFFFF, 16); j-=16; }
	BTIC1H_Rice_WriteNBits(ctx, 0xFFFE, j+1);
#endif

#if 0
	while(j--)
		{ BTIC1H_Rice_WriteBit(ctx, 1); }
	BTIC1H_Rice_WriteBit(ctx, 0);
#endif

//	j=v&((1<<k)-1);
//	BTIC1H_Rice_WriteNBits(ctx, j, k);
	BTIC1H_Rice_WriteNBits(ctx, v, k);

#if 1
	if(p!=1)
	{
		if(p>1)
		{
			j=p;
			while(j>1)
				{ k++; j>>=1; }
			if(k>15)k=15;
		}else
		{
			if(k>0)k--;
		}
		*rk=k;
	}
#endif
}

void BTIC1H_Rice_WriteAdSRiceDc(BTIC1H_Context *ctx, int v, int *rk)
{
	BTIC1H_Rice_WriteAdRiceDc(ctx, (v<<1)^(v>>31), rk);
}


#endif

void BTIC1H_Rice_FlushBits(BTIC1H_Context *ctx)
{
	int i;
	while(ctx->bs_pos>0)
	{
		i=(ctx->bs_win>>24)&0xFF;
//		*ctx->bs_ct++=i;
		BTIC1H_Rice_EmitByte(ctx, i);
		ctx->bs_win<<=8;
		ctx->bs_pos-=8;
	}
	ctx->bs_pos=0;
}
