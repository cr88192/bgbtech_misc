typedef signed char			sbyte;
typedef unsigned char			ubyte;
typedef unsigned char			byte;
typedef unsigned char			uchar;

typedef signed char			s8;
typedef unsigned char			u8;
typedef signed short			s16;
typedef unsigned short		u16;
typedef signed int			s32;
typedef unsigned int			u32;
typedef signed long long		s64;
typedef unsigned long long	u64;

typedef u16 bs3l_key;
typedef u64 bs3l_val;
typedef u16 bs3l_token;

#define BS3LVM_OPR_ADD		1
#define BS3LVM_OPR_SUB		2
#define BS3LVM_OPR_MUL		3
#define BS3LVM_OPR_DIV		4
#define BS3LVM_OPR_AND		5
#define BS3LVM_OPR_OR		6
#define BS3LVM_OPR_XOR		7
#define BS3LVM_OPR_SHL		8
#define BS3LVM_OPR_SHR		9
#define BS3LVM_OPR_SHRR		10
#define BS3LVM_OPR_MOD		11
#define BS3LVM_OPR_LLAND	12
#define BS3LVM_OPR_LLOR		13
#define BS3LVM_OPR_NOT		14
#define BS3LVM_OPR_LNOT		15
#define BS3LVM_OPR_CMPEQ	16
#define BS3LVM_OPR_CMPNE	17
#define BS3LVM_OPR_CMPLT	18
#define BS3LVM_OPR_CMPGT	19
#define BS3LVM_OPR_CMPLE	20
#define BS3LVM_OPR_CMPGE	21

typedef struct BS3LVM_Context_s BS3LVM_Context;
struct BS3LVM_Context_s {
bs3l_val dyn_vals[512];
bs3l_key dyn_keys[512];
u16 dyn_pos;
u16 frm_dynidx[256];
bs3l_val frm_this[256];
u16 frm_pos;
byte tag_break;

bs3l_val cur_this;
bs3l_val cur_return;
};

typedef struct BS3LVM_Object_s BS3LVM_Object;
struct BS3LVM_Object_s {
bs3l_val vals[16];
bs3l_key keys[16];
u16 nkey;
u16 n_tag;
byte z;
};


bs3l_key	bs3lvm_nkey_tag;
bs3l_key	bs3lvm_nkey_value;
bs3l_key	bs3lvm_nkey_opr;
bs3l_key	bs3lvm_nkey_init;
bs3l_key	bs3lvm_nkey_cond;
bs3l_key	bs3lvm_nkey_step;
bs3l_key	bs3lvm_nkey_lnode;
bs3l_key	bs3lvm_nkey_rnode;

bs3l_key	bs3lvm_nkey_unary;
bs3l_key	bs3lvm_nkey_binary;
bs3l_key	bs3lvm_nkey_assign;
bs3l_key	bs3lvm_nkey_null;
bs3l_key	bs3lvm_nkey_true;
bs3l_key	bs3lvm_nkey_false;
bs3l_key	bs3lvm_nkey_this;

bs3l_token	bs3lvm_token_lparen;
bs3l_token	bs3lvm_token_rparen;
bs3l_token	bs3lvm_token_lbrack;
bs3l_token	bs3lvm_token_rbrack;
bs3l_token	bs3lvm_token_lbrace;
bs3l_token	bs3lvm_token_rbrace;
bs3l_token	bs3lvm_token_langle;
bs3l_token	bs3lvm_token_rangle;

bs3l_token	bs3lvm_token_dot;
bs3l_token	bs3lvm_token_comma;
bs3l_token	bs3lvm_token_colon;
bs3l_token	bs3lvm_token_scolon;
bs3l_token	bs3lvm_token_excl;
bs3l_token	bs3lvm_token_tilde;
bs3l_token	bs3lvm_token_at;
bs3l_token	bs3lvm_token_pound;
bs3l_token	bs3lvm_token_prcnt;
bs3l_token	bs3lvm_token_carot;
bs3l_token	bs3lvm_token_and;
bs3l_token	bs3lvm_token_mult;
bs3l_token	bs3lvm_token_minus;
bs3l_token	bs3lvm_token_equals;
bs3l_token	bs3lvm_token_plus;
bs3l_token	bs3lvm_token_pipe;
bs3l_token	bs3lvm_token_slash;

bs3l_token	bs3lvm_token_lequal;
bs3l_token	bs3lvm_token_gequal;
bs3l_token	bs3lvm_token_eequal;
bs3l_token	bs3lvm_token_nequal;
bs3l_token	bs3lvm_token_shl;
bs3l_token	bs3lvm_token_shr;
bs3l_token	bs3lvm_token_shrr;

bs3l_token	bs3lvm_token_lland;
bs3l_token	bs3lvm_token_llor;
bs3l_token	bs3lvm_token_dbladd;
bs3l_token	bs3lvm_token_dblsub;

bs3l_token	bs3lvm_token_eqadd;
bs3l_token	bs3lvm_token_eqsub;
bs3l_token	bs3lvm_token_eqmul;
bs3l_token	bs3lvm_token_eqdiv;
bs3l_token	bs3lvm_token_eqmod;
bs3l_token	bs3lvm_token_eqand;
bs3l_token	bs3lvm_token_eqor;
bs3l_token	bs3lvm_token_eqxor;
bs3l_token	bs3lvm_token_eqshl;
bs3l_token	bs3lvm_token_eqshr;
bs3l_token	bs3lvm_token_eqshrr;

bs3l_token	bs3lvm_token_kw_break;
bs3l_token	bs3lvm_token_kw_case;
bs3l_token	bs3lvm_token_kw_continue;
bs3l_token	bs3lvm_token_kw_default;
bs3l_token	bs3lvm_token_kw_do;
bs3l_token	bs3lvm_token_kw_else;
bs3l_token	bs3lvm_token_kw_for;
bs3l_token	bs3lvm_token_kw_function;
bs3l_token	bs3lvm_token_kw_if;
bs3l_token	bs3lvm_token_kw_return;
bs3l_token	bs3lvm_token_kw_var;
bs3l_token	bs3lvm_token_kw_while;

byte bs3lvm_tok_tok2oprnum[256];

void *bs3lvm_malloc(int sz);
s64 bs3lvm_atoi(uchar *str);
double bs3lvm_atof(uchar *str);

int BS3LVM_GetTokenForString(uchar *name);
int BS3LVM_GetTokenForIdName(char *name);
int BS3LVM_GetTokenForIdString(char *name);
int BS3LVM_GetTokenForIdOpr(char *name);
int BS3LVM_GetTokenForIdNumber(char *name);
uchar *BS3LVM_GetStringForToken(bs3l_token tok);


int BS3LVM_ValSetSlotKeyValIdx(bs3l_val aobj, int idx,
	bs3l_key akey, bs3l_val aval);
bs3l_key BS3LVM_ValGetSlotKeyIdx(bs3l_val aobj, int idx);

int BS3LVM_ParseEatSemicolon(BS3LVM_Context *ctx, uchar **rcs);
bs3l_val BS3LVM_ParseExpr(BS3LVM_Context *ctx, uchar **rcs);
bs3l_val BS3LVM_ParseExprComma(BS3LVM_Context *ctx, uchar **rcs);
bs3l_val BS3LVM_ParseExprCommaNames(BS3LVM_Context *ctx, uchar **rcs);

bs3l_val BS3LVM_ParseStatement(BS3LVM_Context *ctx, uchar **rcs);
bs3l_val BS3LVM_ParseBlockStatement(BS3LVM_Context *ctx, uchar **rcs);

bs3l_val BS3LVM_Eval_Expr(BS3LVM_Context *ctx, bs3l_val val);

int BS3LVM_Eval_NodeIsOprComma(bs3l_val obj);
int BS3LVM_Eval_NodeIsOprLoadIndex(bs3l_val obj);
int BS3LVM_Eval_NodeIsOprLoadSlot(bs3l_val obj);
