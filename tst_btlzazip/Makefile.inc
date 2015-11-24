SRCS = \
	btlza_fastdfl.c		\
	btlza_bitdecode.c	\
	btlza_bitencode.c	\
	btlza_bitrange.c	\
	btlza_bitstream.c	\
	btlza_decode.c		\
	btlza_encode.c		\
	btlza_main.c

HDRS = btlzazip_auto.h

all: btlzazip$(EXESUF)

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

btlzazip_auto.h: autohead$(BINSUF) $(SRCS)
	./autohead$(BINSUF) -api BGBBTJ_API $(SRCS) > btlzazip_auto.h

btlzazip$(EXESUF): $(HDRS) $(SRCS)
	$(CC_OUTEXE)btlzazip$(EXESUF) $(SRCS) $(CFLAGS)