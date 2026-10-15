#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BTM_MAGIC1	0x12345678
#define debug_break __debugbreak();

#include "gfxedit_minrp2.c"

#define get_u16le	gfxedit_getu16
#define get_u32le	gfxedit_getu32

#define btm_malloc		malloc
#define bccx_strdup		strdup

byte *TgvLz_LoadFile(char *name, int *rsz);
#define BTM_LoadFileTmp		TgvLz_LoadFile


#include "btm_tkmod.h"

#include "mod_adpcm.c"

#include "btm_loadmod.c"

#include "tfau_multi.c"

byte *PadAlignBuffer(byte *ct, byte *obuf, int al)
{
	int ofs;
	ofs=ct-obuf;
	ofs=(ofs+(al-1))&(~(al-1));
	return(obuf+ofs);
}

int PackMod(TkMod_Info *mod, byte *obuf, int obsz)
{
	static byte period2note[4096];
	static byte planebuf[64*5*32];
	static byte cplanebuf[64*5*32];
	static short s3m_noteperiod[16]={
		1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907};
	static int init_period;
	byte nci[32];

	byte *ct, *pcs;
	byte *ptr_rvas_pat;
	byte *ptr_rvas_samp;
	u32 chanmsk;
	int c_sn, c_pd, c_ef, c_vl;
	int c_ex, c_fr;
	int ls, le, ll, lk, hz, bi, bd, d, sfl, sln;
	int i, j, k;
	
	if(!init_period)
	{
		init_period=1;
		
		for(ls=0; ls<4096; ls++)
		{
			bi=0; bd=9999999;
			for(lk=0; lk<256; lk++)
			{
				j=(lk>>4)&15;
				k=(lk&15);
				if(k>=12)
					continue;
				hz=(16LL*s3m_noteperiod[k])>>j;
				c_pd=hz/4;
				d=c_pd-ls;
				d^=d>>31;
				
//				if(!hz)
//					continue;
				
				if(d<bd)
					{ bi=lk; bd=d; }
			}
			period2note[ls]=bi;
		}
		period2note[0]=0xFF;
	}
	
	chanmsk=0; k=0;
	for(i=0; i<32; i++)
	{
		if(mod->chan_remap[i]!=0xFF)
		{
			chanmsk|=1U<<i;
		}
	}

	//00, magic
	//04, num patch-samples
	//06, num patterns
	//08, rva patch-samples
	//0C, rva pattern offsets
	//10, chanmask
	//14, speed
	//15, tempo / bpm
	//16, order-count
	//18, rva orders
	//1C, rva songname

	memset(obuf, 0, obsz);

	obuf[0x00]='T';
	obuf[0x01]='K';
	obuf[0x02]='M';
	obuf[0x03]='T';
	obuf[0x04]=(mod->nSamp)>>0;
	obuf[0x05]=(mod->nSamp)>>8;
	obuf[0x06]=(mod->nPat )>>0;
	obuf[0x07]=(mod->nPat )>>8;

	gfxedit_setu32(obuf+0x10, chanmsk);
	
	obuf[0x14]=mod->tick_div;
	obuf[0x15]=mod->tick_bpm;
	
	j=mod->hed2.num_positions;
	obuf[0x16]=j>> 0;	obuf[0x17]=j>> 8;
	
	ct=obuf+0x40;

	k=ct-obuf;
	obuf[0x1C]=k>> 0;	obuf[0x1D]=k>> 8;
	obuf[0x1E]=k>>16;	obuf[0x1F]=k>>24;

	j=strlen(mod->modname)+1;
	memcpy(ct, mod->modname, j);
	ct+=j;
	
	ct=PadAlignBuffer(ct, obuf, 2);
	k=ct-obuf;
	obuf[0x18]=k>> 0;	obuf[0x19]=k>> 8;
	obuf[0x1A]=k>>16;	obuf[0x1B]=k>>24;
	
	if(mod->nPat<254)
	{
		k=mod->hed2.num_positions;
		for(i=0; i<k; i++)
			*ct++=mod->hed2.pattern_tab[i];
	}else
	{
		k=mod->hed2.num_positions;
		for(i=0; i<k; i++)
		{	j=mod->hed2.pattern_tab[i];
			*ct++=j>>0; *ct++=j>>8;		}
	}

	ct=PadAlignBuffer(ct, obuf, 4);

	ptr_rvas_pat=ct;
	gfxedit_setu32(obuf+0x0C, ct-obuf);
	ct+=(mod->nPat+1)*4;
	
	for(i=0; i<mod->nPat; i++)
	{
		memset(planebuf, 0xFF, 64*5*32);
		for(j=0; j<64; j++)
		{
			pcs=mod->patterns+
				(i*mod->patstride)+
				(j*mod->rowstride);
			for(k=0; k<mod->n_chan_remap; k++)
			{			
				if(mod->colstride==4)
				{
					/* Convert from MOD form */
					c_sn=(pcs[0]&0xF0)|(pcs[2]>>4);		//patch/sample
					c_pd=((pcs[0]&0x0F)<<8)|pcs[1];		//period
					c_ef=(pcs[2]&0x0F)|pcs[3];			//effect
					c_vl=0xFF;		//reuse last volume by default
					
					if((c_ef>>4)==0xB)
					{
						c_ef=0x0300|(((c_ef&63)/10)<<4)|((c_ef&63)%10);
					}else
						if((c_ef>>4)==0xC)
					{
						c_vl=c_ef&255;
						c_ef=-1;
					}else
					{
						c_ef=-1;
					}

					hz=period2note[c_pd];

//					if((c_sn==0) && (c_pd==0))
					if((c_sn==0) || (c_pd==0))
					{
						c_sn=-1;
						hz=-1;
					}

					planebuf[(k*64*5)+(0*64)+j]=hz;
					planebuf[(k*64*5)+(1*64)+j]=c_sn;
					planebuf[(k*64*5)+(2*64)+j]=c_ef>>8;
					planebuf[(k*64*5)+(3*64)+j]=c_ef&0xFF;
					planebuf[(k*64*5)+(4*64)+j]=c_vl;
				}else
					if(mod->colstride==6)
				{
					planebuf[(k*64*5)+(0*64)+j]=pcs[0];		//note
					planebuf[(k*64*5)+(1*64)+j]=pcs[1];		//instrument
					planebuf[(k*64*5)+(2*64)+j]=pcs[2];		//command
					planebuf[(k*64*5)+(3*64)+j]=pcs[3];		//info
					planebuf[(k*64*5)+(4*64)+j]=pcs[4];		//voume
				}
				pcs+=mod->colstride;
			}
		}
		
		k=GfxEdit_EncodeRP2(cplanebuf, planebuf,
			64*5*32, 64*5*mod->n_chan_remap);
		gfxedit_setu32(ptr_rvas_pat+i*4, ct-obuf);
		memcpy(ct, cplanebuf, k);
		ct+=k;
	}
	gfxedit_setu32(ptr_rvas_pat+i*4, ct-obuf);

	ct=PadAlignBuffer(ct, obuf, 4);

	ptr_rvas_samp=ct;
	gfxedit_setu32(obuf+0x08, ct-obuf);
//	ct+=mod->nSamp*4;

	for(i=0; i<mod->nSamp; i++)
	{
		//00..03: RVA Data
		//04..07: Length
		//08..0B: Loop Start
		//0C..0F: Loop End
		//10..11: Rate
		//    12: Volume
		//    13: Loop Flags
		//14..1B: Reserved
		//1C..1F: RVA Name

		sfl=0;
		sln=mod->samp_samplen[i];
		if(sln>8192)
//		if(sln>16384)
			sfl|=4;
	
		ls=mod->samp_repeat_start[i];
		ll=mod->samp_repeat_len[i];
		le=ls+ll;
		
		if(ls && ll)
			sfl|=1;
		
		
		gfxedit_setu32(ct+0x04, sln);
		gfxedit_setu32(ct+0x08, ls);
		gfxedit_setu32(ct+0x0C, le);
		gfxedit_setu16(ct+0x10, mod->samp_c2hz[i]);
		ct[0x12]=mod->samp_volume[i];
		ct[0x13]=sfl;
		ct+=0x20;
	}

	for(i=0; i<mod->nSamp; i++)
	{
		if(mod->samp_name[i])
		{
			gfxedit_setu32(ptr_rvas_samp+(i*0x20+0x1C), ct-obuf);
			k=strlen(mod->samp_name[i])+1;
			memcpy(ct, mod->samp_name[i], k);
		}
	}

	for(i=0; i<mod->nSamp; i++)
	{
		ct=PadAlignBuffer(ct, obuf, 4);
		gfxedit_setu32(ptr_rvas_samp+(i*0x20+0x00), ct-obuf);
		ll=mod->samp_samplen[i];
		le=(ll+255)/256;

		sfl=ptr_rvas_samp[i*0x20+0x13];

		lk=0;
		for(j=0; j<le; j++)
		{
			if(!(sfl&4))
			{
				BGBCC_MsImaAdpcm_EncodeBlock8Mono(
					mod->sampdat[i]+j*256,
					ct, 256, &lk);
				ct+=132;
			}else
			{
				BGBCC_MsImaAdpcm_EncodeBlock8Mono2b(
					mod->sampdat[i]+j*256,
					ct, 256, &lk);
				ct+=68;
			}
		}
	}
	
	return(ct-obuf);
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
	
//	*(u64 *)(buf+sz+0)=0;
//	*(u64 *)(buf+sz+8)=0;
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


int main(int argc, char *argv[])
{
	static byte planebuf[64*5*32];
	static byte cplanebuf[64*5*32];
	char *ifn, *ofn;
	byte *obuf, *o2buf, *o3buf;
	byte *ibuf, *i2buf;
	byte *pcs;
	TkMod_Info *mod;
	TkMod_Info *mod2;
	s64 tot;
	int xs, ys, pix, isz, osz, offs, nblk, osz2;
	int hz, hz1;
	int t0, t1, t2, t0e, cnt, qf, bi, bd, bi1, bd1, md;
	int tpd_s3m;
	int tpd_rp2;
	int i, j, k, l;

	ifn=NULL;
	ofn=NULL;
	qf=90;
	md=0;

	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-q"))
			{
				qf=atoi(argv[i+1]);
				i++;
				continue;
			}

			if(!strncmp(argv[i], "-q=", 3))
			{
				qf=atoi(argv[i]+3);
				i++;
				continue;
			}

			if(!strcmp(argv[i], "-e"))
			{
				md=1;
			}

			if(!strcmp(argv[i], "-d"))
			{
				md=2;
			}

			if(!strcmp(argv[i], "-t"))
			{
				md=0;
			}
		
			continue;
		}
		
		if(!ifn)
		{
			ifn=argv[i];
			continue;
		}
		if(!ofn)
		{
			ofn=argv[i];
			continue;
		}
	}

	if(!ifn)
	{
		printf("No input file\n");
		printf("Usage: %s infile outfile args*\n", argv[0]);
//		printf("  -e      Encode image from TGA\n");
//		printf("  -d      Decode image to TGA\n");
//		printf("  -t      Test Encode then Decode\n");
//		printf("  -q val  Quality Level (0..100)\n");
//		printf("  -q=val  Quality Level (0..100) (Alternate)\n");
		return(-1);
	}

	if((md==1) && !ofn)
		md=0;

	mod=TkMod_GetMod(ifn);

	if(!mod)
		return(0);
	
	tot=0;

	printf("nSamp %d\n", mod->nSamp);
	for(i=0; i<mod->nSamp; i++)
	{
		l=mod->samp_samplen[i];
		t0=l*2;
		hz=mod->samp_c2hz[i];
		hz1=hz;
		if(hz1>16000)
			hz1=16000;
		t1=l*((0.5*hz1)/hz);
		printf("  len=%d hz=%d sz0=%d sz1=%d ls=%d ll=%d\n",
			l, hz, t0, t1,
				mod->samp_repeat_start[i],
				mod->samp_repeat_len[i]);
		tot+=t1;
	}
	printf("Tot Patch ADPCM %d\n", (int)tot);

	k=mod->nPat*mod->n_chan_remap*(64*5);
	printf("nPat=%d nChn=%d raw=%d\n", mod->nPat, mod->n_chan_remap, k);

	tpd_s3m=0;
	tpd_rp2=0;

	for(i=0; i<mod->nPat; i++)
	{
		memset(planebuf, 0xFF, 64*5*32);
		for(j=0; j<64; j++)
		{
			pcs=mod->patterns+i*mod->patstride+j*mod->rowstride;
			for(k=0; k<mod->n_chan_remap; k++)
			{
				l=0;
				if((pcs[0]!=0xFF) || (pcs[1]!=0xFF))
					l+=2;
				if((pcs[2]!=0xFF) || (pcs[3]!=0xFF))
					l+=2;
				if((pcs[4]!=0xFF))
					l++;
				if(l)
					l++;
				
				planebuf[(k*64*5)+(0*64)+j]=pcs[0];
				planebuf[(k*64*5)+(1*64)+j]=pcs[1];
				planebuf[(k*64*5)+(2*64)+j]=pcs[2];
				planebuf[(k*64*5)+(3*64)+j]=pcs[3];
				planebuf[(k*64*5)+(4*64)+j]=pcs[4];

				tpd_s3m+=l;
				pcs+=mod->colstride;
			}
		}
		
		k=GfxEdit_EncodeRP2(cplanebuf, planebuf,
			64*5*32, 64*5*mod->n_chan_remap);
		tpd_rp2+=k;
	}
	
	printf("  patS3M=%d patPlRp2=%d\n", tpd_s3m, tpd_rp2);

	l=(mod->songlen+1)*16000;
	mod->mixbuf=malloc(l*2);
	mod->mixpos=mod->mixbuf;
	mod->mixpos_cs=mod->mixbuf;
	mod->mixbufs=mod->mixbuf;
	mod->mixbufe=mod->mixbuf+l;

	while((mod->mixpos+mod->tick_divsamp)<mod->mixbufe)
	{
		TkMod_StepSongRow(mod);
		for(i=0; i<mod->tick_div; i++)
			TkMod_StepMixCtx(mod, mod->tick_samp);
	}

	BGBMID_StoreWAV("out0a.wav", (byte *)(mod->mixbuf), 1, 16000, 16, l);

	
	obuf=malloc(1<<22);
	
	osz=PackMod(mod, obuf, 1<<22);
	printf("Pack %d\n", osz);

	mod2=btm_malloc(sizeof(TkMod_Info));
	memset(mod2, 0, sizeof(TkMod_Info));

	TkMod_LoadFromBufferCtx_TKMT(mod2, obuf, osz);

//	l=16384;
//	mod2->mixbuf=malloc(l*2);
//	mod2->mixpos=mod2->mixbuf;
//	mod2->mixpos_cs=mod2->mixbuf;
//	mod2->mixbufs=mod2->mixbuf;
//	mod2->mixbufe=mod2->mixbuf+l;


	printf("Mod2:\n");
	printf("nSamp %d\n", mod2->nSamp);
	for(i=0; i<mod2->nSamp; i++)
	{
		l=mod2->samp_samplen[i];
		t0=l*2;
		hz=mod2->samp_c2hz[i];
		hz1=hz;
		if(hz1>16000)
			hz1=16000;
		t1=l*((0.5*hz1)/hz);
//		printf("  len=%d hz=%d sz0=%d sz1=%d\n",
//			l, hz, t0, t1);
		printf("  len=%d hz=%d sz0=%d sz1=%d ls=%d ll=%d\n",
			l, hz, t0, t1,
				mod2->samp_repeat_start[i],
				mod2->samp_repeat_len[i]);
	}

	k=mod2->nPat*mod2->n_chan_remap*(64*5);
	printf("nPat=%d nChn=%d raw=%d\n", mod2->nPat, mod2->n_chan_remap, k);


	l=(mod2->songlen+1)*16000;
	mod2->mixbuf=malloc(l*2);
	mod2->mixpos=mod2->mixbuf;
	mod2->mixpos_cs=mod2->mixbuf;
	mod2->mixbufs=mod2->mixbuf;
	mod2->mixbufe=mod2->mixbuf+l;

	while((mod2->mixpos+mod2->tick_divsamp)<mod2->mixbufe)
	{
		TkMod_StepSongRow(mod2);
		for(i=0; i<mod2->tick_div; i++)
			TkMod_StepMixCtx(mod2, mod2->tick_samp);
	}

	BGBMID_StoreWAV("out0.wav", (byte *)(mod2->mixbuf), 1, 16000, 16, l);



#if 0
	ibuf=TgvLz_LoadFile(ifn, &isz);

	if(!ibuf)
	{
		printf("Failed Load %s\n", ifn);
		return(-1);
	}

	obuf=malloc(2*isz+1024);
	o2buf=malloc(2*isz+1024);
	o3buf=malloc(2*isz+1024);
	i2buf=malloc(isz+1024);

//	osz=GfxEdit_EncodeRP2(obuf, ibuf, 2*isz+1024, isz);
	osz=PostRp2Stf_EncodeRP2Test(obuf, ibuf, 2*isz+1024, isz);
	printf("%d: isz -> osz = %d -> %d\n", i, isz, osz);

//	osz2=PostRp2Huff_EncodeBufferPostRp2Test(
//	osz2=BTM_BItRc_EncodeBufferPostRp2Test(
//	osz2=BTM_StfRk_EncodeBufferPostRp2Test(
//		o2buf, obuf, osz);

	osz2=PostRp2_EncodeBufferPostRp2Test(
		o2buf, obuf, 2*isz, osz, 0);

	printf("Huff isz -> osz = %d -> %d\n", osz, osz2);

	t0=clock();
	t1=t0;
	t2=t0+10*CLOCKS_PER_SEC;
	tot=0;
	while(t1<t2)
	{
#if 0
		PostRp2Huff_DecodeBufferPostRp2(o3buf, o2buf, osz2);
//		BTM_BItRc_DecodeBufferPostRp2(o3buf, o2buf, osz2);
//		BTM_StfRk_DecodeBufferPostRp2(o3buf, o2buf, osz2);
		GfxEdit_DecodeRP2(i2buf, o3buf, isz, osz);
#endif

//		PostRp2Huff_DecodeBufferRp2Full(o3buf, o2buf, 2*isz, osz2);
		PostRp2_DecodeBufferRp2Full(o3buf, o2buf, 2*isz, osz2);

//		tot+=osz;
		tot+=isz;
		t1=clock();
	}
	
	printf("%.2f MB/s\n", tot/(10.0*1024*1024));
#endif

	return(0);
}
