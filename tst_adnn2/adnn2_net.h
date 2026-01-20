typedef signed char		sbyte;
typedef signed short		s16;
typedef signed int		s32;
typedef signed long long	s64;

typedef unsigned char			byte;
typedef unsigned short		u16;
typedef unsigned int			u32;
typedef unsigned long long	u64;

typedef struct AdNn2_Layer_s	AdNn2_Layer;
typedef struct AdNn2_Net_s		AdNn2_Net;

#define ADNN2_WGHT16		//use 16-bit working weights internally
#define ADNN2_DOGA			//breed nets
// #define ADNN2_USE8RS		//jitter FP8 rounding for weights
// #define ADNN2_DOTIMESEED	//seed RNG using timing trick

// #define ADNN2_DO8RSM	1	//jitter mask for USE8RS (less jitter)
#define ADNN2_DO8RSM	3	//jitter mask for USE8RS
// #define ADNN2_DO8RSM	7	//jitter mask for USE8RS (more jitter)

// #define ADNN2_DOWGHTFP4		//do FP4 weights

#ifdef ADNN2_WGHT16
#define ADNN2_FWWGHT8		//if 16-bit weights, do forward pass as 8-bit
#define ADNN2_WGHTSGNMSK	0x8000
typedef u16 adnn2_wght;
#else
#define ADNN2_WGHTSGNMSK	0x80
typedef byte adnn2_wght;
#endif

struct AdNn2_Layer_s {
int nn_isz;			//layer input size
int nn_osz;			//layout output size
int nn_stzm;		//stride of zero mask
int nn_szzm;		//size of zero mask
byte nn_afn;		//activation function

int nn_stw4;		//stride with FP4

adnn2_wght *wght;		//neuron weights
adnn2_wght *wghtl;		//neuron weights
#ifdef ADNN2_WGHT16
byte	*wght8;		//neuron weights (FP8)
byte	*wght4;		//neuron weights (FP4)
#endif

byte *wght_zm;		//weight zero mask
byte *act_i;		//neuron activations (input)
byte *act_o;		//neuron activations (output size)
byte *err_o;		//neuron errors (output side)
byte *err_i;		//neuron errors (input side)
u16 *act_po;		//neuron pre-activations (output size)
};

struct AdNn2_Net_s {
int n_layer;
AdNn2_Layer *layer[16];

int v2, v3;
};
