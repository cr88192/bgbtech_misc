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

static int clamp255(int i)
{
	if(i<0)return(0);
	if(i>255)return(255);
	return(i);
}

void BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA(
	byte *pxy, byte *pxy2, byte *pxa,
	int *min_rgba, int *max_rgba,
	int *rmcy, int *rncy,
	int *rmca, int *rnca,
	int *rpartn,
	byte *rgba, int xstride, int ystride, int pfb)
{
	static const int rcp4tab[16]={
		256, 256, 128, 85, 64, 51, 43, 37,
		 32,  28,  26, 23, 21, 20, 18, 17};

	byte pxrgb[16*4];
	byte pxyr[16], pxyg[16], pxyb[16];
	byte pxu[16], pxv[16];
	byte *tpxy, *tpxy1, *tpxy2, *cs, *ct;
	byte *tpart;

	int mcr, mcg, mcb, mca;
	int ncr, ncg, ncb, nca;
	int mcy, mcu, mcv;
	int ncy, ncu, ncv;

	int mcr1, mcg1, mcb1, mcy1;
	int ncr1, ncg1, ncb1, ncy1;
	int mcr2, mcg2, mcb2, mcy2;
	int ncr2, ncg2, ncb2, ncy2;

//	int acr, acg, acb, aca, acy;
//	int mcr3, mcg3, mcb3, mcy3;
//	int ncr3, ncg3, ncb3, ncy3;

//	int mcr3r, mcg3r, mcb3r, ncr3r, ncg3r, ncb3r, mcy3r, ncy3r;
//	int mcr3g, mcg3g, mcb3g, ncr3g, ncg3g, ncb3g, mcy3g, ncy3g;
//	int mcr3b, mcg3b, mcb3b, ncr3b, ncg3b, ncb3b, mcy3b, ncy3b;

	byte mcr3r, mcg3r, mcb3r, ncr3r, ncg3r, ncb3r, mcy3r, ncy3r;
	byte mcr3g, mcg3g, mcb3g, ncr3g, ncg3g, ncb3g, mcy3g, ncy3g;
	byte mcr3b, mcg3b, mcb3b, ncr3b, ncg3b, ncb3b, mcy3b, ncy3b;

//	int mcr3r0, mcg3r0, mcb3r0, ncr3r0, ncg3r0, ncb3r0, mcy3r0, ncy3r0;
//	int mcr3g0, mcg3g0, mcb3g0, ncr3g0, ncg3g0, ncb3g0, mcy3g0, ncy3g0;
//	int mcr3b0, mcg3b0, mcb3b0, ncr3b0, ncg3b0, ncb3b0, mcy3b0, ncy3b0;
//	int mcr3r1, mcg3r1, mcb3r1, ncr3r1, ncg3r1, ncb3r1, mcy3r1, ncy3r1;
//	int mcr3g1, mcg3g1, mcb3g1, ncr3g1, ncg3g1, ncb3g1, mcy3g1, ncy3g1;
//	int mcr3b1, mcg3b1, mcb3b1, ncr3b1, ncg3b1, ncb3b1, mcy3b1, ncy3b1;

	byte mcr3r0, mcg3r0, mcb3r0, ncr3r0, ncg3r0, ncb3r0, mcy3r0, ncy3r0;
	byte mcr3g0, mcg3g0, mcb3g0, ncr3g0, ncg3g0, ncb3g0, mcy3g0, ncy3g0;
	byte mcr3b0, mcg3b0, mcb3b0, ncr3b0, ncg3b0, ncb3b0, mcy3b0, ncy3b0;
	byte mcr3r1, mcg3r1, mcb3r1, ncr3r1, ncg3r1, ncb3r1, mcy3r1, ncy3r1;
	byte mcr3g1, mcg3g1, mcb3g1, ncr3g1, ncg3g1, ncb3g1, mcy3g1, ncy3g1;
	byte mcr3b1, mcg3b1, mcb3b1, ncr3b1, ncg3b1, ncb3b1, mcy3b1, ncy3b1;

	int acr, acg, acb;
	int acr0, acg0, acb0, acr1, acg1, acb1;
	int acn0, acn1;

	int tcr0, tcg0, tcb0, tcr1, tcg1, tcb1;

	int dr, dg, db, da, dy;
	int dcu, dcv, acu, acv;
//	int cr, cg, cb, ca, cy;
//	int cyr, cyg, cyb;
	byte cr, cg, cb, ca;
	byte cy, cu, cv;
	byte cyr, cyg, cyb;
	int pn;
	int i, j, k, l;

	mca=255; nca=255;
//	acr=  0; acg=  0; acb=  0; aca=  0;
	
	mcr3r=255; mcg3r=255; mcb3r=255; mcy3r=255;
	ncr3r=  0; ncg3r=  0; ncb3r=  0; ncy3r=  0;
	mcr3g=255; mcg3g=255; mcb3g=255; mcy3g=255;
	ncr3g=  0; ncg3g=  0; ncb3g=  0; ncy3g=  0;
	mcr3b=255; mcg3b=255; mcb3b=255; mcy3b=255;
	ncr3b=  0; ncg3b=  0; ncb3b=  0; ncy3b=  0;

	if((xstride!=4) || (pfb&BC7_PFB_NOAX))
	{
		if(pfb&1)
		{
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=(i*ystride)+(j*xstride);		l=i*16+j*4;
				pxrgb[l+0]=rgba[k+2];	pxrgb[l+1]=rgba[k+1];
				pxrgb[l+2]=rgba[k+0];	pxrgb[l+3]=255;
			}
		}else
		{
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=(i*ystride)+(j*xstride);		l=i*16+j*4;
				pxrgb[l+0]=rgba[k+0];	pxrgb[l+1]=rgba[k+1];
				pxrgb[l+2]=rgba[k+2];	pxrgb[l+3]=255;
			}
		}
	}else
	{
		if(pfb&1)
		{
			for(i=0; i<4; i++)
			{
				cs=rgba+(i*ystride); ct=pxrgb+i*16;
				ct[0]=cs[2]; ct[1]=cs[1]; ct[2]=cs[0]; ct[3]=cs[3];
				cs+=4; ct+=4;
				ct[0]=cs[2]; ct[1]=cs[1]; ct[2]=cs[0]; ct[3]=cs[3];
				cs+=4; ct+=4;
				ct[0]=cs[2]; ct[1]=cs[1]; ct[2]=cs[0]; ct[3]=cs[3];
				cs+=4; ct+=4;
				ct[0]=cs[2]; ct[1]=cs[1]; ct[2]=cs[0]; ct[3]=cs[3];
			}
		}else
		{
			for(i=0; i<4; i++)
			{
				cs=rgba+(i*ystride); ct=pxrgb+i*16;
				memcpy(ct, cs, 16);
			}
		}
	}

#if 1
	pn=-1;
	if(bgbbtj_bc7_part_init && rpartn && !(pfb&BC7_PFB_NOPART))
	{
		mcu=255;	mcv=255;
		ncu=0;		ncv=0;
		cs=pxrgb;
		for(i=0; i<16; i++)
		{
//			cr=cs[0]; cg=cs[1]; cb=cs[2]; cs+=4;
//			cu=((cb-cg)>>1)+128;	cv=((cr-cg)>>1)+128;

			cr=cs[0]; cg=cs[1]; cb=cs[2]; cs+=4;
			cy=(cr+2*cg+cb)>>2;
			cu=((cb-cy)>>1)+128;	cv=((cr-cy)>>1)+128;

//			cg=cs[1]; cu=((cs[2]-cg)>>1)+128;
//			cv=((cs[0]-cg)>>1)+128; cs+=4;
			pxu[i]=cu;	pxv[i]=cv;
			if(cu<mcu) { mcu=cu; }
			if(cv<mcv) { mcv=cv; }
			if(cu>ncu) { ncu=cu; }
			if(cv>ncv) { ncv=cv; }
		}
		acu=(mcu+ncu)>>1;
		acv=(mcv+ncv)>>1;

		dcu=ncu-mcu; dcv=ncv-mcv;
//		if((dcu+dcv)>48)
		if((dcu+dcv)>48)
		{
			if(dcu>dcv)
			{
				k=0;
				for(i=0; i<16; i++)
					{ k|=((pxu[i]>acu)<<i); }
				pn=bgbbtj_bc7_part2_idx[k];
			}else
			{
				k=0;
				for(i=0; i<16; i++)
					{ k|=((pxv[i]>acv)<<i); }
				pn=bgbbtj_bc7_part2_idx[k];
			}
		}
		*rpartn=pn;
	}else if(rpartn)
	{
		*rpartn=pn;
	}
#endif

	if(pn>=0)
	{
		tpart=bgbbtj_bc7_partition2+(pn*16);

		mcr3r0=255; mcg3r0=255; mcb3r0=255; mcy3r0=255;
		ncr3r0=  0; ncg3r0=  0; ncb3r0=  0; ncy3r0=  0;
		mcr3g0=255; mcg3g0=255; mcb3g0=255; mcy3g0=255;
		ncr3g0=  0; ncg3g0=  0; ncb3g0=  0; ncy3g0=  0;
		mcr3b0=255; mcg3b0=255; mcb3b0=255; mcy3b0=255;
		ncr3b0=  0; ncg3b0=  0; ncb3b0=  0; ncy3b0=  0;

		mcr3r1=255; mcg3r1=255; mcb3r1=255; mcy3r1=255;
		ncr3r1=  0; ncg3r1=  0; ncb3r1=  0; ncy3r1=  0;
		mcr3g1=255; mcg3g1=255; mcb3g1=255; mcy3g1=255;
		ncr3g1=  0; ncg3g1=  0; ncb3g1=  0; ncy3g1=  0;
		mcr3b1=255; mcg3b1=255; mcb3b1=255; mcy3b1=255;
		ncr3b1=  0; ncg3b1=  0; ncb3b1=  0; ncy3b1=  0;
		
		acr=0;		acg=0;		acb=0;
		acr0=0;		acg0=0;		acb0=0;		acn0=0;
		acr1=0;		acg1=0;		acb1=0;		acn1=0;

		cs=pxrgb;
		for(i=0; i<16; i++)
		{
			cr=cs[0]; cg=cs[1]; cb=cs[2]; ca=cs[3];
			cs+=4;

			cyr=(4*cr+3*cg+  cb)>>3;	//Yellow
			cyg=(  cr+4*cg+3*cb)>>3;	//Cyan
			cyb=(3*cr+  cg+4*cb)>>3;	//Magenta

			acr+=cr;	acg+=cg;	acb+=cb;

			pxyr[i]=cyr;		pxyg[i]=cyg;
			pxyb[i]=cyb;		pxa[i]=ca;

			if(cyr<mcy3r) { mcr3r=cr; mcg3r=cg; mcb3r=cb; mcy3r=cyr; }
			if(cyr>ncy3r) { ncr3r=cr; ncg3r=cg; ncb3r=cb; ncy3r=cyr; }
			if(cyg<mcy3g) { mcr3g=cr; mcg3g=cg; mcb3g=cb; mcy3g=cyg; }
			if(cyg>ncy3g) { ncr3g=cr; ncg3g=cg; ncb3g=cb; ncy3g=cyg; }
			if(cyb<mcy3b) { mcr3b=cr; mcg3b=cg; mcb3b=cb; mcy3b=cyb; }
			if(cyb>ncy3b) { ncr3b=cr; ncg3b=cg; ncb3b=cb; ncy3b=cyb; }
			if(ca<mca) { mca=ca; }
			if(ca>nca) { nca=ca; }

			if(tpart[i])
			{
				acr1+=cr;	acg1+=cg;	acb1+=cb;	acn1++;

				if(cyr<mcy3r1)
					{ mcr3r1=cr; mcg3r1=cg; mcb3r1=cb; mcy3r1=cyr; }
				if(cyr>ncy3r1)
					{ ncr3r1=cr; ncg3r1=cg; ncb3r1=cb; ncy3r1=cyr; }
				if(cyg<mcy3g1)
					{ mcr3g1=cr; mcg3g1=cg; mcb3g1=cb; mcy3g1=cyg; }
				if(cyg>ncy3g1)
					{ ncr3g1=cr; ncg3g1=cg; ncb3g1=cb; ncy3g1=cyg; }
				if(cyb<mcy3b1)
					{ mcr3b1=cr; mcg3b1=cg; mcb3b1=cb; mcy3b1=cyb; }
				if(cyb>ncy3b1)
					{ ncr3b1=cr; ncg3b1=cg; ncb3b1=cb; ncy3b1=cyb; }
			}else
			{
				acr0+=cr;	acg0+=cg;	acb0+=cb;	acn0++;

				if(cyr<mcy3r0)
					{ mcr3r0=cr; mcg3r0=cg; mcb3r0=cb; mcy3r0=cyr; }
				if(cyr>ncy3r0)
					{ ncr3r0=cr; ncg3r0=cg; ncb3r0=cb; ncy3r0=cyr; }
				if(cyg<mcy3g0)
					{ mcr3g0=cr; mcg3g0=cg; mcb3g0=cb; mcy3g0=cyg; }
				if(cyg>ncy3g0)
					{ ncr3g0=cr; ncg3g0=cg; ncb3g0=cb; ncy3g0=cyg; }
				if(cyb<mcy3b0)
					{ mcr3b0=cr; mcg3b0=cg; mcb3b0=cb; mcy3b0=cyb; }
				if(cyb>ncy3b0)
					{ ncr3b0=cr; ncg3b0=cg; ncb3b0=cb; ncy3b0=cyb; }
			}
		}
		
		acr=acr>>4;		acg=acg>>4;		acb=acb>>4;
		
		j=rcp4tab[acn0];		k=rcp4tab[acn1];
		acr0=(acr0*j+127)>>8;	acg0=(acg0*j+127)>>8;	acb0=(acb0*j+127)>>8;
		acr1=(acr1*k+127)>>8;	acg1=(acg1*k+127)>>8;	acb1=(acb1*k+127)>>8;
	}else
	{
		acr=0;		acg=0;		acb=0;

		cs=pxrgb;
		for(i=0; i<16; i++)
		{
			cr=cs[0]; cg=cs[1]; cb=cs[2]; ca=cs[3];
			cs+=4;

			cyr=(4*cr+3*cg+  cb)>>3;	//Yellow
			cyg=(  cr+4*cg+3*cb)>>3;	//Cyan
			cyb=(3*cr+  cg+4*cb)>>3;	//Magenta

			acr+=cr;	acg+=cg;	acb+=cb;

			pxyr[i]=cyr;		pxyg[i]=cyg;
			pxyb[i]=cyb;		pxa[i]=ca;

			//Find Mins/Maxs
			if(cyr<mcy3r) { mcr3r=cr; mcg3r=cg; mcb3r=cb; mcy3r=cyr; }
			if(cyr>ncy3r) { ncr3r=cr; ncg3r=cg; ncb3r=cb; ncy3r=cyr; }
			if(cyg<mcy3g) { mcr3g=cr; mcg3g=cg; mcb3g=cb; mcy3g=cyg; }
			if(cyg>ncy3g) { ncr3g=cr; ncg3g=cg; ncb3g=cb; ncy3g=cyg; }
			if(cyb<mcy3b) { mcr3b=cr; mcg3b=cg; mcb3b=cb; mcy3b=cyb; }
			if(cyb>ncy3b) { ncr3b=cr; ncg3b=cg; ncb3b=cb; ncy3b=cyb; }
			if(ca<mca) { mca=ca; }
			if(ca>nca) { nca=ca; }
		}

		acr=acr>>4;		acg=acg>>4;		acb=acb>>4;
	}

	dr=ncy3r-mcy3r;
	dg=ncy3g-mcy3g;
	db=ncy3b-mcy3b;

#if 1
	if(dg>dr)
	{
		if(dg>=db)
		{
			mcr=mcr3g; mcg=mcg3g; mcb=mcb3g; mcy=mcy3g;
			ncr=ncr3g; ncg=ncg3g; ncb=ncb3g; ncy=ncy3g;
			tpxy=pxyg; dy=ncy-mcy;
		}else
		{
			mcr=mcr3b; mcg=mcg3b; mcb=mcb3b; mcy=mcy3b;
			ncr=ncr3b; ncg=ncg3b; ncb=ncb3b; ncy=ncy3b;
			tpxy=pxyb; dy=ncy-mcy;
		}
	}else
	{
		if(dr>=db)
		{
			mcr=mcr3r; mcg=mcg3r; mcb=mcb3r; mcy=mcy3r;
			ncr=ncr3r; ncg=ncg3r; ncb=ncb3r; ncy=ncy3r;
			tpxy=pxyr; dy=ncy-mcy;
		}else
		{
			mcr=mcr3b; mcg=mcg3b; mcb=mcb3b; mcy=mcy3b;
			ncr=ncr3b; ncg=ncg3b; ncb=ncb3b; ncy=ncy3b;
			tpxy=pxyb; dy=ncy-mcy;
		}
	}
#endif

#if 1
	dy=ncy-mcy;
//	j=dy>>1;
	j=(96*dy)>>8;
	tcr0=acr-j;	tcg0=acg-j;	tcb0=acb-j;
	tcr1=acr+j;	tcg1=acg+j;	tcb1=acb+j;
	tcr0=(3*mcr+tcr0)>>2;	tcg0=(3*mcg+tcg0)>>2;	tcb0=(3*mcb+tcb0)>>2;
	tcr1=(3*ncr+tcr1)>>2;	tcg1=(3*ncg+tcg1)>>2;	tcb1=(3*ncb+tcb1)>>2;
	if((tcr0|tcg0|tcb0)>>8)
		{ tcr0=clamp255(tcr0); tcg0=clamp255(tcg0); tcb0=clamp255(tcb0); }
	if((tcr1|tcg1|tcb1)>>8)
		{ tcr1=clamp255(tcr1); tcg1=clamp255(tcg1); tcb1=clamp255(tcb1); }
	mcr=tcr0;	mcg=tcg0;	mcb=tcb0;
	ncr=tcr1;	ncg=tcg1;	ncb=tcb1;
#endif

	if(pn>=0)
	{
		dr=ncy3r0-mcy3r0;
		dg=ncy3g0-mcy3g0;
		db=ncy3b0-mcy3b0;

		mcr1=mcr3r0; mcg1=mcg3r0; mcb1=mcb3r0; mcy1=mcy3r0;
		ncr1=ncr3r0; ncg1=ncg3r0; ncb1=ncb3r0; ncy1=ncy3r0;
		tpxy1=pxyr; dy=ncy1-mcy1;

		if(dg>dy)
		{
			mcr1=mcr3g0; mcg1=mcg3g0; mcb1=mcb3g0; mcy1=mcy3g0;
			ncr1=ncr3g0; ncg1=ncg3g0; ncb1=ncb3g0; ncy1=ncy3g0;
			tpxy=pxyg; dy=ncy1-mcy1;
		}
		if(db>dy)
		{
			mcr1=mcr3b0; mcg1=mcg3b0; mcb1=mcb3b0; mcy1=mcy3b0;
			ncr1=ncr3b0; ncg1=ncg3b0; ncb1=ncb3b0; ncy1=ncy3b0;
			tpxy1=pxyb; dy=ncy1-mcy1;
		}

		dr=ncy3r1-mcy3r1;
		dg=ncy3g1-mcy3g1;
		db=ncy3b1-mcy3b1;

		mcr2=mcr3r1; mcg2=mcg3r1; mcb2=mcb3r1; mcy2=mcy3r1;
		ncr2=ncr3r1; ncg2=ncg3r1; ncb2=ncb3r1; ncy2=ncy3r1;
		tpxy2=pxyr; dy=ncy2-mcy2;

		if(dg>dy)
		{
			mcr2=mcr3g1; mcg2=mcg3g1; mcb2=mcb3g1; mcy2=mcy3g1;
			ncr2=ncr3g1; ncg2=ncg3g1; ncb2=ncb3g1; ncy2=ncy3g1;
			tpxy2=pxyg; dy=ncy2-mcy2;
		}
		if(db>dy)
		{
			mcr2=mcr3b1; mcg2=mcg3b1; mcb2=mcb3b1; mcy2=mcy3b1;
			ncr2=ncr3b1; ncg2=ncg3b1; ncb2=ncb3b1; ncy2=ncy3b1;
			tpxy2=pxyb; dy=ncy2-mcy2;
		}

#if 1
		dy=ncy1-mcy1;
//		j=dy>>1;
		j=(96*dy)>>8;
		tcr0=acr0-j;	tcg0=acg0-j;	tcb0=acb0-j;
		tcr1=acr0+j;	tcg1=acg0+j;	tcb1=acb0+j;
//		tcr0=(mcr1+tcr0)>>1; tcg0=(mcg1+tcg0)>>1; tcb0=(mcb1+tcb0)>>1;
//		tcr1=(ncr1+tcr1)>>1; tcg1=(ncg1+tcg1)>>1; tcb1=(ncb1+tcb1)>>1;
		tcr0=(3*mcr1+tcr0)>>2; tcg0=(3*mcg1+tcg0)>>2; tcb0=(3*mcb1+tcb0)>>2;
		tcr1=(3*ncr1+tcr1)>>2; tcg1=(3*ncg1+tcg1)>>2; tcb1=(3*ncb1+tcb1)>>2;

		if((tcr0|tcg0|tcb0|tcr1|tcg1|tcb1)>>8)
		{	tcr0=clamp255(tcr0);	tcg0=clamp255(tcg0);
			tcb0=clamp255(tcb0);	tcr1=clamp255(tcr1);
			tcg1=clamp255(tcg1);	tcb1=clamp255(tcb1);	}

		mcr1=tcr0;	mcg1=tcg0;	mcb1=tcb0;
		ncr1=tcr1;	ncg1=tcg1;	ncb1=tcb1;

		dy=ncy2-mcy2;
//		j=dy>>1;
		j=(96*dy)>>8;
		tcr0=acr1-j;	tcg0=acg1-j;	tcb0=acb1-j;
		tcr1=acr1+j;	tcg1=acg1+j;	tcb1=acb1+j;
//		tcr0=(mcr2+tcr0)>>1; tcg0=(mcg2+tcg0)>>1; tcb0=(mcb2+tcb0)>>1;
//		tcr1=(ncr2+tcr1)>>1; tcg1=(ncg2+tcg1)>>1; tcb1=(ncb2+tcb1)>>1;
		tcr0=(3*mcr2+tcr0)>>2; tcg0=(3*mcg2+tcg0)>>2; tcb0=(3*mcb2+tcb0)>>2;
		tcr1=(3*ncr2+tcr1)>>2; tcg1=(3*ncg2+tcg1)>>2; tcb1=(3*ncb2+tcb1)>>2;

		if((tcr0|tcg0|tcb0|tcr1|tcg1|tcb1)>>8)
		{	tcr0=clamp255(tcr0);	tcg0=clamp255(tcg0);
			tcb0=clamp255(tcb0);	tcr1=clamp255(tcr1);
			tcg1=clamp255(tcg1);	tcb1=clamp255(tcb1);	}

		mcr2=tcr0;	mcg2=tcg0;	mcb2=tcb0;
		ncr2=tcr1;	ncg2=tcg1;	ncb2=tcb1;
#endif
	}

#if 0
	mcr=mcr3r; mcg=mcg3r; mcb=mcb3r; mcy=mcy3r;
	ncr=ncr3r; ncg=ncg3r; ncb=ncb3r; ncy=ncy3r;
	tpxy=pxyr; dy=ncy-mcy;

	if(dg>dy)
	{
		mcr=mcr3g; mcg=mcg3g; mcb=mcb3g; mcy=mcy3g;
		ncr=ncr3g; ncg=ncg3g; ncb=ncb3g; ncy=ncy3g;
		tpxy=pxyg; dy=ncy-mcy;
	}
	if(db>dy)
	{
		mcr=mcr3b; mcg=mcg3b; mcb=mcb3b; mcy=mcy3b;
		ncr=ncr3b; ncg=ncg3b; ncb=ncb3b; ncy=ncy3b;
		tpxy=pxyb; dy=ncy-mcy;
	}
#endif

//	for(i=0; i<16; i++)
//		{ pxy[i]=tpxy[i]; }
	memcpy(pxy, tpxy, 16);

	min_rgba[0]=mcr;			min_rgba[1]=mcg;
	min_rgba[2]=mcb;			min_rgba[3]=mca;
	max_rgba[0]=ncr;			max_rgba[1]=ncg;
	max_rgba[2]=ncb;			max_rgba[3]=nca;
	
	*rmcy=mcy; *rncy=ncy;
	*rmca=mca; *rnca=nca;
	
	if(pn>=0)
	{
		tpart=bgbbtj_bc7_partition2+(pn*16);
		for(i=0; i<16; i++)
			{ pxy2[i]=tpart[i]?tpxy2[i]:tpxy1[i]; }

		min_rgba[ 4]=mcr1;			min_rgba[ 5]=mcg1;
		min_rgba[ 6]=mcb1;			min_rgba[ 7]=mca;
		max_rgba[ 4]=ncr1;			max_rgba[ 5]=ncg1;
		max_rgba[ 6]=ncb1;			max_rgba[ 7]=nca;
		min_rgba[ 8]=mcr2;			min_rgba[ 9]=mcg2;
		min_rgba[10]=mcb2;			min_rgba[11]=mca;
		max_rgba[ 8]=ncr2;			max_rgba[ 9]=ncg2;
		max_rgba[10]=ncb2;			max_rgba[11]=nca;

		rmcy[1]=mcy1; rncy[1]=ncy1;
		rmcy[2]=mcy2; rncy[2]=ncy2;
		rmcy[3]=mcu; rncy[3]=ncu;
		rmcy[4]=mcv; rncy[4]=ncv;
	}
}
