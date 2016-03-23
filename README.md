# bgbtech_misc
BGB related misc stuff.

tst_btic1h
==========

tst_btic1h: BTIC1H VfW codec (Test).
This was an initial test of the design for BTIC1H.
This implementation does frame-at-a-time encoding, and does not support incremental encoding.
A version used for streaming video from a robot would likely be a little more minimalist and
support incremental encoding. This is a "too be done" thing.

This supports being compiled as a VfW codec driver. This allows its use from other Windows applications while embedded in an AVI. AVI and a modified BMP will be the canonical formats for on-disk storage of BTIC1H video or images.

BTIC1H is a modestly fast Blocky VQ codec using Adaptive Rice coding for things like color deltas and commands.

There are both faster blocky VQ codecs, as well as codecs which offer a better quality/bpp, but it works ok.


tst_fastbcn
===========

tst_fastbcn: Test for a "reasonably fast" BC7 encoder.
It is single-threaded plain C.
Tests have it getting around 50-60 megapixels/second from RGBA on a 3.4GHz AMD K10 (Phenom II).

It has a JPEG decoder with support for an Alpha Channel extension.
Alpha can be done via shoving another JPEG inside the first JPEG via APPn markers
(representing a monochrome alpha channel).
This is reasonably transparent, but non-aware programs can't use the alpha channel.
Normal JPEG images can be decoded, and will be treated as opaque.
Note: Current decoder is only good for 4:2:0 images which are multiples of 16 pixels.

Also possible: shim transcode from BTIC1H metablock format to BC7.
JPEG is slower, but compresses better.  BTIC1H could be faster.
Would either way likely need to use a planar YUV format or similar as an intermediate format for sake of mipmaps.

As-is: On the test PC, the JPEG decoder by itself pulls off about 50 Mpix/sec, and can be converted to BC7 at 30-35 Mpix/sec. It is around 25-30 if producing BC7 output with mipmaps (the actual Mpix/sec is the same, however throughput is slightly lower due to needing to process more pixel data).


BCn Modes
---------

BCn Modes
* 0, Unknown/Invalid
* 1, BC1/DXT1, 64-bit with optional 1-bit alpha
* 2, BC2/DXT3, 128-bit, with 64-bit color and 4-bpp alpha.
* 3, BC3/DXT5, 128-bit, with 64-bit color and 8-bpp color.
* 4, BC4, 64-bit Y image.
* 5, BC5, 128-nit Y/A or R/G image.
* 6, BC6H, 128-bit HDR.
* 7, BC7, 128-bit LDR color+alpha.


Pixel Format
------------

ppss-yqao
* o: RGB/YUYV vs BGR/UYVY
** 0=RGB/YUYV
** 1=BGR/UYVY
* a: Alpha (If pattern can have alpha)
** 0=Alpha
** 1=No Alpha
* q: Don't use partitions (Resv)
* y: Use YUV.
* rr: Reserved, MBZ
* ss: Stride=4-ss; (0->4, 3->1)
* pp: Pixel Bit Type
** 0=Unsigned Byte
** 1=Reserved
** 2=Half-Float
** 3=Float


tst_btlzazip
============

Misc standalone compressor for a format known as BTLZA or BTLZH. This is an extended form of Deflate which has a bigger window and matches and can give better compression for some types of data.

It exists as a compromise between Deflate and LZMA, capable of giving better compression than Deflate while faster to decode than LZMA.

It has an optional arithmetic mode which may (sometimes) result in better compression (but does generally make it slower, so isn't generally used).

The tool basically makes it standalone and hacks on a basic GZIP-like front-end.
