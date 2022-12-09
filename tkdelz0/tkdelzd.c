typedef struct TkDeLz_DecState_s TkDeLz_DecState;

struct TkDeLz_DecState_s {
	u32 magic1;

	TkDeLz_BihState *bih_t;
	TkDeLz_BihState *bih_l;
	TkDeLz_BihState *bih_d;
	TkDeLz_BihState *bih_e;

	TkDeLz_BihState t_bih_t;
	TkDeLz_BihState t_bih_l;
	TkDeLz_BihState t_bih_d;
	TkDeLz_BihState t_bih_e;

	byte *bbuf_t;
	byte *bbuf_l;
	byte *bbuf_d;
	byte *bbuf_e;

	byte *cs;

	byte *ct;		//output buffer
	byte *oct;		//prior output buffer (delta)

	u32 magic2;
};

TkDeLz_DecState *TkDeLz_DecAllocContext()
{
	TkDeLz_DecState *tmp;
	
	tmp=malloc(sizeof(TkDeLz_DecState));
	memset(tmp, 0, sizeof(TkDeLz_DecState));
	
	tmp->bih_t=&tmp->t_bih_t;
	tmp->bih_l=&tmp->t_bih_l;
	tmp->bih_d=&tmp->t_bih_d;
	tmp->bih_e=&tmp->t_bih_e;
	return(tmp);
}

int TkDeLz_SetupDecChunk(TkDeLz_DecState *ctx, byte *buf)
{
//	byte *cs;
	int btag;
	
	if((ctx->magic1!=0x57681324) || (ctx->magic2!=0x57681324))
	{
		memset(ctx, 0, sizeof(TkDeLz_DecState));
		ctx->magic1=0x57681324;
		ctx->magic2=0x57681324;
	}

	ctx->cs=buf;

	btag=tkdelz_getu16le(buf);
//	ct+=2;

	ctx->bih_t=&ctx->t_bih_t;
	ctx->bih_l=&ctx->t_bih_l;
	ctx->bih_d=&ctx->t_bih_d;
	ctx->bih_e=&ctx->t_bih_e;

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

void TkDeLz_MatchLzCpy(byte *dst, byte *src, int sz)
{
	byte *cs, *cse, *ct;
	u64 v0, v1;
	int d;

	if(dst==src)
		return;

	if(dst<src)
	{
		if(sz<=16)
		{
//			v0=((u64 *)src)[0];
//			v1=((u64 *)src)[1];
//			((u64 *)dst)[0]=v0;
//			((u64 *)dst)[1]=v1;

			v0=tkdelz_getu64le(src+0);
			v1=tkdelz_getu64le(src+8);
			tkdelz_setu64le(dst+0, v0);
			tkdelz_setu64le(dst+8, v1);
		}else
		{
			memmove(dst, src, sz);
		}
		return;
	}

	d=dst-src;
	if(d>=sz)
	{
		if(sz<=16)
		{
//			v0=((u64 *)src)[0];
//			v1=((u64 *)src)[1];
//			((u64 *)dst)[0]=v0;
//			((u64 *)dst)[1]=v1;

			v0=tkdelz_getu64le(src+0);
			v1=tkdelz_getu64le(src+8);
			tkdelz_setu64le(dst+0, v0);
			tkdelz_setu64le(dst+8, v1);
		}else
		{
			memcpy(dst, src, sz);
		}
		return;
	}
	
	cs=src; ct=dst;	cse=cs+sz;
	if(d>=16)
	{
		while(cs<cse)
		{
//			v0=((u64 *)cs)[0];
//			v1=((u64 *)cs)[1];
//			((u64 *)ct)[0]=v0;
//			((u64 *)ct)[1]=v1;

			v0=tkdelz_getu64le(cs+0);
			v1=tkdelz_getu64le(cs+8);
			tkdelz_setu64le(ct+0, v0);
			tkdelz_setu64le(ct+8, v1);

			cs+=16;	ct+=16;
		}
		return;
	}

	if(d>=8)
	{
		while(cs<cse)
		{
//			v0=((u64 *)cs)[0];
//			((u64 *)ct)[0]=v0;
//			v1=((u64 *)cs)[1];
//			((u64 *)ct)[1]=v1;

			v0=tkdelz_getu64le(cs+0);
			tkdelz_setu64le(ct+0, v0);
			v1=tkdelz_getu64le(cs+8);
			tkdelz_setu64le(ct+8, v1);

			cs+=16;	ct+=16;
		}
		return;
	}
	
	if(d==1)
	{
		memset(ct, *cs, sz);
		return;
	}

	if(d==2)
	{
		v0=tkdelz_getu16le(src);
		v0|=v0<<16;
		v0|=v0<<32;
		while(cs<cse)
		{
			tkdelz_setu64le(ct+0, v0);
			tkdelz_setu64le(ct+8, v0);
			cs+=16;	ct+=16;
		}
		return;
	}

	if(d==4)
	{
		v0=tkdelz_getu32le(src);
		v0|=v0<<32;
		while(cs<cse)
		{
			tkdelz_setu64le(ct+0, v0);
			tkdelz_setu64le(ct+8, v0);
			cs+=16;	ct+=16;
		}
		return;
	}

	while(cs<cse)
		*ct++=*cs++;
}

int TkDeLz_DecChunkLz(TkDeLz_DecState *ctx)
{
	byte *cs_t, *cs_l, *cs_d, *cs_e;
	byte *ct, *oct;
	u64 v0, v1;
	int tg, ll, ld, lr, ld2;
	int lml, lmd, spfl;
	
	cs_t=ctx->bbuf_t;
	cs_l=ctx->bbuf_l;
	cs_d=ctx->bbuf_d;
	cs_e=ctx->bbuf_e;
	ct=ctx->ct;
	oct=ctx->oct;
	lml=0;
	lmd=0;
	
	while(1)
	{
		tg=*cs_t++;
		ll=(tg&31)+3;
		lr=(tg>>5)&7;
		spfl=0;
		
		if(lr==7)
		{
			lr=*cs_d++;
			if(lr&0x80)
			{
				if(lr&0x40)
				{
					lr=((lr&0x1F)<<8)|(*cs_e++);
					lr=(lr<<8)|(*cs_e++);
				}else
				{
					lr=((lr&0x3F)<<8)|(*cs_e++);
				}
			}
#if 1
			if(lr<8)
			{
				if(!lr)
				{
					break;
				}
			}
#endif
		}

		if(ll==34)
		{
			ll=*cs_d++;
			if(ll&0x80)
			{
				if(ll&0x40)
				{
					ll=((ll&0x1F)<<8)|(*cs_e++);
					ll=(ll<<8)|(*cs_e++);
				}else
				{
					ll=((ll&0x3F)<<8)|(*cs_e++);
				}
			}
			
			if(ll<34)
			{
				if(!ll)
					{ spfl|=3; }
				else if(ll==1)
					{ ll=0; }
				else if(ll==2)
					{ ll=lml; spfl|=2; }
				else
					{ spfl|=2; }
			}
		}

		if(spfl&2)
		{
			ld=lmd;
		}else
		{
			ld=*cs_d++;
			if(ld&0x80)
			{
				if(ld&0x40)
				{
					ld=((ld&0x1F)<<8)|(*cs_e++);
					ld=(ld<<8)|(*cs_e++);
				}else
				{
					ld=((ld&0x3F)<<8)|(*cs_e++);
				}
			}
		}

		if(lr)
		{
			if(lr<=16)
			{
				v0=tkdelz_getu64le(cs_l+0);
				v1=tkdelz_getu64le(cs_l+8);
				tkdelz_setu64le(ct+0, v0);
				tkdelz_setu64le(ct+8, v1);
			}else
			{
				memcpy(ct, cs_l, lr);
			}
			cs_l+=lr;
			ct+=lr;
		}
		
//		if(!ll)
		if(spfl&1)
			break;

		lml=ll;
		lmd=ld;

		if(oct)
		{
			ld2=(ld>>1)^(((s32)(ld<<31))>>31);
			if((oct!=ct) && (ld2<=0))
			{
				ld2=-ld2;
				ld2=(ld2>>1)^(((s32)(ld2<<31))>>31);
				TkDeLz_MatchLzCpy(ct, oct+ld2, ll);
				ct+=ll; oct+=ll;
			}else
			{
				TkDeLz_MatchLzCpy(ct, ct-ld2, ll);
				ct+=ll; oct+=ll;
			}
		}else
		{
			TkDeLz_MatchLzCpy(ct, ct-ld, ll);
			ct+=ll;
		}
	}
	
	ctx->ct=ct;
	ctx->oct=oct;
	return(0);
}

int TkDeLz_DecChunk(TkDeLz_DecState *ctx)
{
	byte *cs;
	int btag;
	int i, j, k;
	
	cs=ctx->cs;
	btag=tkdelz_getu16le(cs);
	cs+=2;

	if((btag>>12)==0)
	{
		k=tkdelz_getu32le(cs);
		k&=0x00FFFFFF;
		cs+=3;

		memcpy(ctx->ct, cs, k);
		ctx->ct+=k;
		cs+=k;

		ctx->cs=cs;
		return(0);
	}
	
	if(	((btag>>12)==1) ||
		((btag>>12)==2) ||
		((btag>>12)==3))
	{
		cs=TkDeLz_DecodeBihChunk(ctx->bih_t, cs);
		cs=TkDeLz_DecodeBihChunk(ctx->bih_l, cs);
		cs=TkDeLz_DecodeBihChunk(ctx->bih_d, cs);
		cs=TkDeLz_DecodeBihChunk(ctx->bih_e, cs);
		ctx->cs=cs;

		TkDeLz_DecChunkLz(ctx);
		return(0);
	}

	return(0);
}

int TkDeLz_DecodeBuffer(TkDeLz_DecState *ctx,
	byte *dst, byte *src, int dsz, int ssz)
{
	int btag;

	TkDeLz_SetupDecChunk(ctx, src);

	ctx->cs=src;
	ctx->ct=dst;
	
	btag=tkdelz_getu16le(ctx->cs);
	while(btag)
	{
		TkDeLz_DecChunk(ctx);
		btag=tkdelz_getu16le(ctx->cs);
	}
	
	return(ctx->ct-dst);
}
