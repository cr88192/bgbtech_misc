#ifndef BTLZAZIP_H
#define BTLZAZIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <btlzazip_conf.h>

#define BGBBTJ_ZFL_ESCAPE_FF	1
#define BGBBTJ_ZFL_DEFLATE64	2
#define BGBBTJ_ZFL_ESCBTLZA		4
#define BGBBTJ_ZFL_ESCBTLZH		8
#define BGBBTJ_ZFL_ISBTLZH		16
#define BGBBTJ_ZFL_ARITH		32
#define BGBBTJ_ZFL_FASTENC		64
#define BGBBTJ_ZFL_RINGHUFF		128
#define BGBBTJ_ZFL_PRELOAD		256

#ifdef _MSC_VER
#define force_inline __forceinline
#else
#define force_inline
#endif

#include <btlzazip_btlza.h>
#include <btlzazip_auto.h>

#endif
