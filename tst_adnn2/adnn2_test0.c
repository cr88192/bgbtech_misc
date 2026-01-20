#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "adnn2_net.c"

static int adnn2_rle_lsym;
static int adnn2_rle_lcnt;

int adnn2_fputc_rawb(int val, FILE *fd)
{
	fputc(val, fd);
	return(0);
}

int adnn2_fputc_raww(int val, FILE *fd)
{
	fputc((val>>0)&0xFF, fd);
	fputc((val>>8)&0xFF, fd);
	return(0);
}

int adnn2_flush_raw(FILE *fd)
{
	return(0);
}

int adnn2_flush_rleb(FILE *fd)
{
	int i, j, k;

	while((adnn2_rle_lcnt>=2) ||
		((adnn2_rle_lcnt>=1) && (adnn2_rle_lsym==0x80)))
	{
		j=adnn2_rle_lcnt;
		if(j>127)
		{
			if((j>=128) && (j<132))
				{ j=124; }
			else
				{ j=127; }
		}
		if(adnn2_rle_lsym==0x10000)
		{
			fputc(0x80, fd);
			fputc(0x80+j, fd);
			adnn2_rle_lcnt-=j;
			continue;
		}
		fputc(0x80, fd);
		fputc(j, fd);
		adnn2_rle_lcnt-=j;
		continue;
	}

	while(adnn2_rle_lcnt>0)
	{
		if(adnn2_rle_lsym==0x10000)
		{
			fputc(0x00, fd);
		}else
			if(adnn2_rle_lsym==0x80)
		{
			fputc(0x80, fd);
			fputc(0x80, fd);
		}else
		{
			fputc(adnn2_rle_lsym, fd);
		}
		adnn2_rle_lcnt--;
	}

	adnn2_rle_lsym=-999;
	adnn2_rle_lcnt=0;
	return(0);
}

int adnn2_flush_rlew(FILE *fd)
{
	int i, j, k;

	while((adnn2_rle_lcnt>=2) ||
		((adnn2_rle_lcnt>=1) && ((adnn2_rle_lsym&0xFF00)==0xFF00)))
	{
		j=adnn2_rle_lcnt;
		if(j>127)
		{
			if((j>=128) && (j<132))		{ j=124; }
			else						{ j=127; }
		}
		if(adnn2_rle_lsym==0x10000)
		{
			adnn2_fputc_raww(0xFF80+j, fd);
			adnn2_rle_lcnt-=j;
			continue;
		}
		adnn2_fputc_raww(0xFF00+j, fd);
		adnn2_rle_lcnt-=j;
		continue;
	}

	while(adnn2_rle_lcnt>0)
	{
		if(adnn2_rle_lsym==0x10000)
		{
			adnn2_fputc_raww(0x0000, fd);
		}else
			if((adnn2_rle_lsym&0xFF00)==0xFF00)
		{
			if(adnn2_rle_lsym==0xFF00)
			{
				adnn2_fputc_raww(0xFF80, fd);
			}else
			{
				adnn2_fputc_raww(0xFF00, fd);
				adnn2_fputc_raww(adnn2_rle_lsym, fd);
			}
		}else
		{
			adnn2_fputc_raww(adnn2_rle_lsym, fd);
		}
		adnn2_rle_lcnt--;
	}

	adnn2_rle_lsym=-999;
	adnn2_rle_lcnt=0;
	return(0);
}

int adnn2_fputc_rleb(int val, FILE *fd)
{
	if((val==adnn2_rle_lsym) || (!val && (adnn2_rle_lsym==0x10000)))
	{
		adnn2_rle_lcnt++;
		return(0);
	}
	if(adnn2_rle_lcnt)
		{ adnn2_flush_rleb(fd); }
	
	if(val==0x80)
	{
		adnn2_rle_lsym=val;
		fputc(0x80, fd);
		fputc(0x80, fd);
		return(0);
	}
	
	if(val==0x00)
	{
		adnn2_rle_lsym=0x10000;
		adnn2_rle_lcnt=1;
		return(0);
	}
	
	adnn2_rle_lsym=val;
	fputc(val, fd);
	return(0);
}

int adnn2_fputc_rlew(int val, FILE *fd)
{
	if((val==adnn2_rle_lsym) || (!val && (adnn2_rle_lsym==0x10000)))
	{
		adnn2_rle_lcnt++;
		return(0);
	}
	if(adnn2_rle_lcnt)
		{ adnn2_flush_rlew(fd); }

	if((val&0xFF00)==0xFF00)
	{
		adnn2_rle_lsym=val;
		if(val==0xFF00)
		{
			adnn2_fputc_raww(0xFF80, fd);
		}else
		{
			adnn2_fputc_raww(0xFF00, fd);
			adnn2_fputc_raww(val, fd);
		}
		return(0);
	}
	
	if(val==0x0000)
	{
		adnn2_rle_lsym=0x10000;
		adnn2_rle_lcnt=1;
		return(0);
	}
	
	adnn2_rle_lsym=val;
	adnn2_fputc_raww(val, fd);
//	fputc((val>>0)&0xFF, fd);
//	fputc((val>>8)&0xFF, fd);
	return(0);
}


int adnn2_fgetc_rawb(FILE *fd)
{
	return(fgetc(fd));
}

int adnn2_fgetc_raww(FILE *fd)
{
	int i0, i1, i2;
	i0=fgetc(fd);
	i1=fgetc(fd);
	i2=i0|(i1<<8);
	return(i2);
}

int adnn2_fgetc_rleb(FILE *fd)
{
	int i, j, k;

	if(adnn2_rle_lcnt>0)
	{
		adnn2_rle_lcnt--;
		return(adnn2_rle_lsym);
	}
	
	i=adnn2_fgetc_rawb(fd);
	if(i!=0x80)
	{
		adnn2_rle_lsym=i;
		adnn2_rle_lcnt=0;
		return(i);
	}

	j=adnn2_fgetc_rawb(fd);
	if((j>=0x01) && (j<=0x7F))
	{
		adnn2_rle_lcnt=j-1;
		return(adnn2_rle_lsym);
	}

	if((j>=0x81) && (j<=0xFF))
	{
		adnn2_rle_lsym=0x00;
		adnn2_rle_lcnt=(j&0x7F)-1;
		return(adnn2_rle_lsym);
	}
	
	if(j==0x80)
	{
		adnn2_rle_lsym=j;
		adnn2_rle_lcnt=0;
		return(j);
	}
		
	i=adnn2_fgetc_rawb(fd);
	adnn2_rle_lsym=i;
	adnn2_rle_lcnt=0;
	return(i);
}

int adnn2_fgetc_rlew(FILE *fd)
{
	int i, j, k;

	if(adnn2_rle_lcnt>0)
	{
		adnn2_rle_lcnt--;
		return(adnn2_rle_lsym);
	}
	
	i=adnn2_fgetc_raww(fd);
	if((i&0xFF00)!=0xFF00)
	{
		adnn2_rle_lsym=i;
		adnn2_rle_lcnt=0;
		return(i);
	}

	j=i&0x00FF;
	if((j>=0x01) && (j<=0x7F))
	{
		adnn2_rle_lcnt=j-1;
		return(adnn2_rle_lsym);
	}

	if((j>=0x81) && (j<=0xFF))
	{
		adnn2_rle_lsym=0x0000;
		adnn2_rle_lcnt=(j&0x7F)-1;
		return(adnn2_rle_lsym);
	}
	
	if(j==0x80)
	{
		adnn2_rle_lsym=0xFF00;
		adnn2_rle_lcnt=0;
		return(j);
	}
	
	i=adnn2_fgetc_raww(fd);
	adnn2_rle_lsym=i;
	adnn2_rle_lcnt=0;
	return(i);
}

int AdNn2_DumpLayer(AdNn2_Layer *layer, FILE *fd, int flag)
{
	int (*rle_fputc_byte)(int val, FILE *fd);
	int (*rle_fputc_word)(int val, FILE *fd);
	int (*rle_flush_byte)(FILE *fd);
	int (*rle_flush_word)(FILE *fd);
	byte *cs, *cse;
	u16 *csw, *cswe;
	int isz, osz, sz, st4, isrle;
	int i, j, k, l;

	adnn2_rle_lsym=-999;
	adnn2_rle_lcnt=0;
	
	rle_fputc_byte=adnn2_fputc_rawb;
	rle_fputc_word=adnn2_fputc_raww;
	rle_flush_byte=adnn2_flush_raw;
	rle_flush_word=adnn2_flush_raw;
	isrle=0;
	
	if((flag&0x00E0)==0x0020)
	{
		rle_fputc_byte=adnn2_fputc_rleb;
		rle_fputc_word=adnn2_fputc_rlew;
		rle_flush_byte=adnn2_flush_rleb;
		rle_flush_word=adnn2_flush_rlew;
		isrle=1;
	}

	isz=layer->nn_isz;
	osz=layer->nn_osz;
	sz=osz*(isz+1);

#ifdef ADNN2_WGHT16
	if((flag&15)==0)
	{
		if(isrle)
		{
			st4=layer->nn_stw4;
			l=osz*st4;
			cs=layer->wght4;
			cse=cs+l;
			while(cs<cse)
				{ rle_fputc_byte(*cs++, fd); }
			rle_flush_byte(fd);
			return(0);
		}

		st4=layer->nn_stw4;
		fwrite(layer->wght4, 1, osz*st4, fd);
		return(0);
	}
	if((flag&15)==1)
	{
		if(isrle)
		{
			l=osz*(isz+1);
			cs=layer->wght8;
			cse=cs+l;
			while(cs<cse)
				{ rle_fputc_byte(*cs++, fd); }
			rle_flush_byte(fd);
			return(0);
		}

		fwrite(layer->wght8, 1, osz*(isz+1), fd);
		return(0);
	}
	if((flag&15)==2)
	{
		if(isrle)
		{
			l=osz*(isz+1);
			csw=layer->wght;
			cswe=csw+l;
			while(csw<cswe)
				{ rle_fputc_word(*csw++, fd); }
			rle_flush_word(fd);
			return(0);
		}

		fwrite(layer->wght, 1, osz*(isz+1)*sizeof(adnn2_wght), fd);
		return(0);
	}
	return(0);
#else
	if((flag&15)!=1)
		return(-1);
	if(isrle)
		return(-1);

	fwrite(layer->wght, 1, osz*(isz+1), fd);
	return(0);
#endif
}

int AdNn2_DumpLayerAsc(AdNn2_Layer *layer, FILE *fd)
{
	adnn2_wght *wght;
	byte *cs4;
	int isz, osz, st4;
	int i, j, k;

	isz=layer->nn_isz;
	osz=layer->nn_osz;
	wght=layer->wght;
//	fwrite(layer->wght, 1, osz*(isz+1)*2, fd);

	st4=layer->nn_stw4;

	for(i=0; i<osz; i++)
	{
		if(layer->wght4)
		{
			cs4=layer->wght4+(i*st4);

			fprintf(fd, "%02X %02X ", cs4[0], cs4[1]);

			for(j=0; j<(st4-2); j++)
			{
				k=cs4[2+j];
				k=((k&15)<<4)|((k>>4)&15);
				fprintf(fd, "%02X", k);
				
			}
			fprintf(fd, "\n");
			continue;
		}
	
		for(j=0; j<(isz+1); j++)
		{
			k=wght[i*(isz+1)+j];
			if(((k>>10)&31)<7)
				k=0;
//			fprintf(fd, "%04X ", k);

#ifdef ADNN2_FWWGHT8
#ifdef ADNN2_WGHT16
			k=AdNn2_Fp16to8(k);
#endif
			fprintf(fd, "%02X ", k);
#else
#ifdef ADNN2_WGHT16
			fprintf(fd, "%04X ", k);
#else
			fprintf(fd, "%02X ", k);
#endif
#endif

		}
		fprintf(fd, "\n");
	}

	return(0);
}

int AdNn2_CalcLayoutHash(AdNn2_Net *net)
{
	char tb[256];
	byte thd[256];
	AdNn2_Layer *layer;
	byte *ct, *cs;
	int i, j, k, h, nl, isz, osz;

	nl=net->n_layer;

	ct=thd;

	*ct++=sizeof(adnn2_wght);
	*ct++=0;

	*ct++=(nl>>0)&255;
	*ct++=(nl>>8)&255;

	for(i=0; i<nl; i++)
	{
		layer=net->layer[i];
		isz=layer->nn_isz;
		osz=layer->nn_osz;
		*ct++=(isz>>0)&255;
		*ct++=(isz>>8)&255;
		*ct++=(osz>>0)&255;
		*ct++=(osz>>8)&255;
	}
	
	h=0;
	cs=thd;
	while(cs<ct)
		h=h*251+(*cs++);
	h=h*251+1;
	return((h>>8)&0xFFFF);
}

int AdNn2_DumpNetAsc(AdNn2_Net *net)
{
	char tb[256];
	AdNn2_Layer *layer;
	FILE *fd;
	int nl, isz, osz, afn;
	int i, j, k, h;

	h=AdNn2_CalcLayoutHash(net);
	sprintf(tb, "adnn2_dump_%04X.txt", h);
	fd=fopen(tb, "wb");

//	fd=fopen("adnn2_dump.dat", "wb");

	nl=net->n_layer;

	fprintf(fd, "# NN2 ASC\n");

//	fprintf(fd, "SzWeight: %d\n", (int)sizeof(adnn2_wght));
#ifdef ADNN2_DOWGHTFP4
	fprintf(fd, "SzWeight: %d\n", 0);
#else
#ifdef ADNN2_FWWGHT8
	fprintf(fd, "SzWeight: %d\n", 1);
#else
	fprintf(fd, "SzWeight: %d\n", 2);
#endif
#endif

	fprintf(fd, "Flags: %d\n", 0);
	fprintf(fd, "NumLayers: %d\n", nl);

	for(i=0; i<nl; i++)
	{
		fprintf(fd, "[Layer_%u]\n", i);
		layer=net->layer[i];
		isz=layer->nn_isz;
		osz=layer->nn_osz;
		afn=layer->nn_afn;
		fprintf(fd, "ISz: %d\n", isz);
		fprintf(fd, "OSz: %d\n", osz);
		fprintf(fd, "AFn: %d\n", afn);
		AdNn2_DumpLayerAsc(net->layer[i], fd);
	}

	fclose(fd);
	return(0);
}

int AdNn2_DumpNet(AdNn2_Net *net, int ix, int fl)
{
	char tb[256];
	AdNn2_Layer *layer;
	FILE *fd;
	int nl, isz, osz, afn, wszfl;
	int i, j, k, h;

	h=AdNn2_CalcLayoutHash(net);

	sprintf(tb, "adnn2_dump_%04X_a%u.nn2", h, ix);
	if(fl&1)
	{
#ifdef ADNN2_FWWGHT8
		sprintf(tb, "adnn2_dump_%04X_a%u_f8.nn2", h, ix);
#endif
#ifdef ADNN2_DOWGHTFP4
		sprintf(tb, "adnn2_dump_%04X_a%u_f4.nn2", h, ix);
#endif
//		sprintf(tb, "adnn2_dump_%04X.dat", h);
	}

	fd=fopen(tb, "wb");

//	fd=fopen("adnn2_dump.dat", "wb");

	nl=net->n_layer;

	fputc('N', fd);
	fputc('N', fd);
	fputc('2', fd);
	fputc(' ', fd);

	wszfl=sizeof(adnn2_wght);

	if(fl&1)
	{
#ifdef ADNN2_FWWGHT8
		wszfl=1;
//		wszfl|=0x10;
#endif

#ifdef ADNN2_DOWGHTFP4
		wszfl=0;
#endif

		wszfl|=0x20;	//RLE
	}

	for(i=0; i<nl; i++)
	{
		layer=net->layer[i];
		isz=layer->nn_isz;
		osz=layer->nn_osz;
		afn=layer->nn_afn;
		if((isz>=65536) || (osz>=65536) || (afn!=0))
			wszfl|=0x10;
	}

	fputc((wszfl>>0)&255, fd);
	fputc((wszfl>>8)&255, fd);

	fputc((nl>>0)&255, fd);
	fputc((nl>>8)&255, fd);

	for(i=0; i<nl; i++)
	{
		layer=net->layer[i];
		isz=layer->nn_isz;
		osz=layer->nn_osz;
		afn=layer->nn_afn;
		fputc((isz>>0)&255, fd);
		fputc((isz>>8)&255, fd);
		fputc((osz>>0)&255, fd);
		fputc((osz>>8)&255, fd);

		if(wszfl&0x10)
		{
			fputc(afn, fd);
			fputc(0, fd);
			fputc(isz>>16, fd);
			fputc(osz>>16, fd);
		}
	}

	for(i=0; i<nl; i++)
		{ AdNn2_DumpLayer(net->layer[i], fd, wszfl); }
	fclose(fd);
	return(0);
}

int AdNn2_LoadNet(AdNn2_Net *net, int ix, int ldfl)
{
	char tb[256];
	int (*rle_fget_byte)(FILE *fd);
	int (*rle_fget_word)(FILE *fd);
	AdNn2_Layer *layer;
	FILE *fd;
	int nl, isz, osz, ixb;
	int i0, i1, i2, i3;
	int i, j, k, l, h, wsz, lfl, afn, bi8, sc8;

	AdNn2_Init();

	h=AdNn2_CalcLayoutHash(net);
	fd=NULL;

	if(ldfl&1)
	{
#ifdef ADNN2_DOWGHTFP4
		if(!fd)
		{
			sprintf(tb, "adnn2_dump_%04X_a%u_f4.nn2", h, ix);
			fd=fopen(tb, "rb");
		}
#endif

#ifdef ADNN2_FWWGHT8
		if(!fd)
		{
			sprintf(tb, "adnn2_dump_%04X_a%u_f8.nn2", h, ix);
			fd=fopen(tb, "rb");
		}
#endif
	}

	if(!fd)
	{
		sprintf(tb, "adnn2_dump_%04X_a%u.nn2", h, ix);
		fd=fopen(tb, "rb");
	}

	if(!fd)
	{
		sprintf(tb, "adnn2_dump_%04X.dat", h);
		fd=fopen(tb, "rb");
	}

	if(!fd)
		fd=fopen("adnn2_dump.dat", "rb");
	
	if(!fd)
		return(0);

	nl=net->n_layer;

	fgetc(fd);	fgetc(fd);
	fgetc(fd);	fgetc(fd);

	i0=fgetc(fd);	i1=fgetc(fd);
	i2=fgetc(fd);	i3=fgetc(fd);
	k=i2|(i3<<8);
	if(k!=nl)
		return(0);
		
	wsz=i0&15;
	lfl=(i1<<4)|(i0>>4);

	rle_fget_byte=adnn2_fgetc_rawb;
	rle_fget_word=adnn2_fgetc_raww;
	if((lfl&0x00E)==0x002)
	{
		rle_fget_byte=adnn2_fgetc_rleb;
		rle_fget_word=adnn2_fgetc_rlew;
	}

	for(i=0; i<nl; i++)
	{
		layer=net->layer[i];
		i0=fgetc(fd);	i1=fgetc(fd);
		i2=fgetc(fd);	i3=fgetc(fd);
		isz=i0|(i1<<8);
		osz=i2|(i3<<8);
		afn=0;

		if(lfl&1)
		{
			i0=fgetc(fd);	i1=fgetc(fd);
			i2=fgetc(fd);	i3=fgetc(fd);
			afn=i0;
			isz|=(i2<<16);
			osz|=(i3<<16);
		}
		
//		afn=4;

		if(isz!=layer->nn_isz)
			break;
		if(osz!=layer->nn_osz)
			break;
		
		layer->nn_afn=afn;
	}

	if(i<nl)
		return(0);

	adnn2_rle_lsym=0;
	adnn2_rle_lcnt=0;

	for(i=0; i<nl; i++)
	{
//		AdNn2_DumpLayer(net->layer[i], fd);
		layer=net->layer[i];
		isz=layer->nn_isz;
		osz=layer->nn_osz;
#ifdef ADNN2_WGHT16
		if(wsz==2)
		{
//			fread(layer->wght, 1, osz*(isz+1)*2, fd);

#if 1
			l=osz*(isz+1);
			for(j=0; j<l; j++)
			{
//				i0=fgetc(fd);	i1=fgetc(fd);
//				i2=i0|(i1<<8);
				i2=rle_fget_word(fd);
				i3=AdNn2_Fp16to8(i2);
				layer->wght[j]=i2;
				layer->wght8[j]=i3;

//				k =(fgetc(fd)&255)<<0;
//				k|=(fgetc(fd)&255)<<8;
//				k=AdNn2_Fp16to8(k);
//				k=adnn2_fp8to16[k&255];
//				layer->wght[j]=k;
			}
#endif
		}else if(wsz==1)
		{
			l=osz*(isz+1);
			for(j=0; j<l; j++)
			{
//				k=fgetc(fd);
				k=rle_fget_byte(fd);
				layer->wght8[j]=k;
				k=adnn2_fp8to16[k&255];
				layer->wght[j]=k;
			}
		}else if(wsz==0)
		{
//			l=osz*(isz+1);
			l=(isz+1)/2;
			for(j=0; j<osz; j++)
			{
//				bi8=fgetc(fd);
				bi8=rle_fget_byte(fd);
//				sc8=fgetc(fd);
				sc8=rle_fget_byte(fd);
				ixb=j*(isz+1);
				for(k=0; k<l; k++)
				{
//					i0=fgetc(fd);
					i0=rle_fget_byte(fd);

					i1=(i0>>0)&15;
					i2=(sc8^((i1<<4)&0x80))+(((i1&7)-4)<<2);
					if(!(i1&7))		i2=0;
					layer->wght8[ixb+0]=i2;
					i3=adnn2_fp8to16[i2&255];
					layer->wght[ixb+0]=i3;

					i1=(i0>>4)&15;
					i2=(sc8^((i1<<4)&0x80))+(((i1&7)-4)<<2);
					if(!(i1&7))		i2=0;
					layer->wght8[ixb+1]=i2;
					i3=adnn2_fp8to16[i2&255];
					layer->wght[ixb+1]=i3;
					
					ixb+=2;
				}

				ixb=(j*(isz+1))+isz;
				i2=bi8;
				layer->wght8[ixb]=i2;
				i3=adnn2_fp8to16[i2&255];
				layer->wght[ixb]=i3;
			}
		}
#else
		if(wsz==2)
		{
			l=osz*(isz+1);
			for(j=0; j<l; j++)
			{
//				i0=fgetc(fd);	i1=fgetc(fd);
//				i2=i0|(i1<<8);
				i2=rle_fget_word(fd);
				i3=AdNn2_Fp16to8(i2);
				layer->wght[j]=i3;
			}
		}else
			if(wsz==1)
		{
			l=osz*(isz+1);
			for(j=0; j<l; j++)
			{
				i2=rle_fget_byte(fd);
				layer->wght[j]=i2;
			}
//			fread(layer->wght, 1, osz*(isz+1)*1, fd);
		}
#endif
		AdNn2_UpdateLayerZMask(layer);
		
		if(adnn2_rle_lcnt)
		{
			printf("AdNn2_LoadNet: RLE Run Mismatch\n");
		}
	}

	if(adnn2_rle_lcnt)
	{
		printf("AdNn2_LoadNet: File RLE Run Error\n");
	}

	fclose(fd);
	return(0);
}


int AdNn2_TestNet(AdNn2_Net *net, int v0, int v1, int dflag)
{
	byte iwbuf[32], owbuf[32], ewbuf[32];
	int v2, v3, v2c, v3c;
	int i, j, k, te;

	v2=v0+v1;
	te=0;
	
	v2c=0;
	if(((v0&  1)+(v1&  1))&  2)		v2c|=  1;
	if(((v0&  3)+(v1&  3))&  4)		v2c|=  2;
	if(((v0&  7)+(v1&  7))&  8)		v2c|=  4;
	if(((v0& 15)+(v1& 15))& 16)		v2c|=  8;
	if(((v0& 31)+(v1& 31))& 32)		v2c|= 16;
	if(((v0& 63)+(v1& 63))& 64)		v2c|= 32;
	if(((v0&127)+(v1&127))&128)		v2c|= 64;
	if(((v0&255)+(v1&255))&256)		v2c|=128;
	v2|=v2c<<16;
	
	for(j=0; j<8; j++)
	{
		iwbuf[0+j]=(v0&(1<<j))?0x38:0xB8;
		iwbuf[8+j]=(v1&(1<<j))?0x38:0xB8;
		iwbuf[16+j]=0;

#if 0
		iwbuf[16+j]=(v2c&(1<<j))?0x38:0xB8;
//			if((i&3)==3)
		if(i&1)
			iwbuf[16+j]=0;
#endif

		iwbuf[24+j]=0;

//			iwbuf[0+j]=0;
//			iwbuf[8+j]=0;
	}
	
	for(j=0; j<10; j++)
//	for(j=0; j<12; j++)
	{
		AdNn2_ForwardEvalNet(net, iwbuf, owbuf);
//			if((i&3)==3)
//			if(i&1)
		if(1)
		{
			for(k=16; k<24; k++)
				iwbuf[k]=owbuf[k];
		}

		for(k=24; k<32; k++)
			iwbuf[k]=owbuf[k];
	}

	v3=0;
	for(j=0; j<24; j++)
	{
		if(!(owbuf[j]&0x80))
//			if(!(owbuf[j]&0x80) && ((owbuf[j]&0x7F)>=0x38))
			v3|=1<<j;
	}

	for(j=0; j<32; j++)
	{
		ewbuf[j]=0;
	}

	for(j=0; j<24; j++)
	{
		ewbuf[j]=0;
		if((v2^v3)&(1<<j))
		{
			ewbuf[j]=(v2&(1<<j))?0x3F:0xBF;
			if(j<16)
			{
				ewbuf[j]=(v2&(1<<j))?0x3C:0xBC;
				if(j<8)
				{
					if((v2^v3)&(1<<(j+15)))
						ewbuf[j]=(v2&(1<<j))?0x27:0xA7;
				}
			}else if((j>16) && (j<24))
			{
				if((v2^v3)&(1<<(j-1)))
					ewbuf[j]=(v2&(1<<j))?0x30:0xB0;
			}
			
			if((owbuf[j]&0x7F)<0x28)
				{ ewbuf[j]=(v2&(1<<j))?0x1F:0x9F; }

			if((owbuf[j]&0x7F)<0x10)
				{ ewbuf[j]=(v2&(1<<j))?0x07:0x87; }
			
//				ewbuf[j]=(v2&(1<<j))?
//					(0x38-(owbuf[j]&0x7F)):
//					0xBF;
			te+=8;
		}else
		{
			if((owbuf[j]&0x7F)<0x20)
			{
				ewbuf[j]=(v2&(1<<j))?0x30:0xB0;
				te+=3;
			}
		}
//			if((owbuf[j]&0x7F)>0x5F)
//			{
//				ewbuf[j]=(owbuf[j]&0x80)?0x3F:0xBF;
//			}
	}

	te/=8;

	if((v2!=v3) && !(dflag&1))
//	if(((v2!=v3) || ((dflag&2) && (te>0))) && !(dflag&1))
	{
		if(net->layer[0]->nn_isz>=32)
		{
			for(j=24; j<32; j++)
			{
				k=net->layer[0]->err_i[j];
				if((k&0x7F)>0x3F)
					{ k=(k&0x80)?0xBF:0x3F; }
				ewbuf[j]=k;
			}
		}

		AdNn2_ReverseEvalNet(net, NULL, ewbuf);
		AdNn2_AdjustNet(net);

		if(!(dflag&2))
		{
			te=AdNn2_TestNet(net, v0, v1, 1);
		}else
		{
			net->v2=v2;
			net->v3=v3;
		}
		return(te);

#if 0
		AdNn2_ForwardEvalNet(net, iwbuf, owbuf);

		v3=0;
		for(j=0; j<24; j++)
		{
			if(!(owbuf[j]&0x80))
				v3|=1<<j;
		}

		te=0;
		for(j=0; j<24; j++)
		{
			ewbuf[j]=0;
			if((v2^v3)&(1<<j))
				{ te+=8; }
			else if((owbuf[j]&0x7F)<0x20)
				{ te+=3; }
			te++;
		}
		te/=8;
#endif
	}
	
	net->v2=v2;
	net->v3=v3;
	
	return(te);
}

int main()
{
	int tnsz[8];
	byte iwbuf[32], owbuf[32], ewbuf[32];
	AdNn2_Net *neta[8];
	int nte[8];
	int tnte[8];
	AdNn2_Net *net;
//	AdNn2_Net *net0, *net1, *net2, *net3;
//	int v0, v1, v2, v3, v2c, v3c;
	int v0, v1, v2, v3;
	int i, j, k, te, e0, e1, e2, e3, nd, ate;
	int anc, and, lvl, lvlhi, nolvl, wasand;

#ifdef ADNN2_DOTIMESEED
	e0=clock();
	e1=e0+(0.1*CLOCKS_PER_SEC);
	
	v0=1; v1=1;
	e2=e0;
	while(e2<e1)
	{
		e2=clock();
		v0+=e0;
		v1+=v0;
	}
	adnn2_fastrand_seed+=(v0^v1);
#endif

//	lvlhi=4<<16;
	lvlhi=2<<16;

#if 0
	tnsz[0]=24;
	tnsz[1]=96;
	tnsz[2]=24;
	net=AdNn2_AllocNet(2, tnsz);
#endif

#if 0
	tnsz[0]=24;
	tnsz[1]=64;
	tnsz[2]=64;
	tnsz[3]=24;
	net=AdNn2_AllocNet(3, tnsz);
#endif

#if 0
	tnsz[0]=24;
	tnsz[1]=72;
	tnsz[2]=72;
	tnsz[3]=72;
	tnsz[4]=24;
	net=AdNn2_AllocNet(4, tnsz);
#endif

#if 0
	tnsz[0]=24;
	tnsz[1]=96;
	tnsz[2]=96;
	tnsz[3]=24;

//	tnsz[3]=96;
//	tnsz[4]=24;
	net=AdNn2_AllocNet(3, tnsz);
#endif

#if 0
	tnsz[0]=24;
	tnsz[1]=256;
	tnsz[2]=256;
	tnsz[3]=256;
	tnsz[4]=24;
	net=AdNn2_AllocNet(4, tnsz);
#endif

#if 0
	tnsz[0]=32;
	tnsz[1]=96;
	tnsz[2]=96;
	tnsz[3]=96;
	tnsz[4]=32;
	net=AdNn2_AllocNet(4, tnsz);
#endif

#if 0
	tnsz[0]=32;
	tnsz[1]=72;
	tnsz[2]=72;
	tnsz[3]=72;
	tnsz[4]=72;
	tnsz[5]=72;
	tnsz[6]=32;
	net=AdNn2_AllocNet(6, tnsz);
#endif

#if 0
	tnsz[0]=24;
	tnsz[1]=48;
	tnsz[2]=48;
	tnsz[3]=48;
	tnsz[4]=48;
	tnsz[5]=48;
	tnsz[6]=24;
	net=AdNn2_AllocNet(6, tnsz);

//	net1=AdNn2_AllocNet(6, tnsz);
//	net2=AdNn2_AllocNet(6, tnsz);
//	net3=AdNn2_AllocNet(6, tnsz);
#endif

#if 0
	tnsz[0]=24;
	tnsz[1]=32;
	tnsz[2]=32;
	tnsz[3]=32;
	tnsz[4]=32;
	tnsz[5]=32;
	tnsz[6]=24;
	net=AdNn2_AllocNet(6, tnsz);
#endif


#if 1
	tnsz[0]=24;
	tnsz[1]=64;
	tnsz[2]=64;
	tnsz[3]=64;
	tnsz[4]=64;
	tnsz[5]=64;
	tnsz[6]=24;
	net=AdNn2_AllocNet(6, tnsz);
#endif

//	AdNn2_LoadNet(net);

	neta[0]=net;
//	neta[1]=AdNn2_CloneNet(net);
//	neta[2]=AdNn2_CloneNet(net);
//	neta[3]=AdNn2_CloneNet(net);
//	neta[4]=AdNn2_CloneNet(net);
//	neta[5]=AdNn2_CloneNet(net);
//	neta[6]=AdNn2_CloneNet(net);
//	neta[7]=AdNn2_CloneNet(net);

	neta[1]=AdNn2_AllocNet(6, tnsz);
	neta[2]=AdNn2_AllocNet(6, tnsz);
	neta[3]=AdNn2_AllocNet(6, tnsz);
	neta[4]=AdNn2_AllocNet(6, tnsz);
	neta[5]=AdNn2_AllocNet(6, tnsz);
	neta[6]=AdNn2_AllocNet(6, tnsz);
	neta[7]=AdNn2_AllocNet(6, tnsz);

	AdNn2_LoadNet(neta[0], 0, 1);

	AdNn2_LoadNet(neta[0], 0, 0);
	AdNn2_LoadNet(neta[1], 1, 0);
	AdNn2_LoadNet(neta[2], 2, 0);
	AdNn2_LoadNet(neta[3], 3, 0);

	AdNn2_LoadNet(neta[4], 4, 0);
	AdNn2_LoadNet(neta[5], 5, 0);
	AdNn2_LoadNet(neta[6], 6, 0);
	AdNn2_LoadNet(neta[7], 7, 0);

	v0=(adnn2_fastrand()&255);
	v1=(adnn2_fastrand()&255);
	nd=255;
	
	lvl=lvlhi>>16;
	v0&=(1<<lvl)-1;
	v1&=(1<<lvl)-1;
	
	v2=0;
	v3=0;
	te=0;	ate=0;
	anc=0;	and=0;
	e0=0; e1=0; e2=0; e3=0;
	wasand=0;
	
	for(i=0; i<(1<<28); i++)
	{
		if(!(i&255))
		{
			printf("%4u/%4u  %3u:%3u  L%3X   %04X %04X (%04X)  %02X %02X (%02X) \r",
				te, ate, anc, and, lvlhi>>8,
				v2&0xFFFF, v3&0xFFFF, (v2^v3)&0xFFFF,
				v2>>16, v3>>16, (v2^v3)>>16);

			fflush(stdout);

			if(!ate)
				ate=te;
			ate=(ate*255+te+127)>>8;

			te=0;

			e0=0; e1=0; e2=0; e3=0;
			anc=0;	and=0;

//			if((i>0) && !(i&65535))
			if((i>=16384) && !(i&4095))
			{
//				AdNn2_DumpNet(net);
//				AdNn2_DumpNetAsc(net);

				AdNn2_DumpNetAsc(neta[0]);
				AdNn2_DumpNet(neta[0], 0, 0);
				AdNn2_DumpNet(neta[1], 1, 0);
				AdNn2_DumpNet(neta[2], 2, 0);
				AdNn2_DumpNet(neta[3], 3, 0);

				AdNn2_DumpNet(neta[4], 4, 0);
				AdNn2_DumpNet(neta[5], 5, 0);
				AdNn2_DumpNet(neta[6], 6, 0);
				AdNn2_DumpNet(neta[7], 7, 0);

				AdNn2_DumpNet(neta[0], 0, 1);
				AdNn2_DumpNet(neta[1], 1, 1);
				AdNn2_DumpNet(neta[2], 2, 1);
				AdNn2_DumpNet(neta[3], 3, 1);

				printf("\n");
				
				if((ate<=0) && (lvl>=8))
					break;
			}
		}

//		if(!(i&3))
		if(!(i&1))
//		if(1)
		{
			wasand=0;

			if((v2==v3) || (nd<=0))
			{
				nolvl=0;

//				if(lvl<8)
				if(0)
				{
#if 1
					v0=(adnn2_fastrand()&255);
					v1=(adnn2_fastrand()&255);
					v0&=(1<<lvl)-1;
					v1&=(1<<lvl)-1;
#endif

#if 0
					v0=(adnn2_fastrand_gauss_u8()&255);
					v1=(adnn2_fastrand_gauss_u8()&255);
					v0>>=8-lvl;
					v1>>=8-lvl;
//					v0&=(1<<lvl)-1;
//					v1&=(1<<lvl)-1;
#endif

#if 1
					if((adnn2_fastrand()&63)<(lvl*lvl))
					{
						v0=(adnn2_fastrand()&255);
						v1=(adnn2_fastrand()&255);
						nolvl=1;
					}
#endif
				}else
				{
					v0=(adnn2_fastrand()&255);
					v1=(adnn2_fastrand()&255);
				}
//				nd=16;
//				nd=64;
//				nd=128;
				nd=512;
				anc++;
			}else
			{
				wasand=1;
				nd--;
				and++;
			}
		}

#ifndef ADNN2_DOGA
		nte[0]=AdNn2_TestNet(neta[0], v0, v1, 2);
		for(j=1; j<8; j++)
			AdNn2_CopyNet(neta[j], neta[0]);
#endif

#ifdef ADNN2_DOGA
//		e0=AdNn2_TestNet(net0, v0, v1);
//		e1=AdNn2_TestNet(net1, v0, v1);
//		e2=AdNn2_TestNet(net2, v0, v1);
//		e3=AdNn2_TestNet(net3, v0, v1);

		for(j=0; j<8; j++)
		{
			k=0;
			if(j>1)k|=1;
//			if(j>3)k|=1;

			if(wasand && (k&1) && tnte[j])
			{
				/*	Same nets with same inputs will
					continue to produce same outputs. */
				tnte[j]+=nte[j];
				continue;
			}

			nte[j]=AdNn2_TestNet(neta[j], v0, v1, k);
			tnte[j]+=nte[j];
		}

		if((i>=1024) && !(i&63))
//		if((i>=1024) && !(i&127))
//		if((i>=1024) && !(i&511))
		{
			for(j=0; j<8; j++)
				if(tnte[j]<0)
					tnte[j]=999999;

			for(j=0; j<8; j++)
				for(k=j+1; k<8; k++)
			{
				if(tnte[k]<tnte[j])
				{
					net=neta[j]; neta[j]=neta[k]; neta[k]=net;
					e3=nte[j]; nte[j]=nte[k]; nte[k]=e3;
					e3=tnte[j]; tnte[j]=tnte[k]; tnte[k]=e3;
				}
			}

			AdNn2_BreedNet(neta[4],
				neta[adnn2_fastrand_gauss_u2()],
				neta[adnn2_fastrand_gauss_u2()]);
			AdNn2_BreedNet(neta[5],
				neta[adnn2_fastrand_gauss_u2()],
				neta[adnn2_fastrand_gauss_u2()]);
			AdNn2_BreedNet(neta[6],
				neta[adnn2_fastrand_gauss_u2()],
				neta[adnn2_fastrand_gauss_u2()]);
			AdNn2_BreedNet(neta[7],
				neta[adnn2_fastrand_gauss_u2()],
				neta[adnn2_fastrand_gauss_u2()]);

//			AdNn2_BreedNet(neta[2],
//				neta[adnn2_fastrand()&1], neta[adnn2_fastrand()&1]);
//			AdNn2_BreedNet(neta[3],
//				neta[adnn2_fastrand()&1], neta[adnn2_fastrand()&1]);

			for(j=0; j<8; j++)
				tnte[j]=0;
		}
#endif

		net=neta[0];
		v2=net->v2;
		v3=net->v3;

		if(!nolvl)
		{
			if((v2==v3) && (lvlhi<0x08FF00))
			{
//				lvlhi+=63;
				lvlhi+=31;
			}
			if((v2!=v3) && (lvlhi>0x010100))
			{
				lvlhi-=23;
//				lvlhi-=43;
//				lvlhi-=47;
//				lvlhi-=95;
	//			lvlhi-=191;
	//			lvlhi=0x010100;
			}
			lvl=lvlhi>>16;
		}
	
//		te+=e0;
		te+=nte[0];
	}
	
//	AdNn2_DumpNet(net);
}
