uchar		*bs3lvm_tok_str[32768];
u16			 bs3lvm_tok_chn[32768];
u16			 bs3lvm_tok_tab[256];
u16			 bs3lvm_tok_cnt;

int BS3LVM_GetTokenForString(uchar *name)
{
	int h, i;

	if(!name)
		return(0);

	h=BS3LVM_HashTagForName((char *)name);
	
	i=bs3lvm_tok_tab[h];
	while(i>0)
	{
		if(!strcmp(bs3lvm_tok_str[i], (char *)name))
			return(i);
		i=bs3lvm_tok_chn[i];
	}
	
	if(bs3lvm_tok_cnt<1)
		bs3lvm_tok_cnt=1;
	i=bs3lvm_tok_cnt++;

	bs3lvm_tok_str[i]=(uchar *)strdup((char *)name);
	bs3lvm_tok_chn[i]=bs3lvm_tok_tab[h];
	bs3lvm_tok_tab[h]=i;
	return(i);
}

int BS3LVM_GetTokenForIdName(char *name)
{
	char tb[128];
	tb[0]='I';
	strncpy(tb+1, name, 126);
	return(BS3LVM_GetTokenForString((uchar *)tb));
}

int BS3LVM_GetTokenForIdString(char *name)
{
	char tb[512];
	tb[0]='S';
	strncpy(tb+1, name, 510);
	return(BS3LVM_GetTokenForString((uchar *)tb));
}

int BS3LVM_GetTokenForIdOpr(char *name)
{
	char tb[128];
	tb[0]='O';
	strncpy(tb+1, name, 126);
	return(BS3LVM_GetTokenForString((uchar *)tb));
}

int BS3LVM_GetTokenForIdNumber(char *name)
{
	char tb[128];
	tb[0]='L';
	strncpy(tb+1, name, 126);
	return(BS3LVM_GetTokenForString((uchar *)tb));
}

uchar *BS3LVM_GetStringForToken(bs3l_token tok)
{
	return(bs3lvm_tok_str[tok]);
}

uchar BS3LVM_GetTTagForToken(bs3l_token tok)
{
	return(bs3lvm_tok_str[tok][0]);
}

int BS3LVM_CheckTokenIdentifierP(bs3l_token tok)
{
	return(BS3LVM_GetTTagForToken(tok)=='I');
}

uchar *BS3LVM_TokenEatWhite(uchar *srcbuf)
{
	char *cs, *ct;
	char c0;
	
	cs=srcbuf;
	c0=*cs;
	if(!c0)
		return(cs);
	if((c0>' ') && (c0!='/'))
		return(cs);
	
//	c0=*cs;
	while(c0 && (c0<=' '))
		{ cs++; c0=*cs; }
	
	if(c0=='/')
	{
		if(cs[1]=='/')
		{
			cs+=2;
			while(*cs && (*cs!='\n'))
				cs++;
			if(*cs=='\n')
				return(BS3LVM_TokenEatWhite(cs+1));
		}

		if(cs[1]=='*')
		{
			cs+=2;
			while(*cs && !((cs[0]=='*') && (cs[1]=='/')))
				cs++;
			if(cs[0]=='*' && cs[1]=='/')
				return(BS3LVM_TokenEatWhite(cs+2));
		}
	}
	return(cs);
}

/*
 * I=identifier
 * L=number (integer)
 * D=number (floating point)
 * S=string
 * C=charstring
 * O=operator
 */
uchar *BS3LVM_TokenReadBasic(uchar *srcbuf, uchar *tokbuf)
{
	uchar *cs, *ct;
	uchar c0, c1;
	
	cs=srcbuf;
	ct=tokbuf;
	
	cs=BS3LVM_TokenEatWhite(cs);

	c0=*cs;
	if(!c0)
	{
		*ct++=0;
		return(cs);
	}
	
	if(	((c0>='a') && (c0<='z')) ||
		((c0>='A') && (c0<='Z')) ||
		(c0=='_') )
	{
		*ct++='I';
		while(	((c0>='a') && (c0<='z')) ||
				((c0>='A') && (c0<='Z')) ||
				((c0>='0') && (c0<='9')) ||
				(c0=='_') )
			{ *ct++=c0; cs++; c0=*cs; }
		*ct++=0;
		return(cs);
	}
	
	if((c0>='0') && (c0<='9'))
	{
		*ct++='L';
		while(	((c0>='0') && (c0<='9')) || (c0=='_') )
		{
			if(c0!='_')
				*ct++=c0;
			cs++; c0=*cs;
		}
		
		if(c0=='.')
		{
			tokbuf[0]='D';

			*ct++=c0; cs++; c0=*cs;

			while(	((c0>='0') && (c0<='9')) || (c0=='_') )
			{
				if(c0!='_')
					*ct++=c0;
				cs++; c0=*cs;
			}
			
			if((c0=='e') || (c0=='E'))
			{
				*ct++=c0; cs++; c0=*cs;
				if((c0=='+') || (c0=='-'))
					{ *ct++=c0; cs++; c0=*cs; }
				while(	((c0>='0') && (c0<='9')) || (c0=='_') )
				{
					if(c0!='_')
						*ct++=c0;
					cs++; c0=*cs;
				}
			}
		}
		*ct++=0;
		return(cs);
	}
	
	if((c0=='\"') || (c0=='\''))
	{
		if(c0=='\"')
			*ct++='S';
		else
			*ct++='C';
		cs++;
		
		while(*cs)
		{
			c1=*cs;
			if(c1==c0)
				break;
			if(c1!='\\')
				{ *ct++=*cs++; continue; }
		}
		if(c1==c0)
			cs++;
		*ct++=0;
		return(cs);
	}
	
	if(	(c0=='.') || (c0==',') || (c0==';') || (c0==':') ||
		(c0=='(') || (c0==')') || (c0=='[') || (c0==']') ||
		(c0=='{') || (c0=='}'))
	{
		*ct++='O';
		*ct++=*cs++;
		*ct++=0;
		return(cs);
	}

	if(	(c0=='~') || (c0=='!') || (c0=='@') || (c0=='#') ||
		(c0=='$') || (c0=='%') || (c0=='^') || (c0=='&') ||
		(c0=='*') || (c0=='-') || (c0=='+') || (c0=='=') ||
		(c0=='<') || (c0=='>') || (c0=='/') || (c0=='?') ||
		(c0=='|') || (c0=='`') )
	{
		*ct++='O';
		*ct++=*cs++;
		c0=*cs;
		while(	(c0=='~') || (c0=='!') || (c0=='@') || (c0=='#') ||
			(c0=='$') || (c0=='%') || (c0=='^') || (c0=='&') ||
			(c0=='*') || (c0=='-') || (c0=='+') || (c0=='=') ||
			(c0=='<') || (c0=='>') || (c0=='/') || (c0=='?') ||
			(c0=='|') || (c0=='`') )
				{ *ct++=c0; cs++; c0=*cs; }
		*ct++=0;
		return(cs);
	}
	
	*ct=0;
	return(cs);
}

uchar *BS3LVM_TokenReadID(BS3LVM_Context *ctx, uchar *srcbuf, bs3l_token *rid)
{
	uchar tokb[256];
	uchar *cs;
	int id;
	
	cs=srcbuf;
	cs=BS3LVM_TokenReadBasic(cs, tokb);
	if(rid)
	{
		id=BS3LVM_GetTokenForString(tokb);
		*rid=id;
	}
	return(cs);
}
