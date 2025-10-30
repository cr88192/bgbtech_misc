int BS3LVM_ParseInitTokens(void)
{
	if(bs3lvm_token_dot)
		return(0);

	BS3LVM_InitTags();

	bs3lvm_token_lparen=BS3LVM_GetTokenForIdOpr("(");
	bs3lvm_token_rparen=BS3LVM_GetTokenForIdOpr(")");
	bs3lvm_token_lbrack=BS3LVM_GetTokenForIdOpr("[");
	bs3lvm_token_rbrack=BS3LVM_GetTokenForIdOpr("]");
	bs3lvm_token_lbrace=BS3LVM_GetTokenForIdOpr("{");
	bs3lvm_token_rbrace=BS3LVM_GetTokenForIdOpr("}");
	bs3lvm_token_langle=BS3LVM_GetTokenForIdOpr("<");
	bs3lvm_token_rangle=BS3LVM_GetTokenForIdOpr(">");

	bs3lvm_token_dot   =BS3LVM_GetTokenForIdOpr(".");
	bs3lvm_token_comma =BS3LVM_GetTokenForIdOpr(",");
	bs3lvm_token_colon =BS3LVM_GetTokenForIdOpr(":");
	bs3lvm_token_scolon=BS3LVM_GetTokenForIdOpr(";");

	bs3lvm_token_excl  =BS3LVM_GetTokenForIdOpr("!");
	bs3lvm_token_tilde =BS3LVM_GetTokenForIdOpr("~");
	bs3lvm_token_at    =BS3LVM_GetTokenForIdOpr("@");
	bs3lvm_token_pound =BS3LVM_GetTokenForIdOpr("#");
	bs3lvm_token_prcnt =BS3LVM_GetTokenForIdOpr("%");
	bs3lvm_token_carot =BS3LVM_GetTokenForIdOpr("^");
	bs3lvm_token_and   =BS3LVM_GetTokenForIdOpr("&");
	bs3lvm_token_mult  =BS3LVM_GetTokenForIdOpr("*");
	bs3lvm_token_minus =BS3LVM_GetTokenForIdOpr("-");
	bs3lvm_token_equals=BS3LVM_GetTokenForIdOpr("=");
	bs3lvm_token_plus  =BS3LVM_GetTokenForIdOpr("+");
	bs3lvm_token_pipe  =BS3LVM_GetTokenForIdOpr("|");
	bs3lvm_token_slash =BS3LVM_GetTokenForIdOpr("/");

	bs3lvm_token_lequal=BS3LVM_GetTokenForIdOpr("<=");
	bs3lvm_token_gequal=BS3LVM_GetTokenForIdOpr(">=");
	bs3lvm_token_eequal=BS3LVM_GetTokenForIdOpr("==");
	bs3lvm_token_nequal=BS3LVM_GetTokenForIdOpr("!=");

	bs3lvm_token_shl   =BS3LVM_GetTokenForIdOpr("<<");
	bs3lvm_token_shr   =BS3LVM_GetTokenForIdOpr(">>");
	bs3lvm_token_shrr  =BS3LVM_GetTokenForIdOpr(">>>");

	bs3lvm_token_lland =BS3LVM_GetTokenForIdOpr("&&");
	bs3lvm_token_llor  =BS3LVM_GetTokenForIdOpr("||");
	bs3lvm_token_dbladd=BS3LVM_GetTokenForIdOpr("++");
	bs3lvm_token_dblsub=BS3LVM_GetTokenForIdOpr("--");

	bs3lvm_token_eqadd =BS3LVM_GetTokenForIdOpr("+=");
	bs3lvm_token_eqsub =BS3LVM_GetTokenForIdOpr("-=");
	bs3lvm_token_eqmul =BS3LVM_GetTokenForIdOpr("*=");
	bs3lvm_token_eqdiv =BS3LVM_GetTokenForIdOpr("/=");
	bs3lvm_token_eqmod =BS3LVM_GetTokenForIdOpr("%=");
	bs3lvm_token_eqand =BS3LVM_GetTokenForIdOpr("&=");
	bs3lvm_token_eqor  =BS3LVM_GetTokenForIdOpr("|=");
	bs3lvm_token_eqxor =BS3LVM_GetTokenForIdOpr("^=");

	bs3lvm_token_eqshl =BS3LVM_GetTokenForIdOpr("<<=");
	bs3lvm_token_eqshr =BS3LVM_GetTokenForIdOpr(">>=");
	bs3lvm_token_eqshrr=BS3LVM_GetTokenForIdOpr(">>>=");

	bs3lvm_tok_tok2oprnum[bs3lvm_token_plus  ]=BS3LVM_OPR_ADD;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_minus ]=BS3LVM_OPR_SUB;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_mult  ]=BS3LVM_OPR_MUL;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_slash ]=BS3LVM_OPR_DIV;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_prcnt ]=BS3LVM_OPR_MOD;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_and   ]=BS3LVM_OPR_AND;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_pipe  ]=BS3LVM_OPR_OR;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_carot ]=BS3LVM_OPR_XOR;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_shl   ]=BS3LVM_OPR_SHL;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_shr   ]=BS3LVM_OPR_SHR;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_shrr  ]=BS3LVM_OPR_SHRR;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_lland ]=BS3LVM_OPR_LLAND;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_llor  ]=BS3LVM_OPR_LLOR;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_tilde ]=BS3LVM_OPR_NOT;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_excl  ]=BS3LVM_OPR_LNOT;

	bs3lvm_tok_tok2oprnum[bs3lvm_token_eequal]=BS3LVM_OPR_CMPEQ;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_nequal]=BS3LVM_OPR_CMPNE;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_langle]=BS3LVM_OPR_CMPLT;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_rangle]=BS3LVM_OPR_CMPGT;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_lequal]=BS3LVM_OPR_CMPLE;
	bs3lvm_tok_tok2oprnum[bs3lvm_token_gequal]=BS3LVM_OPR_CMPGE;

	bs3lvm_token_kw_break   =BS3LVM_GetTokenForIdName("break");
	bs3lvm_token_kw_case    =BS3LVM_GetTokenForIdName("case");
	bs3lvm_token_kw_continue=BS3LVM_GetTokenForIdName("continue");
	bs3lvm_token_kw_default =BS3LVM_GetTokenForIdName("default");
	bs3lvm_token_kw_do      =BS3LVM_GetTokenForIdName("do");
	bs3lvm_token_kw_else    =BS3LVM_GetTokenForIdName("else");
	bs3lvm_token_kw_for     =BS3LVM_GetTokenForIdName("for");
	bs3lvm_token_kw_function=BS3LVM_GetTokenForIdName("function");
	bs3lvm_token_kw_if      =BS3LVM_GetTokenForIdName("if");
	bs3lvm_token_kw_return  =BS3LVM_GetTokenForIdName("return");
	bs3lvm_token_kw_var     =BS3LVM_GetTokenForIdName("var");
	bs3lvm_token_kw_while   =BS3LVM_GetTokenForIdName("while");

	bs3lvm_nkey_unary	=BS3LVM_GetTokenForIdName("unary");
	bs3lvm_nkey_binary	=BS3LVM_GetTokenForIdName("binary");
	bs3lvm_nkey_assign	=BS3LVM_GetTokenForIdName("assign");
	bs3lvm_nkey_null	=BS3LVM_GetTokenForIdName("null");
	bs3lvm_nkey_true	=BS3LVM_GetTokenForIdName("true");
	bs3lvm_nkey_false	=BS3LVM_GetTokenForIdName("false");
	bs3lvm_nkey_this	=BS3LVM_GetTokenForIdName("this");

	return(1);
}

bs3l_val BS3LVM_ParseTokenToLit(BS3LVM_Context *ctx, bs3l_token tok)
{
	uchar *tkb, *cs;
	bs3l_val nv0;
	double f;
	s64 li;
	int n;
	
	tkb=BS3LVM_GetStringForToken(tok);
	
	if(*tkb=='I')
		{ return(BS3LVM_ValWrapSymbol(tkb+1)); }

	if(*tkb=='L')
	{
		li=bs3lvm_atoi(tkb+1);
		return(BS3LVM_ValWrapFixnum(li));
	}

	if(*tkb=='D')
	{
		f=bs3lvm_atof(tkb+1);
		return(BS3LVM_ValWrapFlonum(f));
	}

	if(*tkb=='S')
		{ return(BS3LVM_ValWrapString(tkb+1)); }
	
	if(*tkb=='C')
	{
		li=0; n=0; cs=tkb+1;
		while(*cs)
			{ li|=((u64)(*cs++))<<(n*8); n++; }
		return(BS3LVM_ValWrapFixnum(li));
	}

	return(0);
}


bs3l_val BS3LVM_ParseWrapUnaryOp(BS3LVM_Context *ctx,
	bs3l_token tok, bs3l_val nv1)
{
	bs3l_val nv0;
	uchar *tkb;

	tkb=BS3LVM_GetStringForToken(tok);
	
	nv0=BS3LVM_ValAllocObj();
//	BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_tag, "tag", "unary");
	BS3LVM_ValSetSlotTag(nv0, "unary");
	BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_opr, "opr", tkb+1);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_value, "value", nv1);
	return(nv0);
}

bs3l_val BS3LVM_ParseWrapBinaryOp(BS3LVM_Context *ctx,
	bs3l_token tok, bs3l_val nv1, bs3l_val nv2)
{
	bs3l_val nv0;
	uchar *tkb;

	tkb=BS3LVM_GetStringForToken(tok);
	
	nv0=BS3LVM_ValAllocObj();
//	BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_tag, "tag", "binary");
	BS3LVM_ValSetSlotTag(nv0, "binary");
	BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_opr, "opr", tkb+1);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_lnode, "lnode", nv1);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_rnode, "rnode", nv2);
	return(nv0);
}

bs3l_val BS3LVM_ParseWrapAssignOp(BS3LVM_Context *ctx,
	bs3l_token tok, bs3l_val nv1, bs3l_val nv2)
{
	uchar tb[64];
	bs3l_val nv0;
	uchar *tkb;
	int i;

	tkb=BS3LVM_GetStringForToken(tok);
	
	strcpy(tb, tkb+1);
	i=strlen(tb);
	if((i>0) && (tb[i-1]=='='))
		tb[i-1]=0;

	nv0=BS3LVM_ValAllocObj();
//	BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_tag, "tag", "assign");
	BS3LVM_ValSetSlotTag(nv0, "assign");
	if(tb[0])
		BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_opr, "opr", tb);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_lnode, "lnode", nv1);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_rnode, "rnode", nv2);
	return(nv0);
}

bs3l_val BS3LVM_ParseWrapTagUnary(BS3LVM_Context *ctx,
	bs3l_token tok, bs3l_val nv1)
{
	bs3l_val nv0;
	uchar *tkb;

	tkb=BS3LVM_GetStringForToken(tok);
	
	nv0=BS3LVM_ValAllocObj();
//	BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_tag, "tag", tkb+1);
	BS3LVM_ValSetSlotTag(nv0, tkb+1);
	if(nv1)
		BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_value, "value", nv1);
	return(nv0);
}

bs3l_val BS3LVM_ParseWrapTag(BS3LVM_Context *ctx, bs3l_token tok)
{
	return(BS3LVM_ParseWrapTagUnary(ctx, tok, 0));
}

bs3l_val BS3LVM_ParseWrapDummy(BS3LVM_Context *ctx)
{
	bs3l_val nv0;
	nv0=BS3LVM_ValAllocObj();
//	BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_tag, "tag", "dummy");
	BS3LVM_ValSetSlotTag(nv0, "dummy");
	return(nv0);
}

bs3l_val BS3LVM_ParseWrapTagBinary(BS3LVM_Context *ctx,
	bs3l_token tok, bs3l_val nv1, bs3l_val nv2)
{
	bs3l_val nv0;
	uchar *tkb;

	tkb=BS3LVM_GetStringForToken(tok);
	
	nv0=BS3LVM_ValAllocObj();
//	BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_tag, "tag", tkb+1);
	BS3LVM_ValSetSlotTag(nv0, tkb+1);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_lnode, "lnode", nv1);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_rnode, "rnode", nv2);
	return(nv0);
}

bs3l_val BS3LVM_ParseWrapTagCond(BS3LVM_Context *ctx,
	bs3l_token tok, bs3l_val nv_cond, bs3l_val nv_then, bs3l_val nv_else)
{
	bs3l_val nv0;
	uchar *tkb;

	tkb=BS3LVM_GetStringForToken(tok);

	nv0=BS3LVM_ValAllocObj();
//	BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_tag, "tag", tkb+1);
	BS3LVM_ValSetSlotTag(nv0, tkb+1);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_cond, "cond", nv_cond);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_lnode, "lnode", nv_then);
	if(nv_else)
		BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_rnode, "rnode", nv_else);
	return(nv0);
}

bs3l_val BS3LVM_ParseWrapIf(BS3LVM_Context *ctx,
	bs3l_val nv_cond, bs3l_val nv_then, bs3l_val nv_else)
{
	return(BS3LVM_ParseWrapTagCond(ctx, bs3lvm_token_kw_if,
		nv_cond, nv_then, nv_else));
}

bs3l_val BS3LVM_ParseWrapWhile(BS3LVM_Context *ctx,
	bs3l_val nv_cond, bs3l_val nv_then)
{
	return(BS3LVM_ParseWrapTagCond(ctx, bs3lvm_token_kw_while,
		nv_cond, nv_then, 0));
}

bs3l_val BS3LVM_ParseWrapFor(BS3LVM_Context *ctx,
	bs3l_val nv_init, bs3l_val nv_cond, bs3l_val nv_step, bs3l_val nv_body)
{
	bs3l_val nv0;

	nv0=BS3LVM_ValAllocObj();
//	BS3LVM_ValSetSlotKNameSym(nv0, &bs3lvm_nkey_tag, "tag", "for");
	BS3LVM_ValSetSlotTag(nv0, "for");
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_init, "init", nv_init);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_cond, "cond", nv_cond);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_step, "step", nv_step);
	BS3LVM_ValSetSlotKName(nv0, &bs3lvm_nkey_value, "value", nv_body);
	return(nv0);
}

bs3l_val BS3LVM_ParseExprLit(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	
	if(tok0==bs3lvm_token_lparen)
	{
		cs=cs1;
		nv0=BS3LVM_ParseExpr(ctx, &cs);
		if(!nv0)	return(0);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
		if(tok0==bs3lvm_token_rparen)
		{
			*rcs=cs1;
			return(nv0);
		}
		return(0);
	}

	if(tok0==bs3lvm_token_lbrack)
	{
		cs=cs1;
		nv0=BS3LVM_ParseExprComma(ctx, &cs);
		if(!nv0)	return(0);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok1);
		if(tok1==bs3lvm_token_rbrack)
		{
			nv0=BS3LVM_ParseWrapUnaryOp(ctx, tok0, nv0);
			*rcs=cs1;
			return(nv0);
		}
		return(0);
	}

	if(tok0==bs3lvm_token_lbrace)
	{
		cs=cs1;
		nv0=BS3LVM_ParseExprCommaNames(ctx, &cs);
		if(!nv0)	return(0);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok1);
		if(tok1==bs3lvm_token_rbrace)
		{
			nv0=BS3LVM_ParseWrapUnaryOp(ctx, tok0, nv0);
			*rcs=cs1;
			return(nv0);
		}
		return(0);
	}
	
	tt0=BS3LVM_GetTTagForToken(tok0);
	if((tt0=='I') || (tt0=='L') || (tt0=='D') || (tt0=='S') || (tt0=='C'))
	{
		*rcs=cs1;
		return(BS3LVM_ParseTokenToLit(ctx, tok0));
	}

	return(0);
}

bs3l_val BS3LVM_ParseExprPostfix(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	
	nv0=BS3LVM_ParseExprLit(ctx, &cs);
	
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	while(	(tok0==bs3lvm_token_lbrack ) ||
			(tok0==bs3lvm_token_lparen ) ||
			(tok0==bs3lvm_token_dbladd ) ||
			(tok0==bs3lvm_token_dblsub ) ||
			(tok0==bs3lvm_token_dot ) )
	{
		cs=cs1;
		if(		(tok0==bs3lvm_token_lbrack ) ||
				(tok0==bs3lvm_token_lparen ) )
		{
			nv1=BS3LVM_ParseExprComma(ctx, &cs);
			cs1=BS3LVM_TokenReadID(ctx, cs, &tok1);
			if(	(tok1==bs3lvm_token_rbrack ) ||
				(tok1==bs3lvm_token_rparen ) )
					{ cs=cs1; }
			nv0=BS3LVM_ParseWrapBinaryOp(ctx, tok0, nv0, nv1);
		}
		else if(	(tok0==bs3lvm_token_dbladd ) ||
					(tok0==bs3lvm_token_dblsub ) )
		{
			nv0=BS3LVM_ParseWrapUnaryOp(ctx, tok0, nv0);
		}else if(tok0==bs3lvm_token_dot)
		{
			nv1=BS3LVM_ParseExprLit(ctx, &cs);
			nv0=BS3LVM_ParseWrapBinaryOp(ctx, tok0, nv0, nv1);
		}
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	}
	*rcs=cs;
	return(nv0);
}

bs3l_val BS3LVM_ParseExprPrefix(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	cs=BS3LVM_TokenReadID(ctx, cs, &tok0);
	
	if(	(tok0==bs3lvm_token_excl) ||
		(tok0==bs3lvm_token_tilde) ||
		(tok0==bs3lvm_token_minus) ||
		(tok0==bs3lvm_token_plus) ||
		(tok0==bs3lvm_token_dbladd ) ||
		(tok0==bs3lvm_token_dblsub ) )
	{
		nv1=BS3LVM_ParseExprPrefix(ctx, &cs);
		nv0=BS3LVM_ParseWrapUnaryOp(ctx, tok0, nv1);
		*rcs=cs;
		return(nv0);
	}
	
	nv0=BS3LVM_ParseExprPostfix(ctx, rcs);
	return(nv0);
}

bs3l_val BS3LVM_ParseExprMulDiv(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	
	nv0=BS3LVM_ParseExprPrefix(ctx, &cs);
	
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	while(	(tok0==bs3lvm_token_mult ) ||
			(tok0==bs3lvm_token_slash) ||
			(tok0==bs3lvm_token_prcnt) )
	{
		cs=cs1;
		nv1=BS3LVM_ParseExprPrefix(ctx, &cs);
		nv0=BS3LVM_ParseWrapBinaryOp(ctx, tok0, nv0, nv1);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	}
	*rcs=cs;
	return(nv0);
}

bs3l_val BS3LVM_ParseExprAddSub(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	nv0=BS3LVM_ParseExprMulDiv(ctx, &cs);
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	while(	(tok0==bs3lvm_token_plus ) ||
			(tok0==bs3lvm_token_minus) )
	{
		cs=cs1;
		nv1=BS3LVM_ParseExprMulDiv(ctx, &cs);
		nv0=BS3LVM_ParseWrapBinaryOp(ctx, tok0, nv0, nv1);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	}
	*rcs=cs;
	return(nv0);
}

bs3l_val BS3LVM_ParseExprShlr(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	nv0=BS3LVM_ParseExprAddSub(ctx, &cs);
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	while(	(tok0==bs3lvm_token_shl ) ||
			(tok0==bs3lvm_token_shr ) ||
			(tok0==bs3lvm_token_shrr) )
	{
		cs=cs1;
		nv1=BS3LVM_ParseExprAddSub(ctx, &cs);
		nv0=BS3LVM_ParseWrapBinaryOp(ctx, tok0, nv0, nv1);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	}
	*rcs=cs;
	return(nv0);
}

bs3l_val BS3LVM_ParseExprCmp(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	nv0=BS3LVM_ParseExprShlr(ctx, &cs);
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	while(	(tok0==bs3lvm_token_langle) ||
			(tok0==bs3lvm_token_rangle) ||
			(tok0==bs3lvm_token_eequal) ||
			(tok0==bs3lvm_token_nequal) ||
			(tok0==bs3lvm_token_lequal) ||
			(tok0==bs3lvm_token_gequal) )
	{
		cs=cs1;
		nv1=BS3LVM_ParseExprShlr(ctx, &cs);
		nv0=BS3LVM_ParseWrapBinaryOp(ctx, tok0, nv0, nv1);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	}
	*rcs=cs;
	return(nv0);
}

bs3l_val BS3LVM_ParseExprAndOr(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	nv0=BS3LVM_ParseExprCmp(ctx, &cs);
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	while(	(tok0==bs3lvm_token_lland) ||
			(tok0==bs3lvm_token_llor ) ||
			(tok0==bs3lvm_token_and  ) ||
			(tok0==bs3lvm_token_pipe ) ||
			(tok0==bs3lvm_token_carot) )
	{
		cs=cs1;
		nv1=BS3LVM_ParseExprCmp(ctx, &cs);
		nv0=BS3LVM_ParseWrapBinaryOp(ctx, tok0, nv0, nv1);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	}
	*rcs=cs;
	return(nv0);
}


bs3l_val BS3LVM_ParseExprAssign(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	nv0=BS3LVM_ParseExprAndOr(ctx, &cs);
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	if(	(tok0==bs3lvm_token_equals) ||
			(tok0==bs3lvm_token_eqadd ) ||
			(tok0==bs3lvm_token_eqsub ) ||
			(tok0==bs3lvm_token_eqmul ) ||
			(tok0==bs3lvm_token_eqdiv ) ||
			(tok0==bs3lvm_token_eqmod ) ||
			(tok0==bs3lvm_token_eqand ) ||
			(tok0==bs3lvm_token_eqor  ) ||
			(tok0==bs3lvm_token_eqxor ) ||
			(tok0==bs3lvm_token_eqshl ) ||
			(tok0==bs3lvm_token_eqshr ) ||
			(tok0==bs3lvm_token_eqshrr) )
	{
		cs=cs1;
		nv1=BS3LVM_ParseExprAssign(ctx, &cs);
		nv0=BS3LVM_ParseWrapAssignOp(ctx, tok0, nv0, nv1);
//		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	}
	*rcs=cs;
	return(nv0);
}

bs3l_val BS3LVM_ParseExprComma(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	nv0=BS3LVM_ParseExprAssign(ctx, &cs);
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	while(tok0==bs3lvm_token_comma)
	{
		cs=cs1;
		nv1=BS3LVM_ParseExprAssign(ctx, &cs);
		nv0=BS3LVM_ParseWrapBinaryOp(ctx, tok0, nv0, nv1);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	}
	*rcs=cs;
	return(nv0);
}

bs3l_val BS3LVM_ParseExprNamed(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1, *cs2;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1, nv2;
	byte tt0;

	cs=*rcs;
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	cs2=BS3LVM_TokenReadID(ctx, cs1, &tok1);
	if(BS3LVM_CheckTokenIdentifierP(tok0) &&
		(tok1==bs3lvm_token_colon))
	{
		cs=cs2;
		nv1=BS3LVM_ParseExprAssign(ctx, &cs);
		nv2=BS3LVM_ParseTokenToLit(ctx, tok0);
		nv0=BS3LVM_ParseWrapAssignOp(ctx, bs3lvm_token_equals, nv2, nv1);
	}else
	{
		nv0=BS3LVM_ParseExprAssign(ctx, &cs);
	}
	*rcs=cs;
	return(nv0);
}

bs3l_val BS3LVM_ParseExprCommaNames(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1;
	byte tt0;

	cs=*rcs;
	nv0=BS3LVM_ParseExprNamed(ctx, &cs);
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	while(tok0==bs3lvm_token_comma)
	{
		cs=cs1;
		nv1=BS3LVM_ParseExprNamed(ctx, &cs);
		nv0=BS3LVM_ParseWrapBinaryOp(ctx, tok0, nv0, nv1);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	}
	*rcs=cs;
	return(nv0);
}


bs3l_val BS3LVM_ParseExpr(BS3LVM_Context *ctx, uchar **rcs)
{
	return(BS3LVM_ParseExprAssign(ctx, rcs));
}

int BS3LVM_ParseEatSemicolon(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar *cs, *cs1;
	bs3l_token	tok0, tok1;

	cs=*rcs;
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	if(tok0==bs3lvm_token_scolon)
	{
		*rcs=cs1;
		return(1);
	}
	return(0);
}

bs3l_val BS3LVM_ParseStatement(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar		*cs, *cs1, *cs2, *cs3;
	uchar		*tkb;
	bs3l_token	tok0, tok1, tok2;
	bs3l_val	nv0, nv1, nv2, nv3, nv4;
	byte tt0;

	BS3LVM_ParseInitTokens();

	cs=*rcs;
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
	cs2=BS3LVM_TokenReadID(ctx, cs1, &tok1);
	cs3=BS3LVM_TokenReadID(ctx, cs2, &tok2);

	if(tok0==bs3lvm_token_scolon)
	{
		nv0=BS3LVM_ParseWrapDummy(ctx);
		*rcs=cs1;
		return(nv0);
	}

	if(	(tok0==bs3lvm_token_kw_break) ||
		(tok0==bs3lvm_token_kw_continue))
	{
		cs=cs1;
		nv0=BS3LVM_ParseWrapTag(ctx, tok0);
		BS3LVM_ParseEatSemicolon(ctx, &cs);
		*rcs=cs;
		return(nv0);
	}
	
	if(tok0==bs3lvm_token_kw_var)
	{
		cs=cs1;
		nv0=BS3LVM_ParseExprComma(ctx, &cs);
		nv0=BS3LVM_ParseWrapTagUnary(ctx, bs3lvm_token_kw_var, nv0);
		BS3LVM_ParseEatSemicolon(ctx, &cs);
		*rcs=cs;
		return(nv0);
	}

	if(tok0==bs3lvm_token_kw_return)
	{
		cs=cs1;
		nv0=BS3LVM_ParseExpr(ctx, &cs);
		nv0=BS3LVM_ParseWrapTagUnary(ctx, bs3lvm_token_kw_return, nv0);
		BS3LVM_ParseEatSemicolon(ctx, &cs);
		*rcs=cs;
		return(nv0);
	}

	if(tok0==bs3lvm_token_kw_if)
	{
		cs=cs1;
		nv1=BS3LVM_ParseExprLit(ctx, &cs);
		nv2=BS3LVM_ParseBlockStatement(ctx, &cs);
		nv3=0;
		
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
		if(tok0==bs3lvm_token_kw_else)
		{
			cs1=cs;
			nv3=BS3LVM_ParseBlockStatement(ctx, &cs);
		}

		nv0=BS3LVM_ParseWrapIf(ctx, nv1, nv2, nv3);
		*rcs=cs;
		return(nv0);
	}
	

	if(tok0==bs3lvm_token_kw_while)
	{
		cs=cs1;
		nv1=BS3LVM_ParseExprLit(ctx, &cs);
		nv2=BS3LVM_ParseBlockStatement(ctx, &cs);
		nv0=BS3LVM_ParseWrapWhile(ctx, nv1, nv2);
		*rcs=cs;
		return(nv0);
	}

	if(	(tok0==bs3lvm_token_kw_for) &&
		(tok1==bs3lvm_token_lparen))
	{
		cs=cs2;
		nv1=BS3LVM_ParseStatement(ctx, &cs);
		nv2=BS3LVM_ParseStatement(ctx, &cs);
		nv3=BS3LVM_ParseStatement(ctx, &cs);

		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
		if(tok0!=bs3lvm_token_rparen)
			return(0);
		cs=cs1;
		nv4=BS3LVM_ParseBlockStatement(ctx, &cs);

		nv0=BS3LVM_ParseWrapFor(ctx, nv1, nv2, nv3, nv4);
		*rcs=cs;
		return(nv0);
	}

	if(	(tok0==bs3lvm_token_kw_function) &&
		BS3LVM_CheckTokenIdentifierP(tok1) &&
		(tok2==bs3lvm_token_lparen))
	{
		cs=cs3;
		nv1=BS3LVM_ParseExprComma(ctx, &cs);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
		if(tok0!=bs3lvm_token_rparen)
			return(0);
		cs=cs1;

		nv2=BS3LVM_ParseBlockStatement(ctx, &cs);

		tkb=BS3LVM_GetStringForToken(tok1);
		nv0=BS3LVM_ParseWrapTagCond(ctx,
			bs3lvm_token_kw_function,
			BS3LVM_ValWrapSymbol(tkb+1),
			nv1, nv2);
		*rcs=cs;
		return(nv0);
	}
	
	if(tok0==bs3lvm_token_kw_case)
	{
		cs=cs3;
		nv1=BS3LVM_ParseExpr(ctx, &cs);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
		if(tok0!=bs3lvm_token_colon)
			return(0);
		cs=cs1;
		nv0=BS3LVM_ParseWrapTagUnary(ctx, bs3lvm_token_kw_case, nv1);
		*rcs=cs;
		return(nv0);
	}
	
	nv0=BS3LVM_ParseExprComma(ctx, &cs);
	if(nv0)
	{
		BS3LVM_ParseEatSemicolon(ctx, &cs);
		*rcs=cs;
		return(nv0);
	}
	
	return(0);
}

bs3l_val BS3LVM_ParseStatementList(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar		*cs, *cs1, *cs2;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1, nv2, nv3, nv4;
	byte tt0;

	BS3LVM_ParseInitTokens();

	cs=*rcs;
	nv0=BS3LVM_ParseStatement(ctx, &cs);
	if(!nv0)
		return(0);

	cs=BS3LVM_TokenEatWhite(cs);
	if(*cs)
	{
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
		if((tok0!=bs3lvm_token_rbrace) && (cs1!=cs))
		{
			nv1=BS3LVM_ParseStatementList(ctx, &cs);
			nv0=BS3LVM_ParseWrapTagBinary(ctx,
				bs3lvm_token_scolon, nv0, nv1);
		}
	}
	*rcs=cs;
	return(nv0);
}

bs3l_val BS3LVM_ParseBlockStatement(BS3LVM_Context *ctx, uchar **rcs)
{
	uchar		*cs, *cs1, *cs2;
	bs3l_token	tok0, tok1;
	bs3l_val	nv0, nv1, nv2, nv3, nv4;
	byte tt0;

	BS3LVM_ParseInitTokens();

	cs=*rcs;
	cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
//	cs2=BS3LVM_TokenReadID(ctx, cs1, &tok1);
	
	if(tok0==bs3lvm_token_lbrace)
	{
		cs=cs1;
		nv0=BS3LVM_ParseStatementList(ctx, &cs);
		cs1=BS3LVM_TokenReadID(ctx, cs, &tok0);
		if(tok0!=bs3lvm_token_rbrace)
			return(0);
		*rcs=cs1;
		return(nv0);
	}

	nv0=BS3LVM_ParseStatement(ctx, &cs);
	if(!nv0)
		return(0);
	*rcs=cs;
	return(nv0);
}
