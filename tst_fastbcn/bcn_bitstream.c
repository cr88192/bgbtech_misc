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

void BGBBTJ_BitsLE_Clear(BGBBTJ_BitStream *ctx)
{
	memset(ctx, 0, sizeof(BGBBTJ_BitStream));
}

void BGBBTJ_BitsLE_SetupWrite(BGBBTJ_BitStream *ctx, byte *ct, int sz)
{
	ctx->ct=ct;
	ctx->cte=ct+sz;
	ctx->win=0;
	ctx->pos=0;
}

void BGBBTJ_BitsLE_ClearSetupWrite(BGBBTJ_BitStream *ctx, byte *ct, int sz)
{
	BGBBTJ_BitsLE_Clear(ctx);
	BGBBTJ_BitsLE_SetupWrite(ctx, ct, sz);
}

void BGBBTJ_BitsLE_WriteBits(BGBBTJ_BitStream *ctx, int v, int n)
{
	v&=(1<<n)-1;
	ctx->win|=v<<(ctx->pos);
	ctx->pos+=n;
	while(ctx->pos>=8)
	{
		*ctx->ct++=(ctx->win)&255;
		ctx->win>>=8;
		ctx->pos-=8;
	}
}

void BGBBTJ_BitsLE_WriteBit(BGBBTJ_BitStream *ctx, int v)
	{ BGBBTJ_BitsLE_WriteBits(ctx, v, 1); }
void BGBBTJ_BitsLE_Write2Bits(BGBBTJ_BitStream *ctx, int v)
	{ BGBBTJ_BitsLE_WriteBits(ctx, v, 2); }
void BGBBTJ_BitsLE_Write3Bits(BGBBTJ_BitStream *ctx, int v)
	{ BGBBTJ_BitsLE_WriteBits(ctx, v, 3); }
void BGBBTJ_BitsLE_Write4Bits(BGBBTJ_BitStream *ctx, int v)
	{ BGBBTJ_BitsLE_WriteBits(ctx, v, 4); }
void BGBBTJ_BitsLE_Write5Bits(BGBBTJ_BitStream *ctx, int v)
	{ BGBBTJ_BitsLE_WriteBits(ctx, v, 5); }
void BGBBTJ_BitsLE_Write6Bits(BGBBTJ_BitStream *ctx, int v)
	{ BGBBTJ_BitsLE_WriteBits(ctx, v, 6); }
void BGBBTJ_BitsLE_Write7Bits(BGBBTJ_BitStream *ctx, int v)
	{ BGBBTJ_BitsLE_WriteBits(ctx, v, 7); }
// void BGBBTJ_BitsLE_Write8Bits(BGBBTJ_BitStream *ctx, int v)
//	{ BGBBTJ_BitsLE_WriteBits(ctx, v, 8); }

void BGBBTJ_BitsLE_Write12Bits(BGBBTJ_BitStream *ctx, int v)
	{ BGBBTJ_BitsLE_WriteBits(ctx, v, 12); }
// void BGBBTJ_BitsLE_Write16Bits(BGBBTJ_BitStream *ctx, int v)
//	{ BGBBTJ_BitsLE_WriteBits(ctx, v, 16); }

void BGBBTJ_BitsLE_Write8Bits(BGBBTJ_BitStream *ctx, int v)
{
	ctx->win|=(v&255)<<(ctx->pos);
	*ctx->ct++=ctx->win; ctx->win>>=8;
}

void BGBBTJ_BitsLE_Write16Bits(BGBBTJ_BitStream *ctx, int v)
{
	ctx->win|=(v&65535)<<(ctx->pos);
	*ctx->ct++=ctx->win; ctx->win>>=8;
	*ctx->ct++=ctx->win; ctx->win>>=8;
}

void BGBBTJ_BitsLE_FlushBits(BGBBTJ_BitStream *ctx)
{
	while(ctx->pos>0)
	{
		*ctx->ct++=(ctx->win)&255;
		ctx->win>>=8;
		ctx->pos-=8;
	}
}


void BGBBTJ_BitsLE_SetupRead(BGBBTJ_BitStream *ctx, byte *cs, int sz)
{
	ctx->cs=cs;
	ctx->cse=cs+sz;
//	ctx->win=0;
	ctx->pos=0;
	
	ctx->win=ctx->cs[0] | (ctx->cs[1]<<8) |
		(ctx->cs[2]<<16) | (ctx->cs[3]<<24);
	ctx->cs+=4;
}

void BGBBTJ_BitsLE_ClearSetupRead(BGBBTJ_BitStream *ctx, byte *ct, int sz)
{
	BGBBTJ_BitsLE_Clear(ctx);
	BGBBTJ_BitsLE_SetupRead(ctx, ct, sz);
}

int BGBBTJ_BitsLE_ReadBits(BGBBTJ_BitStream *ctx, int n)
{
	int v;
	
	v=(ctx->win>>ctx->pos)&((1<<n)-1);
	ctx->pos+=n;
	while(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(v);
}

int BGBBTJ_BitsLE_ReadBit(BGBBTJ_BitStream *ctx)
{
	int v;
	
	v=(ctx->win>>ctx->pos)&1;
	ctx->pos++;
	if(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(v);
}

int BGBBTJ_BitsLE_Read2Bits(BGBBTJ_BitStream *ctx)
{
	int v;
	
	v=(ctx->win>>ctx->pos)&3;
	ctx->pos+=2;
	if(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(v);
}

int BGBBTJ_BitsLE_Read7Bits(BGBBTJ_BitStream *ctx)
{
	int v;
	
	v=(ctx->win>>ctx->pos)&127;
	ctx->pos+=7;
	if(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(v);
}

int BGBBTJ_BitsLE_Read8Bits(BGBBTJ_BitStream *ctx)
{
	int v;
	
	v=(ctx->win>>ctx->pos)&255;
	ctx->pos+=8;
	while(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(v);
}

//int BGBBTJ_BitsLE_ReadBit(BGBBTJ_BitStream *ctx)
//	{ return(BGBBTJ_BitsLE_ReadBits(ctx, 1)); }
//int BGBBTJ_BitsLE_Read2Bits(BGBBTJ_BitStream *ctx)
//	{ return(BGBBTJ_BitsLE_ReadBits(ctx, 2)); }
int BGBBTJ_BitsLE_Read3Bits(BGBBTJ_BitStream *ctx)
	{ return(BGBBTJ_BitsLE_ReadBits(ctx, 3)); }
int BGBBTJ_BitsLE_Read4Bits(BGBBTJ_BitStream *ctx)
	{ return(BGBBTJ_BitsLE_ReadBits(ctx, 4)); }
int BGBBTJ_BitsLE_Read5Bits(BGBBTJ_BitStream *ctx)
	{ return(BGBBTJ_BitsLE_ReadBits(ctx, 5)); }
int BGBBTJ_BitsLE_Read6Bits(BGBBTJ_BitStream *ctx)
	{ return(BGBBTJ_BitsLE_ReadBits(ctx, 6)); }
//int BGBBTJ_BitsLE_Read7Bits(BGBBTJ_BitStream *ctx)
//	{ return(BGBBTJ_BitsLE_ReadBits(ctx, 7)); }
//int BGBBTJ_BitsLE_Read8Bits(BGBBTJ_BitStream *ctx)
//	{ return(BGBBTJ_BitsLE_ReadBits(ctx, 8)); }

int BGBBTJ_BitsLE_Read14Bits(BGBBTJ_BitStream *ctx)
	{ return(BGBBTJ_BitsLE_ReadBits(ctx, 14)); }
int BGBBTJ_BitsLE_Read16Bits(BGBBTJ_BitStream *ctx)
	{ return(BGBBTJ_BitsLE_ReadBits(ctx, 16)); }

int BGBBTJ_BitsLE_Peek8Bits(BGBBTJ_BitStream *ctx)
{
	int v;
	v=(ctx->win>>ctx->pos)&255;
	return(v);
}
