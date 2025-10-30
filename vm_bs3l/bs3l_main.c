#include "bs3l_multi.c"

void *bs3lvm_malloc(int sz)
{
	void *ptr;
	
	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

s64 bs3lvm_atoi(uchar *str)
{
	return(atoi(str));
}

double bs3lvm_atof(uchar *str)
{
	return(atof(str));
}

byte *BS3L_LoadFile(char *name, int *rsz)
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

int BS3L_StoreFile(char *name, void *buf, int sz)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)
		return(-1);
	
	fwrite(buf, 1, sz, fd);
	fclose(fd);
	return(0);
}

bs3l_val BS3LVM_PrintLn(BS3LVM_Context *ctx, int n_args, bs3l_val *args)
{
	char *s0;
	s64 li;
	int i;

	for(i=0; i<n_args; i++)
	{
		if(BS3LVM_ValIsString(args[i]))
		{
			s0=BS3LVM_ValUnwrapString(args[i]);
			printf("%s", s0);
		}
		if(BS3LVM_ValIsFixnum(args[i]))
		{
			li=BS3LVM_ValUnwrapFixnum(args[i]);
			printf("%lld", li);
		}
	}
	printf("\n");
}

int main()
{
	BS3LVM_Context *ctx;
	byte *ibuf, *cs;
	bs3l_val ast;
	int isz;
	
	isz=0;
	ibuf=BS3L_LoadFile("test_base0.bs", &isz);
	
	ctx=BS3LVM_AllocContext();
	BS3LVM_Eval_BindNativeFunc(ctx, "println", BS3LVM_PrintLn);
	cs=ibuf;
	ast=BS3LVM_ParseStatementList(ctx, &cs);
	BS3LVM_Eval_Expr(ctx, ast);
}
