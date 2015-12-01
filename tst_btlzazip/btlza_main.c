#include <btlzazip.h>
#include <time.h>

#if 1
void BGBBTJ_BufPNG_DataAdler32_Step16(byte *buf, int *rs1, int *rs2)
{
	byte *cs;
	int s1, s2;

#if 0
	s1=*rs1; s2=*rs2; cs=buf;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	*rs1=s1; *rs2=s2;
#endif

#if 1
	s1=*rs1; s2=*rs2;
	s1+=buf[ 0]; s2+=s1;	s1+=buf[ 1]; s2+=s1;
	s1+=buf[ 2]; s2+=s1;	s1+=buf[ 3]; s2+=s1;
	s1+=buf[ 4]; s2+=s1;	s1+=buf[ 5]; s2+=s1;
	s1+=buf[ 6]; s2+=s1;	s1+=buf[ 7]; s2+=s1;
	s1+=buf[ 8]; s2+=s1;	s1+=buf[ 9]; s2+=s1;
	s1+=buf[10]; s2+=s1;	s1+=buf[11]; s2+=s1;
	s1+=buf[12]; s2+=s1;	s1+=buf[13]; s2+=s1;
	s1+=buf[14]; s2+=s1;	s1+=buf[15]; s2+=s1;
	*rs1=s1; *rs2=s2;
#endif
}

u32 BGBBTJ_BufPNG_DataAdler32(void *buf, int sz, u32 lcrc)
{
	byte *s;
	u32 s1, s2;
	int n, l;

	s=(byte *)buf;
	l=sz;
	s1=lcrc&0xFFFF;
	s2=(lcrc>>16)&0xFFFF;

	while(l>=5552)
	{
		n=5552/16;
		while(n--)
		{
			BGBBTJ_BufPNG_DataAdler32_Step16(s, &s1, &s2);
			s+=16;
		}
		s1=s1%65521;
		s2=s2%65521;
		l-=5552;
	}

	if(l>0)
	{
		while(l>=16)
		{
			BGBBTJ_BufPNG_DataAdler32_Step16(s, &s1, &s2);
			s+=16; l-=16;
		}
		while(l--)
			{ s1+=*s++; s2+=s1; }
		s1=s1%65521;
		s2=s2%65521;
	}

	return(s1|(s2<<16));
}
#endif

u32 BGBBTJ_BufPNG_CRC32(void *buf, int sz, u32 lcrc)
{
	static u32 crctab[256];
	static int init=0;

	byte *s;
	u32 c;
	int i, j;

	if(!init)
	{
		init=1;

		for(i=0; i<256; i++)
		{
			c=i;
			for(j=0; j<8; j++)
				if(c&1)c=0xedb88320^(c>>1);
					else c>>=1;
			crctab[i]=c;
		}
	}

	c=lcrc; s=buf; i=sz;
	while(i--)c=crctab[(c^(*s++))&0xFF]^(c>>8);
	return(c);
}

char *BGBBTJ_BufPNG_Fourcc2String(u32 fcc)
{
	static char buf[64], pos;
	int i, j;

	if(pos>56)pos=0;

	for(i=0; i<4; i++)
	{
		j=(fcc>>(i*8))&0xFF;
		if((j<' ') || (j>'~'))j='~';
		buf[pos+i]=j;
	}
	buf[pos+4]=0;
	return(buf+pos);
}

void *btlza_malloc(int sz)
{
	void *ptr;
	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

void btlza_free(void *ptr)
{
	free(ptr);
}

int BTLZA_DecodeFileStream(FILE *ifd, FILE *ofd, int mode, int flag)
{
	byte tb[4096];
	byte *tbuf1, *tbuf2, *tbuf3, *tbuf4;
	byte *tbufe;
	int szbuf1, szbuf2;
	int blsz, wisz;
	int t0, t1, t2, t3;
	int i0, i1, i2, i3;
	s64 aisz, aosz;
	int i, j, k;

	tbuf1=NULL;	szbuf1=1<<16;
	tbuf2=NULL;	szbuf2=1<<24;
	tbuf3=NULL; tbufe=NULL;

	aisz=0; aosz=0; t0=clock();
	while(!feof(ifd))
	{
		if((flag&2) || (mode==2) || (mode==3))
		{
			t1=clock();
			t2=t1-t0;

			fprintf(stderr, "%.2fKiB/%.2fKiB %.2f%% %.3fKiB/s\r",
				aosz/1024.0, aisz/1024.0,
				(100.0*aosz)/(aisz+1),
				(aosz/1024.0)/((t2+1.0)/CLOCKS_PER_SEC));

//			if(mode==2)
//				fprintf(stderr, "\n");
		}
	
		i0=fgetc(ifd);
		if(i0<0)
		{
			if(mode==3)
			{
				fseek(ifd, 0, 0);
				continue;
			}
			break;
		}
		
		if((i0==0xE1) || (i0==0xEC))
		{
			i1=fgetc(ifd);
			i1=(i1<<8)|fgetc(ifd);
			i1=(i1<<8)|fgetc(ifd);
			
			if(i0==0xEC)
			{
				i1=(i1<<8)|fgetc(ifd);
				i1=(i1<<8)|fgetc(ifd);
			}
			
			if(i1<0)
			{
				break;
			}
			
			if(!tbuf1 || (i1>szbuf1))
			{
				while(i1>szbuf1)
					szbuf1=szbuf1+(szbuf1>>1);
				tbuf1=realloc(tbuf1, szbuf1);
			}
			
			i2=i1-4;
			if(i0==0xEC)
				i2=i1-6;
			
			fread(tbuf1, 1, i2, ifd);
			
			if(!tbuf2)
				{ tbuf2=malloc(szbuf2); tbufe=NULL; }
			if(tbufe)
				{ memcpy(tbuf2, tbufe-wisz, wisz); }
			tbuf4=tbuf2+wisz;
			
//			i=BTLZA_DecodeStreamZl(tbuf1, tbuf2, szbuf1, szbuf2);
//			j=BTLZA_DecodeStreamSzZl(tbuf1, tbuf2, i2, szbuf2, &i, 0);
			j=BTLZA_DecodeStreamSzZl(tbuf1, tbuf4, i2, szbuf2, &i, 0);
			if(j<0)
			{
				fprintf(stderr, "BTLZA_DecodeFileStream: "
					"Stream Error %d\n", j);
				break;
			}
			
//			tbufe=tbuf2+i;
			tbufe=tbuf4+i;
			
			if(mode==1)
			{
				fwrite(tbuf4, 1, i, ofd);
				aisz+=i1;
				aosz+=i;
				continue;
			}

			if(mode==3)
			{
				aisz+=i1;
				aosz+=i;
				continue;
			}

			if(mode==2)
			{
				if(!tbuf3)
					{ tbuf3=malloc(szbuf2); }
				fread(tbuf3, 1, i, ofd);

				aisz+=i1;
				aosz+=i;
				
				if(!memcmp(tbuf4, tbuf3, i))
				{
//					fprintf(stderr, "Match %d bytes\n", i);
					continue;
				}

				fprintf(stderr, "\n");
				
				for(j=0; j<i; j++)
					if(tbuf4[j]!=tbuf3[j])
						break;
				fprintf(stderr, "%d/%d: %02X!=%02X\n",
					j, i, tbuf4[j], tbuf3[j]);
				continue;
			}

			continue;
		}
		
		if(i0==0xE3)
		{
			i1=fgetc(ifd);
			i1=(i1<<8)|fgetc(ifd);
			i1=(i1<<8)|fgetc(ifd);
//			fread(tb+4, 1, i1-4, ifd);

			if(i1<0)break;

			if(!tbuf1 || (i1>szbuf1))
			{
				while(i1>szbuf1)
					szbuf1=szbuf1+(szbuf1>>1);
				tbuf1=realloc(tbuf1, szbuf1);
			}
			
			fread(tbuf1+4, 1, i1-4, ifd);
			
			if((tbuf1[4]=='S') && (tbuf1[5]=='T'))
			{
				if(mode==1)
				{
					fwrite(tbuf1+6, 1, i1-6, ofd);
					aisz+=i1;
					aosz+=i1-6;
					continue;
				}
			}

			if((tbuf1[4]=='B') && (tbuf1[5]=='I'))
			{
				if(!tbuf2)
					{ tbuf2=malloc(szbuf2); }			
				if(tbufe)
					{ memcpy(tbuf2, tbufe-wisz, wisz); }
				tbuf4=tbuf2+wisz;
			
				i2=i1-6;
			
//				i=BTLZA_DecodeStreamZl(tbuf1, tbuf2, szbuf1, szbuf2);
				j=BTLZA_DecodeStreamSzZl(
					tbuf1+6, tbuf4, i2, szbuf2, &i, 0);
				if(j<0)
				{
					fprintf(stderr, "BTLZA_DecodeFileStream: "
						"Stream Error %d\n", j);
					break;
				}
			
				tbufe=tbuf4+i;

				if(mode==1)
				{
					fwrite(tbuf4, 1, i, ofd);
					aisz+=i1;
					aosz+=i;
					continue;
				}

				if(mode==3)
				{
					aisz+=i1;
					aosz+=i;
					continue;
				}

				if(mode==2)
				{
					if(!tbuf3)
						{ tbuf3=malloc(szbuf2); }
					fread(tbuf3, 1, i, ofd);

					aisz+=i1;
					aosz+=i;

					if(!memcmp(tbuf4, tbuf3, i))
					{
//						fprintf(stderr, "Match %d bytes\n", i);
						continue;
					}

					fprintf(stderr, "\n");
				
					for(j=0; j<i; j++)
						if(tbuf4[j]!=tbuf3[j])
							break;
					fprintf(stderr, "%d/%d: %02X!=%02X\n",
						j, i, tbuf4[j], tbuf3[j]);
					continue;
				}

				continue;
			}
			
			continue;
		}

		if(i0==0xE4)
		{
			i1=fgetc(ifd);
			i1=(i1<<8)|fgetc(ifd);
			i1=(i1<<8)|fgetc(ifd);
			
			fread(tb+4, 1, i1-4, ifd);
			
			if(	(tb[4]=='B') && (tb[5]=='T') &&
				(tb[6]=='L') && (tb[7]=='Z'))
			{
//				szbuf2=1<<tb[10];
				blsz=1<<tb[10];
				wisz=1<<tb[11];
				szbuf2=blsz+wisz;
				tbuf2=realloc(tbuf2, szbuf2);
			}
			continue;
		}

		if(i0==0xE5)
		{
			i1=fgetc(ifd);
			fread(tb+2, 1, i1-2, ifd);
			continue;
		}

		if(i0==0xED)
		{
			i1=fgetc(ifd);
			i1=(i1<<8)|fgetc(ifd);
			i1=(i1<<8)|fgetc(ifd);
			i1=(i1<<8)|fgetc(ifd);
			i1=(i1<<8)|fgetc(ifd);

			if(!tbuf1 || (i1>szbuf1))
			{
				while(i1>szbuf1)
					szbuf1=szbuf1+(szbuf1>>1);
				tbuf1=realloc(tbuf1, szbuf1);
			}
			
			fread(tbuf1+6, 1, i1-6, ifd);
			
			if((tbuf1[6]=='S') && (tbuf1[7]=='T'))
			{
				if(mode==1)
				{
					fwrite(tbuf1+8, 1, i1-8, ofd);
					aisz+=i1;
					aosz+=i1-8;
					continue;
				}
			}
			
			continue;
		}
		
		fprintf(stderr, "Invalid Marker Magic %02X\n", i0);
		
		break;
	}
	return(0);
}

int BTLZA_EncodeFileStream(FILE *ifd, FILE *ofd, char *ifn,
	int lvl, int flag)
{
	static int lwsz[10]={
		12, 14, 15, 15, 17,
		18, 19, 19, 20, 20};

	byte tb[1024];
	byte *tbuf1, *tbuf2, *tbuf3;
	byte *ct;
	int szbuf1, szbuf2;
	int t0, t1, t2, t3;
	s64 aisz, aosz;
	int bsz, wsz, bsz2, wsz2;
	int lbok;
	int i, j, k, l;

	wsz=lwsz[lvl&15];
	bsz=wsz+2;

	memset(tb, 0, 1024);
	tb[0]=0xE4;		tb[1]=0;
	tb[2]=0;		tb[3]=0;
	tb[4]='B';		tb[5]='T';
	tb[6]='L';		tb[7]='Z';
	tb[ 8]=0;		tb[ 9]=0;
	tb[10]=bsz;		tb[11]=wsz;
	
	ct=tb+12;
	
	if(ifn)
	{
		tb[8]|=1;
		strcpy(ct, ifn);
		i=strlen(ifn)+1;
		i=((i+3)>>2)<<2;
		ct+=i;
	}
	
	k=ct-tb;
	tb[1]=k>>16;
	tb[2]=k>> 8;
	tb[3]=k    ;

	fwrite(tb, 1, k, ofd);

	bsz2=1<<bsz;
	wsz2=1<<wsz;
	szbuf1=bsz2+wsz2;
	szbuf2=1<<(bsz+1);
	tbuf1=malloc(szbuf1);
	tbuf2=malloc(szbuf2);
	tbuf3=tbuf1+wsz2;
	aisz=0; aosz=0;
	lbok=0;

	t0=clock();
	while(!feof(ifd))
	{
		if(flag&2)
		{
			t1=clock();
			t2=t1-t0;

			fprintf(stderr, "%.2fKiB/%.2fKiB Sz=%.2f%%(C=%.2f%%) %.3fKiB/s\r",
				aosz/1024.0, aisz/1024.0,
				(100.0*aosz)/(aisz+1), 100-((100.0*aosz)/(aisz+1)),
				(aisz/1024.0)/((t2+1.0)/CLOCKS_PER_SEC));
		}
	
		i=1<<bsz;
		j=fread(tbuf3, 1, i, ifd);
		if(j<=0)break;

		l=lvl;
		if(lbok)l|=(BGBBTJ_ZFL_PRELOAD<<8);
//		k=BTLZA_BitEnc_EncodeStreamXLvlZl(tbuf3, tbuf2, j, szbuf2, lvl);
		k=BTLZA_BitEnc_EncodeStreamXLvlZlTest(tbuf3, tbuf2, j, szbuf2, l);

		if((k<0) || (k>=(j+2)))
		{
			if((j+6)<(1<<24))
			{
				l=j+6;
				fputc(0xE3, ofd);	fputc(l>>16, ofd);
				fputc(l>> 8, ofd);	fputc(l    , ofd);
				fputc('S'  , ofd);	fputc('T'  , ofd);
				fwrite(tbuf3, 1, j, ofd);
			}else
			{
				l=j+8;
				fputc(0xED, ofd);	fputc(0, ofd);
				fputc(l>>24, ofd);	fputc(l>>16, ofd);
				fputc(l>> 8, ofd);	fputc(l    , ofd);
				fputc('S'  , ofd);	fputc('T'  , ofd);
				fwrite(tbuf3, 1, j, ofd);
			}

			aisz+=j;		aosz+=l;
			continue;
		}

		if((k+4)<(1<<24))
		{
			l=k+4;
			fputc(0xE1, ofd);	fputc(l>>16, ofd);
			fputc(l>> 8, ofd);	fputc(l    , ofd);
			fwrite(tbuf2, 1, k, ofd);
		}else
		{
			l=k+6;
			fputc(0xEC , ofd);	fputc(0    , ofd);
			fputc(l>>24, ofd);	fputc(l>>16, ofd);
			fputc(l>> 8, ofd);	fputc(l    , ofd);
			fwrite(tbuf2, 1, k, ofd);
		}
		
		/* preload dictionary for subsequent block */
		memcpy(tbuf1, tbuf3+j-wsz2, wsz2);
		lbok=1;

		aisz+=j;
		aosz+=l;
	}
	
	if(flag&2)
	{
		fprintf(stderr, "\n");
	}

	return(0);
}

void help(char *pgm)
{
	fprintf(stderr, "usage: %s options* <infile> [-o <outfile>]\n", pgm);
	fprintf(stderr,
	"  -d           Decode (default is Encode)\n"
	"  -dt          Decode Test (Compare with output file)\n"
	"  -ds          Decode Sim (Decode but don't write output)\n"
	"  -c           Write to stdout\n"
	"  -v           Verbose\n"
	"  -o <name>    Specify output file\n"
	"  -1 .. -9     Compression Level\n"
	"  -1a .. -9a   Compression Level (With Arithmetic, *)\n"
	"  -fe          Use fast encoder\n"
	"  -rh          Use RingHuff encoding\n"
	"  -h           This message\n"
	"  --help       This message\n"
	"\n"
	"*: The arithmetic coder only rarely gives good results\n"
	);
}

int main(int argc, char *argv[])
{
	char tb[1024];
	char *ifn;
	char *ofn;
	FILE *ifd, *ofd;
	int lvl, mode, flag, lvflag;
	int i, j, k;
	
	ifn=NULL; ofn=NULL; mode=0; flag=0; lvl=5; lvflag=0;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-d"))
				{ mode=1; continue; }

			if(!strcmp(argv[i], "-dt"))
				{ mode=2; continue; }
			if(!strcmp(argv[i], "-ds"))
				{ mode=3; continue; }

			if(!strcmp(argv[i], "-c"))
				{ flag|=1; continue; }
			if(!strcmp(argv[i], "-v"))
				{ flag|=2; continue; }

			if(!strcmp(argv[i], "-o"))
				{ ofn=argv[i+1]; i++; continue; }

			if(!strcmp(argv[i], "-1"))
				{ lvl=1; continue; }
			if(!strcmp(argv[i], "-2"))
				{ lvl=2; continue; }
			if(!strcmp(argv[i], "-3"))
				{ lvl=3; continue; }
			if(!strcmp(argv[i], "-4"))
				{ lvl=4; continue; }
			if(!strcmp(argv[i], "-5"))
				{ lvl=5; continue; }
			if(!strcmp(argv[i], "-6"))
				{ lvl=6; continue; }
			if(!strcmp(argv[i], "-7"))
				{ lvl=7; continue; }
			if(!strcmp(argv[i], "-8"))
				{ lvl=8; continue; }
			if(!strcmp(argv[i], "-9"))
				{ lvl=9; continue; }

			if(!strcmp(argv[i], "-1a"))
				{ lvl=1; lvflag|=BGBBTJ_ZFL_ARITH; continue; }
			if(!strcmp(argv[i], "-2a"))
				{ lvl=2; lvflag|=BGBBTJ_ZFL_ARITH; continue; }
			if(!strcmp(argv[i], "-3a"))
				{ lvl=3; lvflag|=BGBBTJ_ZFL_ARITH; continue; }
			if(!strcmp(argv[i], "-4a"))
				{ lvl=4; lvflag|=BGBBTJ_ZFL_ARITH; continue; }
			if(!strcmp(argv[i], "-5a"))
				{ lvl=5; lvflag|=BGBBTJ_ZFL_ARITH; continue; }
			if(!strcmp(argv[i], "-6a"))
				{ lvl=6; lvflag|=BGBBTJ_ZFL_ARITH; continue; }
			if(!strcmp(argv[i], "-7a"))
				{ lvl=7; lvflag|=BGBBTJ_ZFL_ARITH; continue; }
			if(!strcmp(argv[i], "-8a"))
				{ lvl=8; lvflag|=BGBBTJ_ZFL_ARITH; continue; }
			if(!strcmp(argv[i], "-9a"))
				{ lvl=9; lvflag|=BGBBTJ_ZFL_ARITH; continue; }

			if(!strcmp(argv[i], "-fe"))
				{ lvflag|=BGBBTJ_ZFL_FASTENC; continue; }
			if(!strcmp(argv[i], "-rh"))
				{ lvflag|=BGBBTJ_ZFL_RINGHUFF; continue; }

			if(!strcmp(argv[i], "-h"))
				{ mode=-1; continue; }
			if(!strcmp(argv[i], "--help"))
				{ mode=-1; continue; }

			fprintf(stderr, "unrecognized option %s\n", argv[i]);
			continue;
		}
		if(!ifn)
		{
			ifn=argv[i];
			continue;
		}
		fprintf(stderr, "too many names %s\n", argv[i]);
		continue;
	}
	
	if(mode==-1)
	{
		help(argv[0]);
		return(0);
	}
	
	lvl=lvl|(lvflag<<8);
	
	if(ifn)
	{
		ifd=fopen(ifn, "rb");
		if(!ifd)
		{
			fprintf(stderr, "Could not open input %s\n", ifn);
			return(-1);
		}
	}else
	{
		ifd=stdin;
	}
	
	if((mode!=0) && ifn && !ofn && !(flag&1))
	{
		fread(tb, 1, 1024, ifd);
		fseek(ifd, 0, 0);
		
		if((tb[4]=='B') && (tb[5]=='T') &&
			(tb[6]=='L') && (tb[7]=='Z'))
		{
			if(tb[8]&1)
			{
				ofn=strdup(tb+12);
			}
		}
	}

	if(ofn)
	{
		if((mode==2) || (mode==3))
		{
			ofd=fopen(ofn, "rb");
		}else if((mode==0) || (mode==1))
		{
			ofd=fopen(ofn, "wb");
		}
		if(!ofd)
		{
			fprintf(stderr, "Could not open output %s\n", ofn);
			return(-1);
		}
	}else if((flag&1) || (!ifn))
	{
		ofd=stdout;
	}else if(mode==0)
	{
		sprintf(tb, "%s.blz", ifn);
		ofd=fopen(tb, "wb");
		if(!ofd)
		{
			fprintf(stderr, "Could not open output %s\n", tb);
			return(-1);
		}
	}else
	{
//		ofd=NULL;

		fprintf(stderr, "No Output %s\n", tb);
		return(-1);
	}
	
	if(mode==0)
	{
		BTLZA_EncodeFileStream(ifd, ofd, ifn, lvl, flag);
		return(0);
	}

	if((mode==1) || (mode==2) || (mode==3))
	{
		BTLZA_DecodeFileStream(ifd, ofd, mode, flag);
		return(0);
	}
	
	return(-1);
}
