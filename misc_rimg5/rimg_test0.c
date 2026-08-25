#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <time.h>


#ifndef BTIC2F_BYTE
#define BTIC2F_BYTE
typedef unsigned char			byte;
typedef unsigned short		u16;
typedef unsigned int			u32;
typedef unsigned long long	u64;

typedef signed char			sbyte;
typedef signed short			s16;
typedef signed int			s32;
typedef signed long long		s64;
#endif

#include "bt1h_targa.c"

double checkrmse(byte *ibuf1, byte *ibuf2, int xs, int ys)
{
	double e, er, eg, eb;
	int cr0, cg0, cb0;
	int cr1, cg1, cb1;
	int dr, dg, db;
	int n;
	int i, j, k;
	
	er=0; eg=0; eb=0;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=(i*xs+j)*4;
		cr0=ibuf1[k+0];	cg0=ibuf1[k+1];	cb0=ibuf1[k+2];
		cr1=ibuf2[k+0];	cg1=ibuf2[k+1];	cb1=ibuf2[k+2];
		dr=cr1-cr0;		dg=cg1-cg0;		db=cb1-cb0;
		er+=dr*dr;
		eg+=dg*dg;
		eb+=db*db;
	}
	
	e=(er+eg+eb)/3;
	n=xs*ys;
	printf("RMSE: Er=%.3f Eg=%.3f Eb=%.3f Eavg=%.3f\n",
		sqrt(er/n), sqrt(eg/n), sqrt(eb/n), sqrt(e/n));

	e=sqrt(e/n);
	return(e);
}

byte *TgvLz_LoadFile(char *name, int *rsz)
{
	byte *buf;
	FILE *fd;
	int sz, i;
	
	fd=fopen(name, "rb");
	if(!fd)
	{
		return(NULL);
	}
	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);
	buf=malloc(sz+24);
	i=fread(buf, 1, sz, fd);
	fclose(fd);
	
	if(i!=sz)
	{
		if(i>0)
		{
			sz=i;
		}else
		{
			free(buf);
			return(NULL);
		}
	}

	memset(buf+sz, 0, 16);
	
	*rsz=sz;
	return(buf);
}

int TgvLz_StoreFile(char *name, void *buf, int sz)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)
		return(-1);
	
	fwrite(buf, 1, sz, fd);
	fclose(fd);
	return(0);
}

int RImg5_UnpackBlock(byte *ibuf, u16 *obuf)
{
	static const u64 pxm4=0x0001000100010001ULL;
	static const u64 pxm2=0x0000000000010001ULL;
	static const u16 rgbitab[16]={
		0x0000, 0x0015, 0x02A0, 0x02B5,
		0x5400, 0x5415, 0x5540, 0x56B5,
		0x294A, 0x295F, 0x2BEA, 0x2BFF,
		0x7D4A, 0x8000, 0x7FEA, 0x7FFF};
	u16 pxa[16];
	byte *cs;
	u16 *ct, *cte;
	u16 pxl, pxr, ml, md;
	u64 pxv, pxv1;
	
	cs=ibuf;
	ct=obuf;
	cte=obuf+256;
	pxr=0;
	while(ct<cte)
	{
		pxv=*(u64 *)cs;

#if 0
		if(!(pxv&pxm4))
		{
			pxv1=pxv>>1;
			*(u64 *)ct=pxv1;
			pxl=pxv1>>48;
			pxr=(pxr-4)&15;
			cs+=8; ct+=4;
//			pxa[pxr]=pxl;
			pxa[(pxr+0)&15]=pxv1>>48;
			pxa[(pxr+1)&15]=pxv1>>32;
			pxa[(pxr+2)&15]=pxv1>>16;
			pxa[(pxr+3)&15]=pxv1>> 0;
			continue;
		}
		if(!(pxv&pxm2))
		{
			pxv1=((u32)pxv)>>1;
			*(u32 *)ct=pxv1;
			pxl=pxv1>>16;
			pxr=(pxr-2)&15;
			cs+=4; ct+=2;
//			pxa[pxr]=pxl;
			pxa[(pxr+0)&15]=pxv1>>16;
			pxa[(pxr+1)&15]=pxv1>> 0;
			continue;
		}
#endif

		if(!(pxv&1))
		{
			pxl=((u16)pxv)>>1;
			cs+=2;
			pxr=(pxr-1)&15;
			*ct++=pxl;
			pxa[pxr]=pxl;
			continue;
		}
		
		if(!(pxv&2))
		{
			if(!(pxv&0xC))
			{
				cs+=2;
				ml=(pxv>> 4)&255;
				md=(pxv>>12)& 15;
				if(!md)
				{
					if(!ml)
					{
						pxl=0x8000;
						pxr=(pxr-1)&15;
						*ct++=pxl;
						pxa[pxr]=pxl;
						continue;
					}
					while(ml--)
						*ct++=pxl;
					continue;
				}
				while(ml--)
					*ct++=*(ct-md);
				continue;
			}else
				if((pxv&0xC)==0x04)
			{
				ml=(pxv>>4)& 15;
				md=(pxv>>8)&255;
				cs+=2;
				while(ml--)
					*ct++=*(ct-md);
				continue;
			}
		}else
		{
			if(!(pxv&0x0C))
			{
				pxl=pxa[(pxr+(pxv>>4))&15];
				cs++;
				*ct++=pxl;
				continue;
			}else if(!(pxv&0x04))
			{
				ml=(pxv>>4)&15;
				if(!ml)
				{
					while(ct<cte)
						*ct++=pxl;
					break;
				}
				while(ml--)
					*ct++=pxl;
				cs++;
				continue;
			}else if(!(pxv&0x08))
			{
				pxl=rgbitab[(pxv>>4)&15];
				cs++;
				*ct++=pxl;
				continue;
			}
		}
	}
	return(cs-ibuf);
}

int RImg5_PackBlock(byte *cbuf, u16 *ibuf)
{
	static const u16 rgbitab[20]={
		0x0000, 0x0015, 0x02A0, 0x02B5,
		0x5400, 0x5415, 0x5540, 0x56B5,
		0x294A, 0x295F, 0x2BEA, 0x2BFF,
		0x7D4A, 0x8000, 0x7FEA, 0x7FFF,
		0x80FF, 0x80FF, 0x80FF, 0x80FF};
	static const signed char rgbihash[64]={
		 0, 16,  9,  2, 16, 11, 16,  1,
		16, 16,  3, 16, 12, 16, 16, 14,
		16,  4, 16, 16, 16, 16, 15, 13,
		 5, 16,  8,  7, 16,  6, 10, 16,
		 0, 16,  9,  2, 16, 11, 16,  1,
		16, 16,  3, 16, 12, 16, 16, 14,
		16,  4, 16, 16, 16, 16, 15, 13,
		 5, 16,  8,  7, 16,  6, 10, 16
	};
	u16 pxa[16];
	byte pxah[64];
	byte pxoh[64];
	byte *ct;
	u16 *cs, *cse;
	u16 pxl, pxr, pxc, ml, md, mo, mrle;
	u64 pxv;
	int i, j, k, h;
	
	for(i=0; i<16; i++)
		pxa[i]=0x80FF;
//	for(i=0; i<32; i++)
//		pxah[i]=0;
	memset(pxah, 0, 64);
	memset(pxoh, 0, 64);
	
	ct=cbuf;		cs=ibuf;
	cse=ibuf+256;	pxr=0;
	pxl=0x80FF;		mrle=0;
	while(cs<cse)
	{
		pxc=*cs;
		if(pxc&0x8000)
			pxc=0x8000;
		if((pxc==pxl) && (mrle<254))
			{ mrle++; cs++; continue; }
		if(mrle)
		{
			if(mrle<=15)
			{
				*ct++=0x0B|(mrle<<4);
			}else
			{
				*ct++=0x01|(mrle<<4);
				*ct++=0x10|(mrle>>4);
			}
//			cs++;
			mrle=0;
			continue;
		}
		
		h=((pxc*22190+1842)>>16)&63;

		mo=pxoh[h];
		md=(cs-ibuf)-mo;
		if((md>1) && (ibuf[mo]==pxc))
		{
			for(ml=0; (cs+ml)<cse; ml++)
				if(cs[ml]!=ibuf[mo+ml])
					break;
			if(ml>254)
				ml=254;
			if(ml>3)
			{
				if(md>15)
				{
					if(ml>15)
						ml=15;
				}
			
				if((ml<256) && (md<16))
				{
					j=0x0001|(ml<<4)|(md<<12);
					*ct++=(j>>0)&255;
					*ct++=(j>>8)&255;
				}else
					if((ml<16) && (md<256))
				{
					j=0x0005|(ml<<4)|(md<<8);
					*ct++=(j>>0)&255;
					*ct++=(j>>8)&255;
				}else
				{
					__debugbreak();
				}
				
				while(ml--)
				{
					pxc=*cs++;
					h=((pxc*22190+1842)>>16)&63;
					pxoh[h]=cs-ibuf;
				}
				continue;
			}
		}

		pxoh[h]=cs-ibuf;

		j=pxah[h];
		if(pxa[j]==pxc)
			{ i=(j-pxr)&15; }
		else
		{
			i=16;
#if 1
			for(i=0; i<16; i++)
				if(pxa[(pxr+i)&15]==pxc)
					break;
			if(i<16)
				{ pxah[h]=(pxr+i)&15; }
#endif
		}
		if(i<16)
		{
			*ct++=0x03|(i<<4);
			pxl=pxc;
			cs++;
			continue;
		}

		i=rgbihash[h];
		if(rgbitab[i]!=pxc)
			i=16;
//		for(i=0; i<16; i++)
//			if(rgbitab[i]==pxc)
//				break;
		if(i<16)
		{
			*ct++=0x07|(i<<4);
			pxl=pxc;
			cs++;
			continue;
		}
		
		j=(pxc<<1)&0xFFFE;
		if(pxc==0x8000)
			j=0x0001;
		*ct++=(j>>0)&255;
		*ct++=(j>>8)&255;

		pxr=(pxr-1)&15;
		pxl=pxc;
		cs++;
		pxa[pxr]=pxc;
		pxah[h]=pxr;
	}
	
	if(mrle)
	{
		if(mrle<=15)
		{
			*ct++=0x0B|(mrle<<4);
		}else
		{
			*ct++=0x0B|(0<<4);
		}
	}
	return(ct-cbuf);
}

int RImg5_CheckBlockFlat(u16 *ibuf)
{
	int px;
	int i, j, k;
	px=ibuf[0];
	for(i=1; i<256; i++)
		if(ibuf[i]!=px)
			break;
	if(i<256)
		return(-1);
	return(px);
}

int RImg5_GetBlockPx(u16 *blkbuf, int cx, int cy, u16 *ibuf, int xs, int ys)
{
	int px, py, x, y, z, pv;
	
	if((cx<0) || (cy<0) || ((cx+16)>xs) || ((cy+16)>ys))
	{
		for(z=0; z<256; z++)
			blkbuf[z]=0x0000;
		if(((cx+15)<0) || ((cy+15)<0) || (cx>=xs) || (cy>=ys))
			return(0);

		for(py=0; py<16; py++)
		{
			y=cy+py;
			if(y<0)
				continue;
			if(y>=ys)
				break;
			for(px=0; px<16; px++)
			{
				x=cx+px;
				if(x<0)
					continue;
				if(x>=xs)
					break;
				pv=ibuf[y*xs+x];
				if(pv&0x8000)
					pv=0x8000;
				blkbuf[py*16+px]=pv;
			}
		}
		return(1);
	}
	z=(cy*xs+cx);
	for(py=0; py<256; py+=16)
		{ memcpy(blkbuf+py, ibuf+z, 16*2); z+=xs; }
	return(1);
}

int RImg5_PutBlockPx(
	u16 *blkbuf, int cx, int cy, u16 *ibuf, int xs, int ys,
	int bbxo, int bbyo, int bbxh, int bbyh)
{
	u16 *cs, *ct;
	int px, py, pz, x, y, z, pv;
	
	if(bbxo<0)		bbxo=0;
	if(bbyo<0)		bbyo=0;
	if(bbxh>xs)		bbxh=xs;
	if(bbyh>ys)		bbyh=ys;
	
	if((cx<bbxo) || (cy<bbyo) || ((cx+16)>bbxh) || ((cy+16)>bbyh))
	{
		if(((cx+15)<bbxo) || ((cy+15)<bbyo) || (cx>=bbxh) || (cy>=bbyh))
			return(0);

		/* partially outside target image */
		for(py=0; py<16; py++)
		{
			y=cy+py;
			if(y<bbyo)
				continue;
			if(y>=bbyh)
				break;
			for(px=0; px<16; px++)
			{
				x=cx+px;
				if(x<bbxo)
					continue;
				if(x>=bbxh)
					break;
				pv=blkbuf[py*16+px];
				ibuf[y*xs+x]=pv;
			}
		}
		return(1);
	}

	z=(cy*xs+cx);
//	for(pz=0; pz<256; pz+=16)
//		{ memcpy(ibuf+z, blkbuf+pz, 16*2); z+=xs; }

#if 1
	cs=blkbuf;	ct=ibuf+z;
	for(y=0; y<4; y++)
	{
		memcpy(ct, cs, 16*2); ct+=xs; cs+=16;
		memcpy(ct, cs, 16*2); ct+=xs; cs+=16;
		memcpy(ct, cs, 16*2); ct+=xs; cs+=16;
		memcpy(ct, cs, 16*2); ct+=xs; cs+=16;
	}
#endif

	return(1);
}

int RImg5_GetBlock(u16 *blkbuf, int cx, int cy, u16 *ibuf, int xs, int ys)
{
	return(RImg5_GetBlockPx(blkbuf, cx*16, cy*16, ibuf, xs, ys));
}

int RImg5_PutBlock(u16 *blkbuf, int cx, int cy, u16 *ibuf, int xs, int ys)
{
	return(RImg5_PutBlockPx(blkbuf, cx*16, cy*16, ibuf, xs, ys, 0, 0, xs, ys));
}

int RImg5_Hash8ForBuf(byte *buf, int sz)
{
	byte *cs, *cse;
	int sum0, sum1;

	cs=buf; cse=buf+sz;
	sum0=0;	sum1=0;
	while(cs<cse)
		{ sum1+=sum0; sum0+=*cs++; }
	sum0=((u16)sum0)+(sum0>>16);
	sum1=((u16)sum1)+(sum1>>16);
	sum0=((byte)sum0)+(sum0>>8);
	sum1=((byte)sum1)+(sum1>>8);
	return((sum0^sum1)&255);
}

int RImg5_PackImage(u16 *ibuf, int xs, int ys, byte *cbuf, u32 *ixbuf)
{
	u16 pxbuf[256], px2buf[512];
	u16 pxrhash[256];
	byte *ct, *ct1;
	int cx, cy, cz, cxs, cys, px, psz, poff, h, hi1, psz1, poff1;
	int i, j, k;
	
	cxs=(xs+15)>>4;
	cys=(ys+15)>>4;
	ct=cbuf;
	
	memset(pxrhash, 0, 256*2);
	
	for(cy=0; cy<cys; cy++)
		for(cx=0; cx<cxs; cx++)
	{
		RImg5_GetBlock(pxbuf, cx, cy, ibuf, xs, ys);
		
		px=RImg5_CheckBlockFlat(pxbuf);
		if(px>=0)
		{
			ixbuf[cy*cxs+cx]=px;
			continue;
		}
		
		psz=RImg5_PackBlock(ct, pxbuf);
		
		RImg5_UnpackBlock(ct, px2buf);
		
		if(memcmp(pxbuf, px2buf, 16*16*2))
		{
			__debugbreak();
		}
		
		cz=cy*cxs+cx;

		h=RImg5_Hash8ForBuf(ct, psz);
		hi1=pxrhash[h];

#if 1
		if(hi1<cz)
		{
			k=ixbuf[hi1];
			poff1=(k&0x000FFFFF)<<1;
			psz1=(k>>20)&0xFFF;
			ct1=cbuf+poff1;
			
			if((psz1==psz) && !memcmp(ct, ct1, psz))
			{
				ixbuf[cz]=0x10000|hi1;
				continue;
			}
		}
#endif

		ixbuf[cz]=((ct-cbuf)>>1)|(psz<<20);
		pxrhash[h]=cz;
		if(psz&1)
			psz++;
		ct+=psz;
	}
	return(ct-cbuf);
}

void RImg5_FillBlockFlat(u16 *pxbuf, int px)
{
	int j;
	for(j=0; j<256; j++)
		pxbuf[j]=px;
}

// static u16 rimg5_pxcache[65*256];
static u16 *rimg5_pxcache;
static byte *rimg5_pxcbuf[64];
static u32   rimg5_pxcofs[64];

u16 *RImg5_UnpackImageCachedBlock(byte *cbuf, u32 *ixbuf, int sz, int idx)
{
	u16 *pxbuf;
	int ix, px, poff, psz;
	int i, j, k, h;

	ix=idx;
	k=ixbuf[ix];

	if(!rimg5_pxcache)
		rimg5_pxcache=malloc(65*256*2);

	if((k>>16)==0)
	{
		px=k;
		h=((px*22190+1842)>>16)&63;
		pxbuf=rimg5_pxcache+h*256;
		if(!rimg5_pxcbuf[h] && (rimg5_pxcofs[h]==k))
			{ return(pxbuf); }
		RImg5_FillBlockFlat(pxbuf, px);
		rimg5_pxcbuf[h]=NULL;
		rimg5_pxcofs[h]=k;
		return(pxbuf);
	}

	if((k>>16)==1)
	{
		ix=k&0xFFFF;
		k=ixbuf[ix];
	}

	h=((ix*22190+1842)>>16)&63;
	pxbuf=rimg5_pxcache+h*256;

	if((rimg5_pxcbuf[h]==cbuf) && (rimg5_pxcofs[h]==k))
		{ return(pxbuf); }

	poff=(k&0x000FFFFF)<<1;
	psz=(k>>20)&0xFFF;

	RImg5_UnpackBlock(cbuf+poff, pxbuf);
	rimg5_pxcbuf[h]=cbuf;
	rimg5_pxcofs[h]=k;
	return(pxbuf);
}

int RImg5_UnpackImage(byte *cbuf, u32 *ixbuf, int sz,
	u16 *obuf, int xs, int ys)
{
	u16 *pxbuf;
	byte *ct;
	int cx, cy, cxs, cys;
	int i, j, k;
	
	cxs=(xs+15)>>4;
	cys=(ys+15)>>4;
	ct=cbuf;
	
	for(cy=0; cy<cys; cy++)
		for(cx=0; cx<cxs; cx++)
	{
		pxbuf=RImg5_UnpackImageCachedBlock(cbuf, ixbuf, sz, cy*cxs+cx);
		RImg5_PutBlock(pxbuf, cx, cy, obuf, xs, ys);
	}
	return(0);
}

/*
Unpack image while also copying it into a bounding rect within another image.
If one wants a version with a dirty mask, this is an excercise for the reader.
 */
int RImg5_UnpackImageBlit(
	byte *cbuf, u32 *ixbuf,
	int sz, int bxs, int bys,
	u16 *obuf, int oxs, int oys,
	int oxo, int oyo,
	int bbxo, int bbyo, int bbxh, int bbyh)
{
	u16 *pxbuf;
	byte *ct;
	int cx, cy, cxs, cys;
	int i, j, k;
	
	cxs=(bxs+15)>>4;
	cys=(bys+15)>>4;
	ct=cbuf;
	
	for(cy=0; cy<cys; cy++)
	{
		if((oyo+cy*16+16)<bbyo)
			continue;
		if((oyo+cy*16+ 0)>=bbyh)
			continue;
		for(cx=0; cx<cxs; cx++)
		{
			if((oxo+cx*16+16)<bbxo)
				continue;
			if((oxo+cx*16+ 0)>=bbxh)
				continue;
		
			pxbuf=RImg5_UnpackImageCachedBlock(cbuf, ixbuf, sz, cy*cxs+cx);
			RImg5_PutBlockPx(pxbuf, oxo+cx*16, oyo+cy*16, obuf, oxs, oys,
				bbxo, bbyo, bbxh, bbyh);
		}
	}
	return(0);
}


int print_usage(char *arg0)
{
	printf("Usage: %s infile outfile [opts*]\n", arg0);
	printf("  -q lvl      Quality in percentage, 100=lossless\n");
	printf("  -e          Encode Image\n");
	printf("  -d          Decode Image\n");
	return(0);
}

int main(int argc, char *argv[])
{
	static const u16 rgbitab[16]={
		0x0000, 0x0015, 0x02A0, 0x02B5,
		0x5400, 0x5415, 0x5540, 0x56B5,
		0x294A, 0x295F, 0x2BEA, 0x2BFF,
		0x7D4A, 0x8000, 0x7FEA, 0x7FFF};
	int rgbih[32];
	char *ifn, *ofn;
	byte *ibuf, *cbuf, *obuf;
	u16 *ibuf5, *obuf5;
	u32 *ixbuf;
	double f, g, h;
	long long tpix;
	int t0, t1, t2, t0e;
	int xs, ys, xs1, ys1, sz, qfl, md, cxs, cys;
	int i, j, k;

#if 0
	for(md=0; md<32; md++)
	{
		t0=rand();
		t1=rand();
	
		for(i=0; i<32; i++)
			{ rgbih[i]=-1; }
		for(i=0; i<16; i++)
		{
			j=rgbitab[i];
	//		k=((j*4093)>>12)&31;
//			k=((j*4091+11)>>12)&31;
//			k=((j*t0+t1)>>12)&31;
			k=((j*t0+t1)>>16)&31;
			
			if(rgbih[k]>=0)
			{
				printf("-%d- ", k);
				break;
			}
			else
			{
				printf(" %d  ", k);
				rgbih[k]=i;
			}
		}
		printf("\n");
		
		if(i>=16)
		{
			printf("t0=%d t1=%d\n", t0, t1);
			
			for(j=0; j<32; j++)
			{
				k=rgbih[j];
				if(k<0)
					k=16;
				printf("%d, ", k);
			}
			printf("\n");
			break;
		}
	}
#endif

	md=0;
	qfl=75;
//	qfl=100;

	ifn=NULL;
	ofn=NULL;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-e"))
			{
				md=1;
				continue;
			}

			if(!strcmp(argv[i], "-d"))
			{
				md=2;
				continue;
			}

			continue;
		}
		if(!ifn)
			{ ifn=argv[i]; continue; }
		if(!ofn)
			{ ofn=argv[i]; continue; }
	}

	if(md==0)
	{
		if(!ifn)
		{
			print_usage(argv[0]);
			return(0);
		}

		ibuf5=BTIC1H_Img_LoadTGA555(ifn, &xs, &ys);
		if(!ibuf5)
		{
			printf("Fail read TGA %s\n", ifn);
			return(0);
		}

		cxs=((xs+15)>>4);
		cys=((ys+15)>>4);
		ixbuf=malloc(cxs*cys*sizeof(u32));
		cbuf=malloc(1024+xs*ys*6);

		sz=RImg5_PackImage(ibuf5, xs, ys, cbuf, ixbuf);
		printf("IxSz=%d PSz=%d\n", cxs*cys*4, sz);


		obuf5=malloc(cxs*cys*256*2);

		RImg5_UnpackImage(cbuf, ixbuf, sz, obuf5, xs, ys);
		BTIC1H_Img_SaveTGA555("rimg_out0.tga", obuf5, xs, ys);

		if(memcmp(ibuf5, obuf5, xs*ys*2))
		{
			printf("Image Mismatch\n");
			return(1);
		}

		printf("Decode Bench:\n");

		tpix=0;
		t0=clock();
		t1=t0;
		t0e=t0+5*CLOCKS_PER_SEC;
		while(t1<t0e)
		{
			RImg5_UnpackImage(cbuf, ixbuf, sz, obuf5, xs, ys);
			tpix+=xs*ys;
			t1=clock();
		}
		
		printf("%f Mpix/sec\n", tpix/((1000000.0*(t1-t0))/CLOCKS_PER_SEC));
		return(0);
	}
}
