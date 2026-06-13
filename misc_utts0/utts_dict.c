typedef struct TkuTts_WordDict_s TkuTts_WordDict;

struct TkuTts_WordDict_s {
char *strtab_buf;
int strtab_buf_sz;
int strtab_buf_ofs;

int *strtab_str_ofs;
int *strtab_str_chn;
int strtab_str_num;
int strtab_str_max;

int *word_key;
int *word_val;
int *word_chn_cs;
int *word_chn_ci;
int word_num;
int word_max;

int strtab_hash[256];
int word_hash_cs[256];
int word_hash_ci[256];
};

byte *TkuTts_LoadFile(char *name, int *rsz);

int tkutts_tolower(int c)
{
	if((c>='A') && (c<='Z'))
		return(c+('a'-'A'));
	return(c);
}

int tkutts_stricmp(const char *s0, const char *s1)
{
	const char *cs0, *cs1;
	int i0, i1;
	
	cs0=s0; cs1=s1;
	i0=*cs0;	i1=*cs1;
	while(i0 && i1)
	{
		if(i0==i1)
			{ cs0++; cs1++; i0=*cs0; i1=*cs1; continue; }
		if(tkutts_tolower(i0)!=tkutts_tolower(i1))
			break;
		cs0++;		cs1++;
		i0=*cs0;	i1=*cs1;
	}
	if(!i0 && !i1)
		return(0);
	return(tkutts_tolower(i0)-tkutts_tolower(i1));
}

int tkutts_strnicmp(const char *s0, const char *s1, int sz)
{
	const char *cs0, *cs1;
	int i0, i1, n;
	
	cs0=s0; cs1=s1;
	i0=*cs0;	i1=*cs1;	n=sz;
	while(i0 && i1 && (n>0))
	{
		if(i0==i1)
			{ cs0++; cs1++; i0=*cs0; i1=*cs1; continue; }
		if(tkutts_tolower(i0)!=tkutts_tolower(i1))
			break;
		cs0++;		cs1++;		n--;
		i0=*cs0;	i1=*cs1;
	}
	if(!i0 && !i1)
		return(0);
	if(n<1)
		return(0);
	return(tkutts_tolower(i0)-tkutts_tolower(i1));
}

int TkuTts_HashString(const char *str)
{
	const char *cs;
	int h;
	cs=str;	h=1;
	while(*cs)	h=h*251+(*cs++);
	h=(h*251+17);
	h=(h*251+17);
	return((h>>8)&255);
}

int TkuTts_HashStringCi(const char *str)
{
	const char *cs;
	int h;
	cs=str;	h=1;
	while(*cs)	h=h*251+tkutts_tolower(*cs++);
	h=(h*251+17);
	h=(h*251+17);
	return((h>>8)&255);
}

int TkuTts_DictInternStringIdx(TkuTts_WordDict *dict, const char *str)
{
	char *s0;
	int i, j, k, h, l;
	
	if(!str)
		return(0);
	
	h=TkuTts_HashString(str);
	i=dict->strtab_hash[h];
	while(i>0)
	{
		s0=dict->strtab_buf+dict->strtab_str_ofs[i];
		if(!strcmp(s0, str))
			return(i);
		i=dict->strtab_str_chn[i];
	}
	
	if(!dict->strtab_buf)
	{
		dict->strtab_buf=malloc(65536);
		dict->strtab_buf_sz=65536;
		dict->strtab_buf_ofs=1;
		dict->strtab_str_ofs=malloc(8192*sizeof(int));
		dict->strtab_str_chn=malloc(8192*sizeof(int));
		dict->strtab_str_max=8192;
		dict->strtab_str_num=1;
		dict->strtab_buf[0]=0;
		dict->strtab_str_ofs[0]=0;
		dict->strtab_str_chn[0]=0;
	}
	
	if((dict->strtab_str_num+1)>=dict->strtab_str_max)
	{
		k=dict->strtab_str_max;
		k=k+(k>>1);
		dict->strtab_str_ofs=realloc(dict->strtab_str_ofs, k*sizeof(int));
		dict->strtab_str_chn=realloc(dict->strtab_str_chn, k*sizeof(int));
		dict->strtab_str_max=k;
	}
	
	l=strlen(str);
	
	if((dict->strtab_buf_ofs+l+3)>=dict->strtab_buf_sz)
	{
		k=dict->strtab_buf_sz;
		k=k+(k>>1);
		dict->strtab_buf=realloc(dict->strtab_buf, k);
		dict->strtab_buf_sz=k;
	}
	
	i=dict->strtab_str_num++;
	j=dict->strtab_buf_ofs;
	dict->strtab_str_ofs[i]=j;
	s0=dict->strtab_buf+j;
	memcpy(s0, str, l+1);
	
	dict->strtab_buf_ofs=j+l+1;
	
	dict->strtab_str_chn[i]=dict->strtab_hash[h];
	dict->strtab_hash[h]=i;
	return(i);
}

char *TkuTts_Dict_PtrForStrIdx(TkuTts_WordDict *dict, int idx)
{
	return(dict->strtab_buf+dict->strtab_str_ofs[idx]);
}

char *TkuTts_StrGetLine(char *cs, char *buf)
{
	char *ct;
	
	ct=buf;
	while(*cs)
	{
		if(*cs=='\r')	break;
		if(*cs=='\n')	break;
		*ct++=*cs++;
	}
	*ct++=0;

	if(*cs=='\r')	cs++;
	if(*cs=='\n')	cs++;
	return(cs);
}

TkuTts_WordDict *TkuTts_LoadDict(char *name)
{
	char tbuf[256], t1buf[128], t2buf[128];
	TkuTts_WordDict *tmp;
	char *ibuf, *cs, *cse;
	int isz, n, ix0, ix1, hcs, hci;
	int i, j, k;
	
	ibuf=TkuTts_LoadFile(name, &isz);
	if(!ibuf)
		return(NULL);
	
	tmp=malloc(sizeof(TkuTts_WordDict));
	memset(tmp, 0, sizeof(TkuTts_WordDict));
	
	tmp->word_key=malloc(4096*sizeof(int));
	tmp->word_val=malloc(4096*sizeof(int));
	tmp->word_chn_cs=malloc(4096*sizeof(int));
	tmp->word_chn_ci=malloc(4096*sizeof(int));
	tmp->word_num=1;
	tmp->word_max=4096;

	cs=ibuf; cse=ibuf+isz;
	while(cs<cse)
	{
		cs=TkuTts_StrGetLine(cs, tbuf);
		
		if((tbuf[0]=='#') || (tbuf[0]=='/') || (tbuf[0]==';'))
			continue;
		
		n=sscanf(tbuf, "%s %s", t1buf, t2buf);
		if(n<2)
			continue;
		
		if((tmp->word_num+1)>=tmp->word_max)
		{
			k=tmp->word_max;
			k=k+(k>>1);
			tmp->word_key=realloc(tmp->word_key, k*sizeof(int));
			tmp->word_val=realloc(tmp->word_val, k*sizeof(int));
			tmp->word_chn_cs=realloc(tmp->word_chn_cs, k*sizeof(int));
			tmp->word_chn_ci=realloc(tmp->word_chn_ci, k*sizeof(int));
			tmp->word_max=k;
		}

		hcs=TkuTts_HashString(t1buf);
		hci=TkuTts_HashStringCi(t1buf);

		ix0=TkuTts_DictInternStringIdx(tmp, t1buf);
		ix1=TkuTts_DictInternStringIdx(tmp, t2buf);
		i=tmp->word_num++;
		tmp->word_key[i]=ix0;
		tmp->word_val[i]=ix1;
		tmp->word_chn_cs[i]=tmp->word_hash_cs[hcs];
		tmp->word_chn_ci[i]=tmp->word_hash_ci[hci];
		tmp->word_hash_cs[hcs]=i;
		tmp->word_hash_ci[hci]=i;
	}
	
	return(tmp);
}

int TkuTts_LookupWordIndexCs(TkuTts_WordDict *dict, const char *name)
{
	char *s0;
	int i, h;
	
	h=TkuTts_HashString(name);
	i=dict->word_hash_cs[h];
	while(i>0)
	{
		s0=TkuTts_Dict_PtrForStrIdx(dict, dict->word_key[i]);
		if(!strcmp(s0, name))
			return(i);
		i=dict->word_chn_cs[i];
	}
	return(0);
}

int TkuTts_LookupWordIndexCi(TkuTts_WordDict *dict, const char *name)
{
	char *s0;
	int i, h;
	
	h=TkuTts_HashStringCi(name);
	i=dict->word_hash_ci[h];
	while(i>0)
	{
		s0=TkuTts_Dict_PtrForStrIdx(dict, dict->word_key[i]);
		if(!tkutts_stricmp(s0, name))
			return(i);
		i=dict->word_chn_ci[i];
	}
	return(0);
}

int TkuTts_LookupWordIndexPfxCi(TkuTts_WordDict *dict, const char *name)
{
	char *s0;
	int i, h, l, bi, bl;
	
	bi=0;	bl=0;
	for(i=1; i<(dict->word_num); i++)
	{
		s0=TkuTts_Dict_PtrForStrIdx(dict, dict->word_key[i]);
		l=strlen(s0);
		if(!tkutts_strnicmp(s0, name, l))
		{
			if(l>bl)
				{ bi=i; bl=l; }
		}
	}
	return(bi);
}

int TkuTts_TranscribeWord(
	TkuTts_WordDict *dict, const char *name, char **robuf)
{
	const char *s0, *s1, *s2;
	char *ct;
	int ix, k, l;
	
	ct=*robuf;

	ix=TkuTts_LookupWordIndexCs(dict, name);
	if(ix>0)
	{
		s0=TkuTts_Dict_PtrForStrIdx(dict, dict->word_val[ix]);
		strcpy(ct, s0);
		*robuf=ct+strlen(ct);
		return(1);
	}
	
	ix=TkuTts_LookupWordIndexCi(dict, name);
	if(ix>0)
	{
		s0=TkuTts_Dict_PtrForStrIdx(dict, dict->word_val[ix]);
		strcpy(ct, s0);
		*robuf=ct+strlen(ct);
		return(1);
	}
	
	ix=TkuTts_LookupWordIndexPfxCi(dict, name);
	if(ix>0)
	{
		s0=TkuTts_Dict_PtrForStrIdx(dict, dict->word_key[ix]);
		s1=TkuTts_Dict_PtrForStrIdx(dict, dict->word_val[ix]);

		l=strlen(s0);
		s2=name+l;
		if(*s2=='-')
			s2++;
		
		strcpy(ct, s1);
		ct=ct+strlen(ct);
		k=TkuTts_TranscribeWord(dict, s2, &ct);
		if(k>0)
		{
			*robuf=ct;
			return(k);
		}
	}
	
	return(0);
}

static const char *tkutts_smallnames[]={
	"zero", "one", "two", "three", "four", "five", "six", "seven",
	"eight", "nine", "ten", "eleven", "twelve", "thirteen",
	"fourteen", "fifteen", "sixteen", "seventeen", "eighteen",
	"nineteen", "twenty"};
static const char *tkutts_tensnames[]={
	"zero",	"ten",	"twenty", "thirty", "forty",
	"fifty", "sixty", "seventy", "eighty", "ninety" };
	

const char *TkuTts_NameForSmallNumber(int val)
{
	if((val>=0) && (val<=20))
		return(tkutts_smallnames[val]);
	return(NULL);
}

int TkuTts_TranscribeNumber(
	TkuTts_WordDict *dict, s64 val, char **robuf)
{
	static int rec;
	const char *s0, *s1;

	if(val<0)
		return(0);

	if(val<21)
	{
		s0=tkutts_smallnames[val];
		return(TkuTts_TranscribeWord(dict, s0, robuf));
	}

	if(val<100)
	{
		s0=tkutts_smallnames[val%10];
		s1=tkutts_tensnames[val/10];
		TkuTts_TranscribeWord(dict, s1, robuf);
		if(val%10)
			TkuTts_TranscribeWord(dict, s0, robuf);
		return(1);
	}

	if(val<1000)
	{
		s0=tkutts_smallnames[val/100];
		TkuTts_TranscribeWord(dict, s0, robuf);
		TkuTts_TranscribeWord(dict, "hundred", robuf);
		if(val%100)
		{
			rec++;
			TkuTts_TranscribeNumber(dict, val%100, robuf);
			rec--;
		}
		return(1);
	}

	if((val<10000) && !rec)
	{
		if(val%100)
		{
			rec++;
			TkuTts_TranscribeNumber(dict, val/100, robuf);
			TkuTts_TranscribeNumber(dict, val%100, robuf);
			rec--;
			return(1);
		}

		s0=tkutts_smallnames[val/1000];
		TkuTts_TranscribeWord(dict, s0, robuf);
		TkuTts_TranscribeWord(dict, "thousand", robuf);
		if(val%1000)
		{
			rec++;
			TkuTts_TranscribeNumber(dict, val%1000, robuf);
			rec--;
		}
		return(1);
	}

	if(val<1000000)
	{
		rec++;
		TkuTts_TranscribeNumber(dict, val/1000, robuf);
		TkuTts_TranscribeWord(dict, "thousand", robuf);
		if(val%1000)
			TkuTts_TranscribeNumber(dict, val%1000, robuf);
		rec--;
		return(1);
	}

	if(val<1000000000)
	{
		rec++;
		TkuTts_TranscribeNumber(dict, val/1000000, robuf);
		TkuTts_TranscribeWord(dict, "million", robuf);
		if(val%1000000)
			TkuTts_TranscribeNumber(dict, val%1000000, robuf);
		rec--;
		return(1);
	}

	if(val<1000000000000LL)
	{
		rec++;
		TkuTts_TranscribeNumber(dict, val/1000000000LL, robuf);
		TkuTts_TranscribeWord(dict, "billion", robuf);
		if(val%1000000000LL)
			TkuTts_TranscribeNumber(dict, val%1000000000LL, robuf);
		rec--;
		return(1);
	}

	if(val<1000000000000000LL)
	{
		rec++;
		TkuTts_TranscribeNumber(dict, val/1000000000000LL, robuf);
		TkuTts_TranscribeWord(dict, "trillion", robuf);
		if(val%1000000000000LL)
			TkuTts_TranscribeNumber(dict, val%1000000000000LL, robuf);
		rec--;
		return(1);
	}
	
	return(0);
}

int TkuTts_TranscribeToken(
	TkuTts_WordDict *dict, char *tok, char **robuf)
{
	char *s0, *s1, *ct;
	int val;

	if((*tok>='a') && (*tok<='z'))
	{
		return(TkuTts_TranscribeWord(dict, tok, robuf));
	}

	if((*tok>='A') && (*tok<='Z'))
	{
		return(TkuTts_TranscribeWord(dict, tok, robuf));
	}
	
	if((*tok>='0') && (*tok<='9'))
	{
		val=atoi(tok);
		return(TkuTts_TranscribeNumber(dict, val, robuf));
	}
	
	if(*tok==',')
	{
		ct=*robuf;
		*ct++='.';
		*ct=0;
		*robuf=ct;
		return(1);
	}
	
	if((*tok=='.') || *tok==';')
	{
		ct=*robuf;
		*ct++=' ';
		*ct=0;
		*robuf=ct;
		return(1);
	}
	
	return(0);
}

char *TkuTts_EatWhite(char *str)
{
	char *cs;
	cs=str;
	while(*cs && (*cs<=' '))
		cs++;
	return(cs);
}

char *TkuTts_ReadToken(char *str, char *obuf)
{
	char *cs, *ct;
	
	cs=TkuTts_EatWhite(str);
	if(!*cs)
	{
		*obuf=0;
		return(cs);
	}
	
	ct=obuf;

	if((*cs>='0') && (*cs<='9'))
	{
		while((*cs>='0') && (*cs<='9'))
			*ct++=*cs++;
		*ct=0;
		return(cs);
	}

	if(((*cs>='a') && (*cs<='z')) ||
		((*cs>='A') && (*cs<='Z')))
	{
		while(((*cs>='a') && (*cs<='z')) ||
			((*cs>='A') && (*cs<='Z')))
				*ct++=*cs++;
		*ct=0;
		return(cs);
	}
	
	*ct++=*cs++;
	*ct=0;
	return(cs);
}

int TkuTts_TranscribePhrase(
	TkuTts_WordDict *dict, char *str, char **robuf)
{
	char tbuf[256];
	char *cs;
	
	cs=str;
	while(*cs)
	{
		cs=TkuTts_ReadToken(cs, tbuf);
		TkuTts_TranscribeToken(dict, tbuf, robuf);
	}
	return(0);
}
