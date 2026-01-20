#include "adnn2_net.h"
#include "adnn2_fp8.c"

void *adnn2_malloc(int sz)
{
	void *ptr;
	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

void *adnn2_malloc_f8rng(int sz)
{
	byte *ptr;
	int i;
	ptr=malloc(sz);
//	memset(ptr, 0, sz);
	for(i=0; i<sz; i++)
		ptr[i]=adnn2_fastrand()&0xBF;
	return(ptr);
}

AdNn2_Layer *AdNn2_AllocLayer(int isz, int osz)
{
	AdNn2_Layer *tmp;
	int j, k, l, l4;
	
	tmp=adnn2_malloc(sizeof(AdNn2_Layer));
	tmp->nn_isz=isz;
	tmp->nn_osz=osz;
	tmp->nn_afn=0;

	j=isz+1;
	j=(j+7)&(~7);
	l=osz*j;
	k=((l+127)/64)*8;
	tmp->nn_stzm=(j/8);
	tmp->nn_szzm=k;

	l=osz*(isz+1);
	
	j=(isz+1)/2;
	tmp->nn_stw4=2+j;
	l4=osz*(j+2);

	tmp->wght=adnn2_malloc_f8rng(l*sizeof(adnn2_wght));
	tmp->wghtl=adnn2_malloc_f8rng(l*sizeof(adnn2_wght));
#ifdef ADNN2_WGHT16
	tmp->wght8=adnn2_malloc_f8rng(l);

#ifdef ADNN2_DOWGHTFP4
	tmp->wght4=adnn2_malloc_f8rng(l4);
#endif
#endif

	tmp->wght_zm=adnn2_malloc(k);
	tmp->act_o=adnn2_malloc(osz);
	tmp->act_i=adnn2_malloc(isz);
	tmp->err_o=adnn2_malloc(osz);
	tmp->err_i=adnn2_malloc(isz);
	tmp->act_po=adnn2_malloc(osz*sizeof(u16));
	
	return(tmp);
}

AdNn2_Net *AdNn2_AllocNet(int nl, int *lasz)
{
	AdNn2_Net *tmp;
	int i, j, k;
	
	tmp=adnn2_malloc(sizeof(AdNn2_Net));
	
	tmp->n_layer=nl;
	for(i=0; i<nl; i++)
	{
		tmp->layer[i]=AdNn2_AllocLayer(lasz[i+0], lasz[i+1]);
	}
	return(tmp);
}

int AdNn2_UpdateLayerUnpackFP4(AdNn2_Layer *layer)
{
	byte *zcs, *ct, *ct4;
	float twa;
	int isz, osz, zst, bw, ntwa, bi8, bwscl, bw0, bw1, dw0, dw1, st4;
	int i, j, k;

	isz=layer->nn_isz;
	osz=layer->nn_osz;
	zst=layer->nn_stzm;

	ct=layer->wght8;
	ct4=layer->wght4;
	st4=layer->nn_stw4;

	for(i=0; i<osz; i++)
	{
		bi8=ct4[0];
		bwscl=ct4[1];
	
		for(j=0; j<(st4-2); j++)
		{
			bw=ct4[2+j];
			dw0=((bw>>0)&15); bw0=0;
			dw1=((bw>>4)&15); bw1=0;
			if(dw0)		{ bw0=(bwscl+((dw0&7)-4))^((dw0<<4)&0x80); }
			if(dw1)		{ bw1=(bwscl+((dw1&7)-4))^((dw1<<4)&0x80); }
			ct[j*2+0]=bw0;
			ct[j*2+1]=bw1;
		}
		ct[isz]=bi8;

		ct4+=st4;
		ct+=(isz+1);
	}
	return(0);
}

int AdNn2_UpdateLayerZMask(AdNn2_Layer *layer)
{
	adnn2_wght *cs;
	byte *zcs, *ct, *ct4;
	float twa;
	int isz, osz, zst, bw, nbw, ntwa, bwscl, bw0, bw1, dw0, dw1;
	int i, j, k;

#if 1
	isz=layer->nn_isz;
	osz=layer->nn_osz;
	zst=layer->nn_stzm;

	cs=layer->wght;
	ct=layer->wght8;
	ct4=layer->wght4;
	zcs=layer->wght_zm;
	memset(zcs, 0, layer->nn_szzm);
	for(i=0; i<osz; i++)
	{
		for(j=0; j<(isz+1); j++)
		{
			bw=cs[j];
#ifdef ADNN2_WGHT16

#ifdef ADNN2_USE8RS
			k=AdNn2_Fp16to8Rs(bw);
#else
			k=AdNn2_Fp16to8(bw);
#endif
//			if(!((bw>>10)&31))
//			if(((bw>>10)&31)<5)
			if(!k)
				zcs[j>>3]|=1<<(j&7);
			ct[j]=k;
#else
			if(!bw)
				zcs[j>>3]|=1<<(j&7);
#endif
		}
		
		if(ct4)
		{
			nbw=0;
			twa=0; ntwa=4;
			for(j=0; j<(isz+1); j++)
			{
				bw=ct[j];
				if(!bw)
					continue;
				if((bw&0x7F)>nbw)
					nbw=bw&0x7F;
				twa+=fabs(AdNn2_Fp8toF32(bw));
				ntwa++;
			}
			if(ntwa)
				twa/=ntwa;

			bwscl=AdNn2_F32to8(twa);
			if(nbw>(bwscl+(3*4)))
				bwscl=nbw-(3*4);
			if(bwscl>0x73)
				bwscl=0x73;
			if(bwscl<0x0C)
				bwscl=0x0C;
			
			ct4[0]=ct[isz];
			ct4[1]=bwscl;
			for(j=0; j<isz; j+=2)
			{
				bw0=ct[j+0];
				bw1=ct[j+1];
				dw0=(bw0&0x7F)-(bwscl&0x7F);
				dw1=(bw1&0x7F)-(bwscl&0x7F);
				dw0=((dw0+2)>>2)+4;
				dw1=((dw1+2)>>2)+4;
				if(dw0>7)dw0=7;
				if(dw1>7)dw1=7;
				if((dw0<0) || !bw0)dw0=0;
				if((dw1<0) || !bw1)dw1=0;
//				if((bwscl+((dw0-4)<<2))<0)		dw0=0;
//				if((bwscl+((dw1-4)<<2))<0)		dw1=0;
				
				if(dw0>0)dw0|=(bw0>>4)&8;
				if(dw1>0)dw1|=(bw1>>4)&8;
				bw=(dw1<<4)|dw0;
				ct4[2+(j>>1)]=bw;
			}
			ct4+=layer->nn_stw4;
		}
		
		cs+=(isz+1);
		ct+=(isz+1);
		zcs+=zst;
	}
#endif

#ifdef ADNN2_DOWGHTFP4
	if(layer->wght4)
		{ AdNn2_UpdateLayerUnpackFP4(layer); }
#endif

	return(0);
}

int AdNn2_CopyNet(AdNn2_Net *dnet, AdNn2_Net *snet)
{
	AdNn2_Layer *dlyr, *slyr;
	int i, j, k, nl, isz, osz;
	
	nl=snet->n_layer;
	for(i=0; i<nl; i++)
	{
		dlyr=dnet->layer[i];
		slyr=snet->layer[i];
		isz=slyr->nn_isz;
		osz=slyr->nn_osz;
		dlyr->nn_afn=slyr->nn_afn;
		memcpy(dnet->layer[i]->wght, snet->layer[i]->wght,
			osz*(isz+1)*sizeof(adnn2_wght));
		AdNn2_UpdateLayerZMask(dlyr);
	}
	return(0);
}

int AdNn2_BreedNet(AdNn2_Net *dnet, AdNn2_Net *sanet, AdNn2_Net *sbnet)
{
	AdNn2_Layer *dlyr, *salyr, *sblyr;
	int bw, bwa, bwb, bwc;
	int i, j, k, nl, ix, isz, osz, lsz, wsel;
	
	nl=dnet->n_layer;
	for(i=0; i<nl; i++)
	{
		dlyr=dnet->layer[i];
		salyr=sanet->layer[i];
		sblyr=sbnet->layer[i];
		isz=dlyr->nn_isz;
		osz=dlyr->nn_osz;
		lsz=osz*(isz+1);

		wsel=adnn2_fastrand()&1;
		dlyr->nn_afn=wsel?sblyr->nn_afn:salyr->nn_afn;

		if(!(adnn2_fastrand()&16383))
		{
//			dlyr->nn_afn=adnn2_fastrand()&7;
//			dlyr->nn_afn=adnn2_fastrand()&3;
		}

		for(j=0; j<osz; j++)
		{
			wsel=adnn2_fastrand()&1;
			for(k=0; k<(isz+1); k++)
			{
				ix=(j*(isz+1))+k;
				bwa=salyr->wght[ix];
				bwb=sblyr->wght[ix];
				bw=wsel?bwb:bwa;
//				if(!(adnn2_fastrand()&63))
//				if(!(adnn2_fastrand()&255))
//					bw=AdNn2_BreedWeight(bwa, bwb);
//				if(!(adnn2_fastrand()&31))
				if(!(adnn2_fastrand()&255))
				{
					bwc=bw+(adnn2_fastrand_gauss_s8()>>5);
					if(!((bw^bwc)&0x8000))
						bw=bwc;
				}
				dlyr->wght[ix]=bw;
			}
		}

#if 0
		for(j=0; j<lsz; j++)
		{
			k=wsel?sblyr->wght[j]:salyr->wght[j];
			if(!(adnn2_fastrand()&15))
				k+=adnn2_fastrand_gauss_s8()>>5;
			dlyr->wght[j]=k;

//			dlyr->wght[j]=AdNn2_BreedWeight(salyr->wght[j], sblyr->wght[j]);
		}
#endif
		AdNn2_UpdateLayerZMask(dlyr);
	}
	return(0);
}

AdNn2_Net *AdNn2_CloneNet(AdNn2_Net *net0)
{
	int lasz[32];
	AdNn2_Net *tmp;
	int i, j, k, nl;
	
	nl=net0->n_layer;
	lasz[0]=net0->layer[0]->nn_isz;
	for(i=0; i<nl; i++)
	{
		lasz[i+1]=net0->layer[i]->nn_osz;
	}

	tmp=AdNn2_AllocNet(nl, lasz);
	AdNn2_CopyNet(tmp, net0);
	return(tmp);
}

int AdNn2_ForwardEvalLayer(AdNn2_Layer *layer, byte *buf_i, byte *buf_o)
{
#ifdef ADNN2_FWWGHT8
	byte *cs;
#else
	adnn2_wght *cs;
#endif
	byte *zcs;
	u16 (*FnActive)(u16 fn);
	int isz, osz, zst;
	adnn2_wght bv, bw;
	u16 acc, tv;
	int bv0, bv1, bv2, bv3, bw0, bw1, bw2, bw3;
	float ax, f0, f1, f2, f3, x0, x1, x2, x3, y0, y1, y2, y3;
	int i, j, k, zm, zms;
	
#ifdef ADNN2_FWWGHT8
	cs=layer->wght8;
#else
	cs=layer->wght;
#endif
	zcs=layer->wght_zm;
	isz=layer->nn_isz;
	osz=layer->nn_osz;
	zst=layer->nn_stzm;

//	FnActive=AdNn2_SSqrtFp16;
	FnActive=AdNn2_SaSSqrtFp16;

	zm=layer->nn_afn;
	if(zm)
	{
		if(zm==1)	FnActive=AdNn2_USqrtFp16;
		if(zm==2)	FnActive=AdNn2_IdentFp16;
		if(zm==3)	FnActive=AdNn2_ReluFp16;

		if(zm==4)	FnActive=AdNn2_SaSSqrtFp16;
		if(zm==5)	FnActive=AdNn2_SbSSqrtFp16;
		if(zm==6)	FnActive=AdNn2_SSqrt2Fp16;
		if(zm==7)	FnActive=AdNn2_USqrt2Fp16;
	}

	for(j=0; j<isz; j++)
	{
		bv=buf_i[j];
		layer->act_i[j]=bv;
	}
	
	for(i=0; i<osz; i++)
	{
#ifdef ADNN2_FWWGHT8
		ax=AdNn2_Fp8toF32(cs[isz]);
#else
		ax=AdNn2_WeightToF32(cs[isz]);
#endif
//		ax=0;
		for(j=0; j<isz; j++)
		{
#if 1
			zm=*(u16 *)(zcs+(j>>3));
			k=j&7;
//			if(zm==65535)
//				{ j+=15; continue; }
			zms=(zm>>k);
			if(zms&1)
			{
				if((zms&15)==15)
				{
					if((zms&255)==255)
						{ j+=7; continue; }
					j+=3;
					continue;
				}
//				if((zms&3)==3)
//					{ j++; continue; }
				continue;
			}

#ifdef ADNN2_FWWGHT8
// #if 0
//			if(!(zm&15) && ((j+3)<isz))
			if((j+3)<isz)
			{
				bv0=buf_i[j+0];		bw0=cs[j+0];
				bv1=buf_i[j+1];		bw1=cs[j+1];
				bv2=buf_i[j+2];		bw2=cs[j+2];
				bv3=buf_i[j+3];		bw3=cs[j+3];
				x0=adnn2_fp8to32f[bv0];		y0=adnn2_fp8to32f[bw0];
				x1=adnn2_fp8to32f[bv1];		y1=adnn2_fp8to32f[bw1];
				x2=adnn2_fp8to32f[bv2];		y2=adnn2_fp8to32f[bw2];
				x3=adnn2_fp8to32f[bv3];		y3=adnn2_fp8to32f[bw3];
				f0=x0*y0;	f1=x1*y1;
				f2=x2*y2;	f3=x3*y3;
				f0=f0+f1;
				f2=f2+f3;
				ax=ax+f0+f2;
				j+=3;
				continue;
			}
#endif

#endif

			bv=buf_i[j];
			bw=cs[j];
//			tv=adnn2_fp8mul16[(bv<<8)|bw];
//			acc=AdNn2_AddFp16(acc, tv);

#ifdef ADNN2_FWWGHT8
			ax=ax+AdNn2_MulFp8toF32(bv, bw);
#else
			ax=ax+AdNn2_MulWeightToF32(
				AdNn2_Fp8ToWeight(bv), bw);
#endif
		}
		
		acc=AdNn2_F32to16(ax);
		layer->act_po[i]=acc;
//		tv=AdNn2_SqrtFp16(acc);
		tv=FnActive(acc);

//		k=adnn2_fp16t10to8[tv>>6];
		k=AdNn2_Fp16to8(tv);
//		k=AdNn2_Fp16to8Rs(tv);
		buf_o[i]=k;
		layer->act_o[i]=k;
		
		cs+=isz+1;
		zcs+=zst;
	}
	
	return(0);
}

int AdNn2_ForwardEvalNet(AdNn2_Net *net, byte *buf_i, byte *buf_o)
{
	byte *bli, *blo;
	int nl, osz;
	int i, j, k;
	
	AdNn2_Init();
	
	nl=net->n_layer;
	for(i=0; i<nl; i++)
	{
		if(i>0)
			{ bli=net->layer[i-1]->act_o; }
		else
			{ bli=buf_i; }
		blo=net->layer[i]->act_o;
		AdNn2_ForwardEvalLayer(net->layer[i], bli, blo);
	}
	
	osz=net->layer[nl-1]->nn_osz;
	blo=net->layer[nl-1]->act_o;
	memcpy(buf_o, blo, osz);
	return(0);
}

/*
 * Eval net backwards, giving error values;
 * ri/ro represent the reversed inputs and outputs
 * So, ro will actually be the input here.
 */
int AdNn2_ReverseEvalLayer(AdNn2_Layer *layer, byte *err_ri, byte *err_ro)
{
#ifdef ADNN2_FWWGHT8
	byte *cs, *cs0;
#else
	adnn2_wght *cs, *cs0;
#endif
	int isz, osz;
	adnn2_wght bv, bw, bwb;
	u16 acc, tv, acc_po;
	float ax, f, g;
	int i, j, k;
	
#ifdef ADNN2_FWWGHT8
	cs=layer->wght8;
#else
	cs=layer->wght;
#endif
	isz=layer->nn_isz;
	osz=layer->nn_osz;
	cs0=cs;
	
	for(i=0; i<isz; i++)
	{
		ax=0;
		for(j=0; j<osz; j++)
		{
			bv=err_ro[j];
			bw=cs[j*(isz+1)];
			bwb=cs0[j*(isz+1)+isz];

#ifdef ADNN2_FWWGHT8
			f=AdNn2_MulFp8toF32(bv, bw);
#else
//			ax=ax+AdNn2_MulFp8toF32(bv, bw);
//			ax=ax+AdNn2_MulWeightToF32(
//				AdNn2_Fp8ToWeight(bv), bw)-
//				AdNn2_WeightToF32(bwb);

#if 0
			tv=layer->act_po[j];
			g=AdNn2_Fp16toF32(tv);
			g=AdNn2_SSqrtDeriv2(g);
//			g=AdNn2_SSqrtDeriv(g);
//			g=0.5;
			g=AdNn2_Fp8toF32(bv)*g;
			f=AdNn2_WeightToF32(bw)*g;
#endif

			f=AdNn2_MulWeightToF32(
				AdNn2_Fp8ToWeight(bv), bw);
#endif

//			acc_po=layer->act_po[j];
//			f*=1.0/(2.0*AdNn2_Fp16toF32(AdNn2_SqrtFp16(acc_po)));

			ax=ax+f;
		}
		
		ax*=0.5;
		
		acc=AdNn2_F32to16(ax);
		tv=acc;
//		tv=AdNn2_SSqrtFp16(acc);
//		k=adnn2_fp16t10to8[tv>>6];
		k=AdNn2_Fp16to8(tv);
//		k=AdNn2_Fp16to8Rs(tv);
		if((k&0x7F)>0x3F)
			k=(k&0x80)?0xBF:0x3F;
		err_ri[i]=k;
		
		cs++;
	}
	
	return(0);
}

int AdNn2_ReverseEvalNet(AdNn2_Net *net, byte *err_ri, byte *err_ro)
{
	AdNn2_Layer *layer;
	byte *bli, *blo, *bli2;
	int nl, osz;
	int i, j, k;
	
	AdNn2_Init();
	
	nl=net->n_layer;
	blo=net->layer[nl-1]->err_o;
	memcpy(blo, err_ro, net->layer[nl-1]->nn_osz);
//	for(i=0; i<nl; i++)
	for(i=nl-1; i>=0; i--)
	{
		if(i>0)
			{ bli2=net->layer[i-1]->err_o; }
		else
			{ bli2=err_ri; }

		layer=net->layer[i];
		bli=layer->err_i;
		blo=layer->err_o;
		AdNn2_ReverseEvalLayer(net->layer[i], bli, blo);
		if(bli2)
			{ memcpy(bli2, bli, layer->nn_isz); }
	}
	return(0);
}

int AdNn2_AdjustLayer(AdNn2_Layer *layer)
{
	adnn2_wght *cs, *lcs;
	byte *eci, *eco, *ci, *co, *zcs;
	int isz, osz;
	adnn2_wght bvo, bvi, bv, bw, bwb, bwl, bwc;
	u16 acc, tv;
	float ax, ay;
	float f, g, h;
	int giadj, wadj, wasc;
	int i, j, k, l;
	
	eci=layer->err_i;
	eco=layer->err_o;
	ci=layer->act_i;
	co=layer->act_o;
	cs=layer->wght;
	lcs=layer->wghtl;
	isz=layer->nn_isz;
	osz=layer->nn_osz;

//	wasc=1+(rand()&3);
	wasc=1;
	
	for(i=0; i<osz; i++)
	{
		giadj=0;
		bv=co[i];

		if((bv&0x7F)>0x60)
			giadj=-5;
		if((bv&0x7F)<0x20)
			giadj=2;

		if((bv&0x7F)>0x70)
			giadj=-7;
		if((bv&0x7F)<0x10)
			giadj=4;
	
		giadj/=2;
	
		bvo=eco[i];
		
//		if(!bvo)
//			continue;
		
		bw=cs[isz];
		bwl=lcs[isz];
		bwc=bw;
		bwb=AdNn2_WeightToFp8Fast(bw);
		
		if(bvo!=0x00)
		{
			wasc=3;
			if((bvo&0x7F)<=0x30)
				wasc=1;
		
			if((bvo^bwb)&0x80)
				{ wadj=-wasc; }
			else
				{ wadj= wasc; }

//			if(bwc>bwl)wadj++;
//			if(bwc<bwl)wadj--;

			if((bwb&0x7F)>(bvo&0x7F))
//			if((bwb&0x7F)>0x3F)
				wadj=-1;
		}

//		bw=(257*bw-bwl)>>8;
//		bw=(257*bw-bwl)>>8;
//		if((bw^bwc)&ADNN2_WGHTSGNMSK)
//			bw=bwc;
//		if(bwc>bwl)wadj++;
//		if(bwc<bwl)wadj--;
//		if((bwc-bwl)>4)wadj++;
//		if((bwl-bwc)>4)wadj--;

#if 0
		bw=AdNn2_AdjustWeightScaleBias(bw, giadj+wadj);

		ax=AdNn2_WeightToF32(bw);
		if(fabs(ax)<1.0)
		{
			if(bvo!=0x00)
				ax+=(bvo&0x80)?(-0.001):(0.001);
		}
		else
			ax=ax*0.999;
		bw=AdNn2_F32ToWeight(ax);
		

		cs[isz]=bw;
		lcs[isz]=bwc;
#endif

		ax=AdNn2_WeightToF32(bwc);

//		ax=0;
		ay=fabs(ax);
		for(j=0; j<isz; j++)
		{
//			f=AdNn2_WeightToF32(cs[isz]);
			f=AdNn2_WeightToF32(cs[j]);
			ax+=f;
			ay+=fabs(f);
		}

		wadj=0;

#if 1
		if(fabs(ax)>49.0)
//			{ wadj=-7; }
			{ wadj=-3; }
		if(fabs(ax)>25.0)
//			{ wadj=-5; }
			{ wadj=-2; }
		if(fabs(ax)>10.0)
			{ wadj=-1; }

		if(fabs(ay)>384.0)
//			{ wadj=-7; }
			{ wadj=-3; }
		if(fabs(ay)>192.0)
///			{ wadj=-3; }
			{ wadj=-2; }
		if(fabs(ay)>48.0)
			{ wadj=-1; }

		if(ay<0.125)
			{ wadj=1; }

		if((bv^bvo)&0x80)
			wadj--;

		wadj/=2;
//		if(!wadj)
//			wadj=1;

//		wadj=1;
//		wadj++;
#endif

//		if((wadj<giadj) || ((giadj==0) && (wadj!=0)))
//			giadj=wadj;
		giadj+=wadj;


#if 1
		bw=AdNn2_AdjustWeightScaleBias(bw, giadj);

		ax=AdNn2_WeightToF32(bw);
		if(fabs(ax)<0.01)
		{
			if(bvo!=0x00)
//				ax+=(bvo&0x80)?(-0.00001):(0.00001);
				ax+=(bvo&0x80)?(-0.00005):(0.00005);
//				ax+=(bvo&0x80)?(-0.0001):(0.0001);
		}
		else
			if(fabs(ax)<0.1)
		{
			if(bvo!=0x00)
//				ax+=(bvo&0x80)?(-0.0001):(0.0001);
				ax+=(bvo&0x80)?(-0.0005):(0.0005);
//				ax+=(bvo&0x80)?(-0.001):(0.001);
		}
		else
			if(fabs(ax)<1.0)
		{
			if(bvo!=0x00)
//				ax+=(bvo&0x80)?(-0.001):(0.001);
				ax+=(bvo&0x80)?(-0.005):(0.005);
//				ax+=(bvo&0x80)?(-0.010):(0.010);
		}
		else
			if(fabs(ax)<4.0)
		{
			ax=ax*0.999;
		}else
		{
			ax=ax*0.85;
		}
		bw=AdNn2_F32ToWeight(ax);

		cs[isz]=bw;
		lcs[isz]=bwc;
#endif

//		if((bv^bvo)&0x80)
//			giadj--;

		for(j=0; j<isz; j++)
		{
			bvi=eci[j];
			bw=cs[j];
			bwl=lcs[j];
			bv=ci[j];
			bwc=bw;

			bwb=AdNn2_WeightToFp8Fast(bw);
			wadj=0;

			if(bvo!=0x00)
			{
#if 1
				if((bwb^bv^bvo)&0x80)
	//			if((bw^bv^bvo^bvi)&0x80)
	//			if(!((bw^bv^bvo)&0x80))
	//			if(!((bw^bv^bvo^bvi)&0x80))
				{
					wadj=-wasc;
//					wadj=-3;
				}else
				{
					wadj= wasc;
//					wadj= 3;
				}
#endif

//				if(bwc>bwl)wadj+=2;
//				if(bwc<bwl)wadj-=2;

//				if((bwc-bwl)>4)wadj++;
//				if((bwl-bwc)>4)wadj--;

//				if((bvo&0x7F)<=0x30)
//					wadj>>=1;
			}

//			if((bwb&0x7F)>(bvo&0x7F))
//			if((bwb&0x7F)>0x3F)
//				wadj=-1;


//			bw=(5*bw-bwl)>>2;
//			bw=(17*bw-bwl)>>4;
//			bw=(257*bw-bwl)>>8;
//			if((bw^bwc)&ADNN2_WGHTSGNMSK)
//				bw=bwc;

//			if(bwc>bwl)wadj++;
//			if(bwc<bwl)wadj--;

			if((bwc<bwl) && (wadj>0))wadj=0;

//			if((bwc-bwl)>4)wadj++;
//			if((bwl-bwc)>4)wadj--;

			bw=AdNn2_AdjustWeightScaleBias(bw, giadj+wadj);
			
			lcs[j]=bwc;
			cs[j]=bw;
		}
		
		cs+=(isz+1);
		lcs+=(isz+1);
	}

	AdNn2_UpdateLayerZMask(layer);
	
	return(0);
}

int AdNn2_AdjustNet(AdNn2_Net *net)
{
	int nl, osz;
	int i, j, k;
	
	AdNn2_Init();
	nl=net->n_layer;
	for(i=0; i<nl; i++)
		{ AdNn2_AdjustLayer(net->layer[i]); }
	return(0);
}
