/*
Flags, low 2 bits:
  00: Majority of 7, 64-bits encodes 8 data bits
  01: Majority of 3, 64-bits encodes 16 data bits
  10: Hamming(7,4) , 64-bits encodes 32 data bits

High 8 bits for each data word are used to encode the local mutation scale.
Remaining 56 bits are left to encode payload data.


 */

byte gann_maj7tab[128];
byte gann_maj8tab[256];
byte gann_gry2lin[256];

byte gann_maj3x2tab[64];
byte gann_ham7x4tab[128];

byte gann_nyb2ham7[16]={
  0x00, 0x07, 0x19, 0x1E,
  0x2A, 0x2D, 0x33, 0x34,
  0x4B, 0x4C, 0x52, 0x55,
  0x61, 0x66, 0x78, 0x7F
};

byte gann_skatab[64];

u64 gann_seed1;
u64 gann_seed2;
u64 gann_seed3;
u64 gann_seed4;

u64 gann_seed1b;
u64 gann_seed2b;

byte GANN_Ham7to4I(byte val)
{
	byte d1, d2, d3, d4;
	byte p1, p2, p3;
	byte q1, q2, q3;
	byte sn, v;
	
	v=val;
	while(1)
	{
		p1=(v>>0)&1;
		p2=(v>>1)&1;
		d1=(v>>2)&1;
		p3=(v>>3)&1;
		d2=(v>>4)&1;
		d3=(v>>5)&1;
		d4=(v>>6)&1;
		
		q1=d1^d2^d4;
		q2=d1^d3^d4;
		q3=d2^d3^d4;
		
		sn =(p1^q1)<<0;
		sn|=(p2^q2)<<1;
		sn|=(p3^q3)<<2;
		
		if(!sn)
			break;
		v^=1<<(sn-1);
	}
	return(d1|(d2<<1)|(d3<<2)|(d4<<3));
}

int GANN_InitMaj()
{
	int i, j, k, l;
	
	gann_seed1=0x5555;
	gann_seed2=0x1234;
	gann_seed3=0x5678;
	gann_seed4=0xABCD;

	for(i=0; i<128; i++)
		GANN_Rand16();

	gann_seed1b=gann_seed1^gann_seed2;
	gann_seed2b=gann_seed2^gann_seed3;

	for(i=0; i<128; i++)
	{
		k=0;
		for(j=0; j<8; j++)
			if(i&(1<<j))
				k++;
		gann_maj7tab[i]=k>3;
		gann_ham7x4tab[i]=GANN_Ham7to4I(i);
	}
	
	for(i=0; i<16; i++)
	{
		j=gann_nyb2ham7[i];
		k=gann_ham7x4tab[j];
		if(k!=i)
			{ __debugbreak(); }
	}

	for(i=0; i<256; i++)
	{
		k=0;
		for(j=0; j<8; j++)
			if(i&(1<<j))
				k++;
		if(k==4)
		{
			gann_maj8tab[i]=gann_maj7tab[i&127];
			continue;
		}
		gann_maj8tab[i]=k>4;
	}

	for(i=0; i<256; i++)
	{
		j=i^(i>>1);
		gann_gry2lin[j]=i;
	}

	for(i=0; i<64; i++)
	{
		k=0;
		if(	(((i>>0)&(i>>1))&1) |
			(((i>>0)&(i>>2))&1) |
			(((i>>1)&(i>>2))&1))
				k|=1;
		if(	(((i>>3)&(i>>4))&1) |
			(((i>>3)&(i>>5))&1) |
			(((i>>4)&(i>>5))&1))
				k|=2;
		gann_maj3x2tab[i]=k;
	}
	

	for(i=0; i<64; i++)
	{
		j=((i>>2)&15);
		l=(i&3);
		k=l;

		if(j==0x5)
		{
			if(l==0) k=1;
			if(l==1) k=1;
			if(l==2) k=0;
			if(l==3) k=5;
		}
		if(j==0x7)
		{
//			if((l==0) || (l==3)) k=5;
			if(l==0) k=5;
			if(l==1) k=1;
			if(l==2) k=0;
			if(l==3) k=5;
		}

		if(j==0x6)
		{
			if(l==0) k=3;
			if(l==1) k=5;
			if(l==2) k=4;
//			if(l==3) k=3;
			if(l==3) k=6;
		}

		if(j==0xA)
		{
			if(l==0) k=2;
			if(l==1) k=0;
			if(l==2) k=2;
			if(l==3) k=4;
		}
		if(j==0xB)
		{
//			if((l==0) || (l==3)) k=4;
			if(l==0) k=4;
			if(l==1) k=0;
			if(l==2) k=2;
			if(l==3) k=4;
		}

		if((j==0x0) || (j==0xF))
			k=0;
			
		gann_skatab[i]=k;
	}

	return(0);
}

byte GANN_Maj64to8(u64 maj)
{
	int i0, i1, i2, i3;
	int v;

	i0=gann_maj7tab[(maj>>49)&127];
	i1=gann_maj7tab[(maj>>42)&127];
	i2=gann_maj7tab[(maj>>35)&127];
	i3=gann_maj7tab[(maj>>28)&127];
	v=i0;			v=(v<<1)|i1;
	v=(v<<1)|i2;	v=(v<<1)|i3;
	i0=gann_maj7tab[(maj>>21)&127];
	i1=gann_maj7tab[(maj>>14)&127];
	i2=gann_maj7tab[(maj>> 7)&127];
	i3=gann_maj7tab[(maj>> 0)&127];
	v=(v<<1)|i0;	v=(v<<1)|i1;
	v=(v<<1)|i2;	v=(v<<1)|i3;

	return(v);
}

u16 GANN_Maj64to16(u64 maj)
{
	int i0, i1, i2, i3;
	int v;

	i0=gann_maj3x2tab[(maj>>42)&63];
	i1=gann_maj3x2tab[(maj>>36)&63];
	i2=gann_maj3x2tab[(maj>>30)&63];
	i3=gann_maj3x2tab[(maj>>24)&63];
	v=i0;			v=(v<<2)|i1;
	v=(v<<2)|i2;	v=(v<<2)|i3;
	i0=gann_maj3x2tab[(maj>>18)&63];
	i1=gann_maj3x2tab[(maj>>12)&63];
	i2=gann_maj3x2tab[(maj>> 6)&63];
	i3=gann_maj3x2tab[(maj>> 0)&63];
	v=(v<<2)|i0;	v=(v<<2)|i1;
	v=(v<<2)|i2;	v=(v<<2)|i3;

	return(v);
}

u32 GANN_Ham64to32(u64 maj)
{
	int i0, i1, i2, i3;
	u32 v;

	i0=gann_ham7x4tab[(maj>>49)&127];
	i1=gann_ham7x4tab[(maj>>42)&127];
	i2=gann_ham7x4tab[(maj>>35)&127];
	i3=gann_ham7x4tab[(maj>>28)&127];
	v=i0;			v=(v<<4)|i1;
	v=(v<<4)|i2;	v=(v<<4)|i3;
	i0=gann_ham7x4tab[(maj>>21)&127];
	i1=gann_ham7x4tab[(maj>>14)&127];
	i2=gann_ham7x4tab[(maj>> 7)&127];
	i3=gann_ham7x4tab[(maj>> 0)&127];
	v=(v<<4)|i0;	v=(v<<4)|i1;
	v=(v<<4)|i2;	v=(v<<4)|i3;

	return(v);
}

u64 GANN_ByteToMaj7(byte val)
{
	u64 v;
	int hi, lo, r;

	hi=0x7F; lo=0x00;
//	r=rand();
	r=GANN_Rand16B();
	v=r;

	r=r&7;
	hi&=~(1<<r);
	lo|= (1<<r);

	r=v>>4; r=r&7;
	hi&=~(1<<r);
	lo|= (1<<r);

	v=(v<<7)|((val&0x80)?hi:lo);
	v=(v<<7)|((val&0x40)?hi:lo);
	v=(v<<7)|((val&0x20)?hi:lo);
	v=(v<<7)|((val&0x10)?hi:lo);
	v=(v<<7)|((val&0x08)?hi:lo);
	v=(v<<7)|((val&0x04)?hi:lo);
	v=(v<<7)|((val&0x02)?hi:lo);
	v=(v<<7)|((val&0x01)?hi:lo);
	return(v);
}

u64 GANN_ByteToMaj3(byte val0, byte val1)
{
	u64 v;
	int hi, lo, r;

	hi=7; lo=0;
//	r=rand();
	r=GANN_Rand16B();
	v=r;

	r=r&3;
	hi&=~(1<<r);
	lo|= (1<<r);

	v=(v<<3)|((val1&0x80)?hi:lo);
	v=(v<<3)|((val1&0x40)?hi:lo);
	v=(v<<3)|((val1&0x20)?hi:lo);
	v=(v<<3)|((val1&0x10)?hi:lo);
	v=(v<<3)|((val1&0x08)?hi:lo);
	v=(v<<3)|((val1&0x04)?hi:lo);
	v=(v<<3)|((val1&0x02)?hi:lo);
	v=(v<<3)|((val1&0x01)?hi:lo);

	v=(v<<3)|((val0&0x80)?hi:lo);
	v=(v<<3)|((val0&0x40)?hi:lo);
	v=(v<<3)|((val0&0x20)?hi:lo);
	v=(v<<3)|((val0&0x10)?hi:lo);
	v=(v<<3)|((val0&0x08)?hi:lo);
	v=(v<<3)|((val0&0x04)?hi:lo);
	v=(v<<3)|((val0&0x02)?hi:lo);
	v=(v<<3)|((val0&0x01)?hi:lo);

	return(v);
}

u64 GANN_IntToMaj3(int val)
{
	return(GANN_ByteToMaj3(
		(val>> 0)&255, (val>> 8)&255));
}

u64 GANN_ByteToHam7(byte val0, byte val1, byte val2, byte val3)
{
	u64 v;
	int hi, lo, r;

	r=rand();
	v=r;

	v=(v<<7)|gann_nyb2ham7[(val3>>4)&15];
	v=(v<<7)|gann_nyb2ham7[(val3>>0)&15];
	v=(v<<7)|gann_nyb2ham7[(val2>>4)&15];
	v=(v<<7)|gann_nyb2ham7[(val2>>0)&15];
	v=(v<<7)|gann_nyb2ham7[(val1>>4)&15];
	v=(v<<7)|gann_nyb2ham7[(val1>>0)&15];
	v=(v<<7)|gann_nyb2ham7[(val0>>4)&15];
	v=(v<<7)|gann_nyb2ham7[(val0>>0)&15];

	return(v);
}

u64 GANN_IntToHam7(u32 val)
{
	return(GANN_ByteToHam7(
		(val>> 0)&255, (val>> 8)&255,
		(val>>16)&255, (val>>24)&255));
}

u16 GANN_Rand16()
{
	u64 v;
	gann_seed1=(gann_seed1<<1)^(gann_seed1>>5);
	gann_seed2=(gann_seed2<<1)^(gann_seed2>>11);
	gann_seed3=(gann_seed3<<1)^(gann_seed3>>17);
	gann_seed4=(gann_seed4<<1)^(~(gann_seed4>>29));
	gann_seed1^=gann_seed2>>13;
	gann_seed2^=gann_seed3>>23;
	gann_seed3^=gann_seed4>>27;
	gann_seed4^=gann_seed1>>19;

	v=gann_seed1^(gann_seed2>>3)^(gann_seed3>>7);
	return(v&65535);
}

u64 GANN_Rand64()
{
	u64 v;
	v=        GANN_Rand16();
	v=(v<<16)|GANN_Rand16();
	v=(v<<16)|GANN_Rand16();
	v=(v<<16)|GANN_Rand16();
	return(v);
}

u16 GANN_Rand16B()
{
	u64 v;
	gann_seed1b=(gann_seed1b<<1)^(gann_seed1b>>7);
	gann_seed2b=(gann_seed2b<<1)^(gann_seed2b>>11);
	gann_seed1b^=gann_seed2b>>13;
	gann_seed2b^=gann_seed1b>>23;
	v=gann_seed1b^gann_seed2b;
	v^=(v>>31);
	return(v&65535);
}

u64 GANN_Rand64B()
{
	u64 v;
//	v=        GANN_Rand16B();
//	v=(v<<16)|GANN_Rand16B();
//	v=(v<<16)|GANN_Rand16B();
//	v=(v<<16)|GANN_Rand16B();

	gann_seed1b=(gann_seed1b<<1)^(gann_seed1b>>7);
//	gann_seed2b=(gann_seed2b<<1)^(gann_seed2b>>11);
	gann_seed2b=(gann_seed2b>>1)^(gann_seed2b<<41);
	gann_seed1b^=gann_seed2b>>13;
	gann_seed2b^=gann_seed1b>>23;
	v=gann_seed1b^gann_seed2b;

	return(v);
}

u64 GANN_MutateMaskExp3X(int ma)
{
	u64 mc;
	int i, j, k;
	
	mc=0;
	for(i=0; i<16; i++)
	{
		if(ma&(1<<i))
			mc|=7ULL<<(i*3);
	}
	return(mc);
}

u64 GANN_MutateMaskExp7X(int ma)
{
	u64 mc;
	int i, j, k;
	
	mc=0;
	for(i=0; i<8; i++)
	{
		if(ma&(1<<i))
			mc|=127ULL<<(i*7);
	}
	return(mc);
}

u64 GANN_MutateIncr(GANN_Context *ctx, u64 va, int bit)
{
	u64 vb, vc, mv;
	u32 v, v0, v1, v2;
	int fl;

	fl=ctx->flag;

	if((bit&7)==7)
	{
		vc=va^(1ULL<<bit);
		return(vc);
	}

	if(fl&2)
	{
		v0=GANN_Ham64to32(va);
		v1=v0+(1<<(bit&31));

		if(	((v1&0x0000007FU)<(v0&0x0000007FU)) ||
			((v1&0x00007F00U)<(v0&0x00007F00U)) ||
			((v1&0x007F0000U)<(v0&0x007F0000U)) ||
			((v1&0x7F000000U)<(v0&0x7F000000U))	)
		{
			vc=va^(1ULL<<bit);
			return(vc);
		}

		vb=GANN_IntToHam7(v1);
		mv=0x00FFFFFFFFFFFFFFULL;
		vc=(vb&mv)|(va&(~mv));
		return(vc);
	}

	if(fl&1)
	{
		v0=GANN_Maj64to16(va);
		v1=v0+(1<<(bit&15));

		if(	((v1&0x007F)<(v0&0x007F)) ||
			((v1&0x7F00)<(v0&0x7F00)))
		{
			vc=va^(1ULL<<bit);
			return(vc);
		}

		vb=GANN_IntToMaj3(v1);
		v2=v0^v1;
		mv=GANN_MutateMaskExp3X(v2);
		vc=(vb&mv)|(va&(~mv));
		return(vc);
	}

	v0=GANN_Maj64to8(va);
	v1=v0+(1<<(bit&7));

	if((v1&0x7F)<(v0&0x7F))
	{
		vc=va^(1ULL<<bit);
		return(vc);
	}
	
	vb=GANN_ByteToMaj7(v1);
	v2=v0^v1;
	mv=GANN_MutateMaskExp7X(v2);
	vc=(vb&mv)|(va&(~mv));
	return(vc);
}

u64 GANN_MutateDecr(GANN_Context *ctx, u64 va, int bit)
{
	u64 vb, vc, mv;
	u32 v, v0, v1, v2;
	int fl;

	fl=ctx->flag;

	if((bit&7)==7)
	{
		vc=va^(1ULL<<bit);
		return(vc);
	}

	if(fl&2)
	{
		v0=GANN_Ham64to32(va);
		v1=v0-(1<<(bit&31));

		if(	((v1&0x0000007FU)>(v0&0x0000007FU)) ||
			((v1&0x00007F00U)>(v0&0x00007F00U)) ||
			((v1&0x007F0000U)>(v0&0x007F0000U)) ||
			((v1&0x7F000000U)>(v0&0x7F000000U))	)
		{
			vc=va^(1ULL<<bit);
			return(vc);
		}

		vb=GANN_IntToHam7(v1);
		mv=0x00FFFFFFFFFFFFFFULL;
		vc=(vb&mv)|(va&(~mv));
		return(vc);
	}

	if(fl&1)
	{
		v0=GANN_Maj64to16(va);
		v1=v0-(1<<(bit&15));

		if(	((v1&0x007F)>(v0&0x007F)) ||
			((v1&0x7F00)>(v0&0x7F00)))
		{
			vc=va^(1ULL<<bit);
			return(vc);
		}

		vb=GANN_IntToMaj3(v1);
		v2=v0^v1;
		mv=GANN_MutateMaskExp3X(v2);
		vc=(vb&mv)|(va&(~mv));
		return(vc);
	}

	v0=GANN_Maj64to8(va);
	v1=v0-(1<<(bit&7));
	
	if((v1&0x7F)>(v0&0x7F))
	{
		vc=va^(1ULL<<bit);
		return(vc);
	}
	
	vb=GANN_ByteToMaj7(v1);
	v2=v0^v1;
	mv=GANN_MutateMaskExp7X(v2);
	vc=(vb&mv)|(va&(~mv));
	return(vc);
}

u64 GANN_MutateInertia(GANN_Context *ctx, u64 va, u64 vb)
{
	u64 vc, mv;
	u32 v, v0, v1, v2, v3;
	int fl;

	fl=ctx->flag;

	if(fl&2)
	{
		v0=GANN_Ham64to32(va);
		v1=GANN_Ham64to32(vb);

		v2=GANN_AdjustInertiaVec8x8(v0, v1);

		vc=GANN_IntToHam7(v2);
		mv=0x00FFFFFFFFFFFFFFULL;
		vc=(vc&mv)|(va&(~mv));
		return(vc);
	}

	if(fl&1)
	{
		v0=GANN_Maj64to16(va);
		v1=GANN_Maj64to16(vb);

		v2=GANN_AdjustInertiaVec8x8(v0, v1);

		vc=GANN_IntToMaj3(v2);
		v3=v0^v2;
		mv=GANN_MutateMaskExp3X(v3);
		vc=(vc&mv)|(va&(~mv));
		return(vc);
	}

	v0=GANN_Maj64to8(va);
	v1=GANN_Maj64to8(vb);

//	v2=3*v0-2*v1;
	v2=5*v0-4*v1;
	if((v2==v0) && (v1!=v0))
		v2=2*v0-v1;

	if((v2^v0)&0x80)
		v2=v0;

//	v2=GANN_AdjustInertiaVec8x8(v0, v1);

	vc=GANN_ByteToMaj7(v2);
	v3=v0^v2;
	mv=GANN_MutateMaskExp7X(v3);
	vc=(vc&mv)|(va&(~mv));
	return(vc);
}

u64 GANN_BreedBits(GANN_Context *ctx, u64 va, u64 vb, int fl)
{
	static u64 nobimsk = 0xFFFEE00000E00000ULL;
	u64 vc, vm;
	int mrw, sk, ska, nobisc;
	int i, j, k;
	
	vm=GANN_Rand64B();
	vc=(va&vm)|(vb&(~vm));

	mrw=(fl>>8)&255;
	mrw=mrw*mrw;

//	k=gann_ham7x4tab[(vc>>56)&127];
//	j=k&3;
//	ska=(k>>2)&3;
//	j=gann_maj3x2tab[(vc>>56)&63];

	ska=(vc>>62)&3;
	j=gann_maj3x2tab[(vc>>56)&63];

//	mrw=(mrw*(j+1))>>2;
	j=j+1; j=j*j;
	mrw=(mrw*j)>>4;
	sk=ska;

	sk=gann_skatab[((fl>>2)&0x3C)|ska];

#if 0
	if(fl&2)
	{
		sk=0;
	}else
	{
		i=((fl>>4)&15);
//		i=((fl>>4)&3);
		if(i==0x5)
//		if(i==1)
//			{ sk=(sk==2)?0:1; }
		{
			if(ska==0) sk=1;
			if(ska==3) sk=5;
		}
		if(i==0x7)
			{ if((ska==0) || (ska==3)) sk=5; }

		if(i==0xA)
//		if(i==2)
//			{ sk=(sk==1)?0:2; }
		{
			if(ska==0) sk=2;
			if(ska==3) sk=4;
		}
		if(i==0xB)
			{ if((ska==0) || (ska==3)) sk=4; }

//		if(i==0xC)
//			{ if(ska==0) sk=3; }

		if((i==0x0) || (i==0xF))
			sk=0;
	}
#endif

//	if(mrw<4)
//		mrw=4;

	if(mrw<1)
		mrw=1;

	if(fl&1)
	{
//		for(j=0; j<16; j++)
//		for(j=0; j<4; j++)
//		for(j=0; j<2; j++)
		for(j=0; j<1; j++)
		{
			i=GANN_Rand16B();
			if(i>mrw)
				continue;

			ctx->mstat_sk[sk]++;
			i=GANN_Rand16();
			
//			if(!(i&4095))
//			if(!(i&1023))
			if(!(i&255))
			{
				if(fl&2)
				{
				}else
					if(fl&1)
				{
					vc=	((vc<<0)&0xFF00000000000000ULL) |
						((vc<<1)&0x0000DB6DB6DB6DB6ULL) |
						((vc>>2)&0x0000249249249249ULL) ;
					continue;
				}else
				{
					vc=	((vc<<0)&0xFF00000000000000ULL) |
						((vc<<1)&0x00FDFBF7EFDFBF7EULL) |
						((vc>>6)&0x0002040810204081ULL) ;
					continue;
				}
			}
			
//			if(((i>>8)&3)!=0)
//			if(!((i&fl)&0xFF00))
//				continue;
			i=(i<<1)^(i>>11);
			
			switch(sk)
			{
//			case 0: case 3:
			case 0:
				/* balanced bit-flip */
//				i=((i<<1)^(i>>11))&63;
				vc^=1ULL<<(i&63);
				break;
			case 2:
				/* 25% chance of clearing bits */
				k=(i>>6)&3;
				if((k==0) && !(vc&(1ULL<<(i&63))))
					i>>=8;
				nobisc=(nobimsk>>(i&63))&1;
				if((k==0) && !nobisc)
				{
					vc&=~(1ULL<<(i&63));
					break;
				}
				vc^=1ULL<<(i&63);
				break;
			case 1:
				/* 25% chance of setting bits */
				k=(i>>6)&3;
				if((k==0) && (vc&(1ULL<<(i&63))))
					i>>=8;
				nobisc=(nobimsk>>(i&63))&1;
				if((k==0) && !nobisc)
				{
					vc|=(1ULL<<(i&63));
					break;
				}
				vc^=1ULL<<(i&63);
				break;
			case 3:
				/* 12.5% chance of inc/dec */
				k=(i>>6)&3;
				if(k==0)
				{
					k=(i>>8)&3;
					if(k==1)
					{
						vc=GANN_MutateIncr(ctx, vc, i&63);
						break;
					}
					if(k==2)
					{
						vc=GANN_MutateDecr(ctx, vc, i&63);
						break;
					}
				}
				vc^=1ULL<<(i&63);
				break;

			case 4:
				/* 25% dec */
				k=(i>>6)&3;
				if(k==0)
				{
					vc=GANN_MutateDecr(ctx, vc, i&63);
					break;
				}
				vc^=1ULL<<(i&63);
				break;
			case 5:
				/* 25% inc */
				k=(i>>6)&3;
				if(k==0)
				{
					vc=GANN_MutateIncr(ctx, vc, i&63);
					break;
				}
				vc^=1ULL<<(i&63);
				break;

			case 6:
				/* 25% inertia */
				k=(i>>6)&3;
				if(k==0)
				{
					vc=GANN_MutateInertia(ctx, va, vb);
					break;
				}
				vc^=1ULL<<(i&63);
				break;
			}
		}
	}

	return(vc);
}

int GANN_RotateVecLf(u64 *ptrs, int ix, int n)
{
	u64 pa;
	int i;
	
	pa=ptrs[ix+0];
	for(i=0; i<n; i++)
		ptrs[ix+i+0]=ptrs[ix+i+1];
	ptrs[ix+n]=pa;
	return(0);
}

int GANN_RotateVecRt(u64 *ptrs, int ix, int n)
{
	u64 pa;
	int i;
	
	pa=ptrs[ix+n];
	for(i=n; i>0; i--)
		ptrs[ix+i+0]=ptrs[ix+i-1];
	ptrs[ix+0]=pa;
	return(0);
}

int GANN_BreedMember(
	GANN_Context *ctx,
	GANN_Member *ma, GANN_Member *mb, GANN_Member *mc,
	int ix)
{
	u64 va, vb;
	int vsz, mrm, ska, skb;
	int i, j, k;
	
	gann_seed1b=GANN_Rand64();
	gann_seed2b=GANN_Rand64();
	
//	mc->mrm=GANN_BreedBits(ma->mrm, mb->mrm, 0x5503);
	mc->mrm=GANN_BreedBits(ctx, ma->mrm, mb->mrm, 0xBB03);
//	mrm=GANN_Maj64to8(mc->mrm);
	i=GANN_Maj64to16(mc->mrm);
	
	ska=(i>>8);
	skb=ska&15;
	mrm=i&255;
	mrm=gann_gry2lin[mrm];
	
	i=(ix*ix)>>13;
	mrm=mrm*(i+1);
	if(mrm>255)
		mrm=255;
	
	mc->mrmb=mrm;
	mc->mrmc=ska;
	mc->brseq=ctx->brseq++;
	
	vsz=ma->vsz;
	for(i=0; i<vsz; i++)
	{
		mc->vec[i]=GANN_BreedBits(ctx,
			ma->vec[i], mb->vec[i], 1|(mrm<<8)|(skb<<4));
	}
	
//	if((((ska>>4)&3)==1) && (vsz>2))
//	if(((ska>>4)&1) && (vsz>2))
	if(vsz>2)
	{
		j=GANN_Rand16B();
		k=((mrm+2)*(mrm+2));
		i=(ska>>4)&3;
//		i=(i+1)*(i+1);	k=(k*j)>>3;
		i=i*i;		k=(k*i)>>2;

		if(j<k)
		{
			j=GANN_Rand16B();
			k=(j&255);
			k=(k*k)>>12;

			i=GANN_Rand16B();
			if(vsz>=65536)
				i^=(GANN_Rand16B()<<8);
			if(vsz<1024)
				i&=4095;

			k=(j&31);
			k=(k*k)>>6;
			while((i+(k>>1)+1)>=(vsz<<4))
				i-=vsz<<4;
			while((i+(k>>1)+1)>=vsz)
				i-=vsz;
			if(i>=0)
			{
				switch(k)
				{
				case 0:
				case 1:
					va=mc->vec[i+0]; vb=mc->vec[i+1];
					mc->vec[i+0]=vb; mc->vec[i+1]=va;
					break;
				case 2:
					va=mc->vec[i+0];
					mc->vec[i+0]=mc->vec[i+1];
					mc->vec[i+1]=mc->vec[i+2];
					mc->vec[i+2]=va;
					break;
				case 3:
					va=mc->vec[i+2];
					mc->vec[i+2]=mc->vec[i+1];
					mc->vec[i+1]=mc->vec[i+0];
					mc->vec[i+0]=va;
					break;
				case 4:
					va=mc->vec[i+0];
					mc->vec[i+0]=mc->vec[i+1];
					mc->vec[i+1]=mc->vec[i+2];
					mc->vec[i+2]=mc->vec[i+3];
					mc->vec[i+3]=va;
					break;
				case 5:
					va=mc->vec[i+3];
					mc->vec[i+3]=mc->vec[i+2];
					mc->vec[i+2]=mc->vec[i+1];
					mc->vec[i+1]=mc->vec[i+0];
					mc->vec[i+0]=va;
					break;
				case 6:		GANN_RotateVecLf(mc->vec, i, 4);	break;
				case 7:		GANN_RotateVecRt(mc->vec, i, 4);	break;
				case 8:		GANN_RotateVecLf(mc->vec, i, 5);	break;
				case 9:		GANN_RotateVecRt(mc->vec, i, 5);	break;
				case 10:	GANN_RotateVecLf(mc->vec, i, 6);	break;
				case 11:	GANN_RotateVecRt(mc->vec, i, 6);	break;
				case 12:	GANN_RotateVecLf(mc->vec, i, 7);	break;
				case 13:	GANN_RotateVecRt(mc->vec, i, 7);	break;
				case 14:	GANN_RotateVecLf(mc->vec, i, 8);	break;
				case 15:	GANN_RotateVecRt(mc->vec, i, 8);	break;
				}
			}
		}
	}

	return(0);
}

int GANN_CloneMember(
	GANN_Context *ctx,
	GANN_Member *ma, GANN_Member *mc)
{
	int vsz;
	int i;
	
	mc->mrm=ma->mrm;
	mc->mrmb=ma->mrmb;
	mc->mrmc=ma->mrmc;
	mc->brseq=ctx->brseq++;
	
	vsz=ma->vsz;
	for(i=0; i<vsz; i++)
		{ mc->vec[i]=ma->vec[i]; }
	return(0);
}

int GANN_GetMemberAsBytes(GANN_Member *ma, byte *buf)
{
	int vsz;
	int i, j;
	
	if(!ma->vec)
		return(0);
	
	if(ma->flag&2)
	{
		vsz=ma->vsz;
		for(i=0; i<vsz; i++)
		{
			j=GANN_Ham64to32(ma->vec[i]);
			buf[i*4+0]=(j>> 0)&255;
			buf[i*4+1]=(j>> 8)&255;
			buf[i*4+2]=(j>>16)&255;
			buf[i*4+3]=(j>>24)&255;
		}
		return(0);
	}
	
	if(ma->flag&1)
	{
		vsz=ma->vsz;
		for(i=0; i<vsz; i++)
		{
			j=GANN_Maj64to16(ma->vec[i]);
			buf[i*2+0]=(j>>0)&255;
			buf[i*2+1]=(j>>8)&255;
		}
		return(0);
	}
	
	vsz=ma->vsz;
	for(i=0; i<vsz; i++)
	{
		buf[i]=GANN_Maj64to8(ma->vec[i]);
	}
	return(0);
}


int GANN_SetMemberAsBytes(GANN_Member *ma, byte *buf)
{
	int vsz;
	int i, j;
	
	if(!ma->vec)
		return(0);
	
	if(ma->flag&2)
	{
		vsz=ma->vsz;
		for(i=0; i<vsz; i++)
		{
//			j=GANN_Ham64to32(ma->vec[i]);
//			buf[i*4+0]=(j>> 0)&255;
//			buf[i*4+1]=(j>> 8)&255;
//			buf[i*4+2]=(j>>16)&255;
//			buf[i*4+3]=(j>>24)&255;

			ma->vec[i]=GANN_ByteToHam7(
				buf[i*4+0], buf[i*4+1],
				buf[i*4+2], buf[i*4+3]);

			j=GANN_Ham64to32(ma->vec[i]);
			if(((j>> 0)&255)!=buf[i*4+0])
				{ __debugbreak(); }
			if(((j>> 8)&255)!=buf[i*4+1])
				{ __debugbreak(); }
			if(((j>>16)&255)!=buf[i*4+2])
				{ __debugbreak(); }
			if(((j>>24)&255)!=buf[i*4+3])
				{ __debugbreak(); }
		}
		return(0);
	}
	
	if(ma->flag&1)
	{
		vsz=ma->vsz;
		for(i=0; i<vsz; i++)
		{
//			j=GANN_Maj64to16(ma->vec[i]);
//			buf[i*2+0]=(j>>0)&255;
//			buf[i*2+1]=(j>>8)&255;
			ma->vec[i]=GANN_ByteToMaj3(buf[i*2+0], buf[i*2+1]);

			j=GANN_Maj64to16(ma->vec[i]);
			if(((j>>0)&255)!=buf[i*2+0])
				{ __debugbreak(); }
			if(((j>>8)&255)!=buf[i*2+1])
				{ __debugbreak(); }
		}
		return(0);
	}
	
	vsz=ma->vsz;
	for(i=0; i<vsz; i++)
	{
		ma->vec[i]=GANN_ByteToMaj7(buf[i]);

		j=GANN_Maj64to8(ma->vec[i]);
		if(j!=buf[i])
			{ __debugbreak(); }
	}
	return(0);
}


int GANN_ModifyMemberAsBytes(GANN_Member *ma, byte *buf)
{
	int vsz;
	int i, j;
	
	if(!ma->vec)
		return(0);
	
	if(ma->flag&2)
	{
		vsz=ma->vsz;
		for(i=0; i<vsz; i++)
		{
			j=GANN_Ham64to32(ma->vec[i]);
			if(		(buf[i*4+0]==((j>> 0)&255)) &&
					(buf[i*4+1]==((j>> 8)&255)) &&
					(buf[i*4+2]==((j>>16)&255)) &&
					(buf[i*4+3]==((j>>24)&255)) )
			{
				continue;
			}

			ma->vec[i]=GANN_ByteToHam7(
				buf[i*4+0], buf[i*4+1],
				buf[i*4+2], buf[i*4+3]);

			j=GANN_Ham64to32(ma->vec[i]);
			if(((j>> 0)&255)!=buf[i*4+0])
				{ __debugbreak(); }
			if(((j>> 8)&255)!=buf[i*4+1])
				{ __debugbreak(); }
			if(((j>>16)&255)!=buf[i*4+2])
				{ __debugbreak(); }
			if(((j>>24)&255)!=buf[i*4+3])
				{ __debugbreak(); }
		}
		return(0);
	}
	
	if(ma->flag&1)
	{
		vsz=ma->vsz;
		for(i=0; i<vsz; i++)
		{
			j=GANN_Maj64to16(ma->vec[i]);
			if(		(buf[i*2+0]==((j>> 0)&255)) &&
					(buf[i*2+1]==((j>> 8)&255)) )
			{
				continue;
			}

			ma->vec[i]=GANN_ByteToMaj3(buf[i*2+0], buf[i*2+1]);

			j=GANN_Maj64to16(ma->vec[i]);
			if(((j>>0)&255)!=buf[i*2+0])
				{ __debugbreak(); }
			if(((j>>8)&255)!=buf[i*2+1])
				{ __debugbreak(); }
		}
		return(0);
	}
	
	vsz=ma->vsz;
	for(i=0; i<vsz; i++)
	{
		j=GANN_Maj64to8(ma->vec[i]);
		if(buf[i]==(j&255))
			continue;
		ma->vec[i]=GANN_ByteToMaj7(buf[i]);

		j=GANN_Maj64to8(ma->vec[i]);
		if(j!=buf[i])
			{ __debugbreak(); }
	}
	return(0);
}

GANN_Context *GANN_AllocContext(int nmemb, int szmemb, int fl)
{
	GANN_Context *ctx;
	int i, j, k;
	int szmb;
	
	szmb=szmemb;
	if(fl&1)
		szmb=szmemb>>1;
	if(fl&2)
		szmb=szmemb>>2;
	
	ctx=malloc(sizeof(GANN_Context));
	memset(ctx, 0, sizeof(GANN_Context));
	ctx->marr=malloc((nmemb+16)*sizeof(GANN_Member *));
	ctx->earr=malloc((nmemb+16)*sizeof(int));
	ctx->nmemb=nmemb;
	ctx->szmemb=szmb;
	ctx->flag=fl;
	ctx->brseq=1;
	
	for(i=0; i<(nmemb+16); i++)
	{
		ctx->earr[i]=0;

		ctx->marr[i]=malloc(sizeof(GANN_Member));
		memset(ctx->marr[i], 0, sizeof(GANN_Member));
		ctx->marr[i]->vec=malloc(szmb*sizeof(u64));
		ctx->marr[i]->vsz=szmb;
		ctx->marr[i]->flag=fl;

		ctx->marr[i]->mrm=GANN_Rand64();
		ctx->marr[i]->brseq=ctx->brseq++;
		
		ctx->marr[i]->mrm&=0x0000000FFFFFFFFFULL;
		
		for(j=0; j<szmb; j++)
		{
			ctx->marr[i]->vec[j]=GANN_Rand64();
		}
	}
	
	return(ctx);
}

int GANN_TestMembers(GANN_Context *ctx)
{
	GANN_Member *mv;
	int ev;
	int i, j, k;

	if(!ctx->TestMember)
		return(0);

	for(i=0; i<ctx->nmemb; i++)
	{
		ctx->earr[i]=ctx->TestMember(ctx, ctx->marr[i]);
	}

	return(0);
}

int GANN_SortMembers(GANN_Context *ctx)
{
	GANN_Member *mv, *mv0;
	int ev;
	int i, j, k;

	mv0=ctx->marr[0];
	for(i=0; i<ctx->nmemb; i++)
	{
		for(j=i+1; j<ctx->nmemb; j++)
		{
			if(ctx->earr[j]<ctx->earr[i])
			{
				mv=ctx->marr[j];
				ctx->marr[j]=ctx->marr[i];
				ctx->marr[i]=mv;

				ev=ctx->earr[j];
				ctx->earr[j]=ctx->earr[i];
				ctx->earr[i]=ev;
			}
		}
	}

	for(i=0; i<ctx->nmemb; i++)
	{
		mv=ctx->marr[i];

		if(i<(ctx->nmemb>>1))
		{
			ctx->skrank[mv->mrmc&15]++;
		}else
		{
			ctx->skrank[mv->mrmc&15]--;
		}
	}

	if(mv0==ctx->marr[0])
	{
		for(i=0; i<8; i++)
		{
			mv=ctx->marr[i];
			k=GANN_Maj64to16(mv->mrm);
			if((k&255)>4)
			{
				k--;
				if((k&255)<0x10)
				{
					k&=~0xFF00;
				}
				
				mv->mrm=GANN_IntToMaj3(k);
				mv->mrmb=(k>>0)&255;
				mv->mrmc=(k>>8)&255;
			}
		}
	}

	k=ctx->nmemb;
	mv=ctx->marr[k+15];
	for(i=15; i>0; i--)
		ctx->marr[k+i]=ctx->marr[k+i-1];
	ctx->marr[k+0]=mv;
	GANN_CloneMember(ctx, ctx->marr[0], mv);

	for(i=0; i<(ctx->nmemb+16); i++)
	{
		ctx->marr[i]->curix=i;
	}

	return(0);
}

int GANN_BreedMembers(GANN_Context *ctx, int keep)
{
	int i0, i1, i2, i3, shk, nm34, nm78;
	int i, j, k;

	k=keep; shk=0;
	while(k>1)
		{ k=k>>1; shk++; }

	nm34=(ctx->nmemb*3)>>2;
	nm78=(ctx->nmemb*7)>>3;
	for(i=keep; i<ctx->nmemb; i++)
	{
		if(!(keep&(keep-1)))
		{
			i0=GANN_Rand16()&(keep-1);
			i1=GANN_Rand16()&(keep-1);
//			i2=GANN_Rand16()&(keep-1);
//			i3=GANN_Rand16()&(keep-1);
//			i0=(i0*i2)>>shk;
//			i1=(i1*i3)>>shk;

			if(i>nm34)
			{
				i1=ctx->nmemb+(GANN_Rand16()&15);
			}
			if(i>nm78)
			{
				i0=ctx->nmemb+(GANN_Rand16()&15);
			}

		}else
		{
			i0=GANN_Rand16()%keep;
			i1=GANN_Rand16()%keep;
//			i2=GANN_Rand16()%keep;
//			i3=GANN_Rand16()%keep;
//			i0=(i0*i2)/keep;
//			i1=(i1*i3)/keep;
		}
		
		if(i0>i1)
			{ i2=i0; i0=i1; i1=i2; }
		
		GANN_BreedMember(ctx,
			ctx->marr[i0],
			ctx->marr[i1],
			ctx->marr[i], i);
	}

	return(0);
}
