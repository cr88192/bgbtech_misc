#if 0
typedef struct {
	s32 left;
	s32 top;
	s32 right;
	s32 bottom;
}BGBBTJ_RECT;

typedef struct {
	byte peRed;
	byte peGreen;
	byte peBlue;
	byte peFlags;
}BGBBTJ_PALETTEENTRY;

typedef struct {
u32 dwMicroSecPerFrame;
u32 dwMaxBytesPerSec;
u32 dwPaddingGranularity;
u32 dwFlags;
u32 dwTotalFrames;
u32 dwInitialFrames;
u32 dwStreams;
u32 dwSuggestedBufferSize;
u32 dwWidth;
u32 dwHeight;
u32 dwScale;
u32 dwRate;
u32 dwStart;
u32 dwLength;
}BGBBTJ_MainAVIHeader;

typedef struct {
u32 fccType;
u32 fccHandler;
u32 dwFlags;
u16 wPriority;
u16 wLanguage;
u32 dwInitialFrames;
u32 dwScale;
u32 dwRate;
u32 dwStart;
u32 dwLength;
u32 dwSuggestedBufferSize;
u32 dwQuality;
u32 dwSampleSize;
BGBBTJ_RECT rcFrame;
}BGBBTJ_AVIStreamHeader;

typedef struct {
unsigned int biSize;
unsigned int biWidth;
unsigned int biHeight;
unsigned short biPlanes;
unsigned short biBitCount;
unsigned int biCompression;
unsigned int biSizeImage;
unsigned int biXPelsPerMeter;
unsigned int biYPelsPerMeter;
unsigned int biClrUsed;
unsigned int biClrImportant;
}BGBBTJ_BMPInfoHeader;

typedef struct
{
unsigned short wFormatTag;
unsigned short nChannels;
unsigned int nSamplesPerSec;
unsigned int nAvgBytesPerSec;
unsigned short nBlockAlign;
unsigned short wBitsPerSample;
unsigned short cbSize;
}BGBBTJ_WaveFmt;
#endif


typedef struct {
TK_FILE *fd;

int offs_avih;

int offs_vidh;
int offs_vidf;
int offs_vidStrd;

int offs_audh;
int offs_audf;
int offs_audStrd;

int osz_avi;
int osz_hdrl;
int osz_strl;
int osz_movi;

BGBBTJ_MainAVIHeader *avih;
BGBBTJ_AVIStreamHeader *vidh;
BGBBTJ_AVIStreamHeader *audh;
BGBBTJ_BITMAPINFOHEADER *vidf;
BGBBTJ_WAVEFORMATEX *audf;

byte *vidStrd;
byte *audStrd;
int sz_vidStrd;
int sz_audStrd;

u32 *index;
int n_index, m_index;
float fps;
int xs, ys;
int frnum;

volatile float accDt;
volatile float accDt2;

void *vidCtx;
void *audCtx;

void *vid_codec_ctx;
void *aud_codec_ctx;

byte *capFrameBuf;
byte *capFrameBuf2;
volatile int capFrameIdx;
volatile int capFrameLastIdx;
volatile int capReqClose;

byte *capEncodeBuf;
byte *capEncodeAuBuf;

short *audSampleBuf;
volatile int audSampleBufSz;
volatile int audSampleBufEndPos;
volatile int audSampleBufStartPos;

}BGBBTJ_AVICtx;

