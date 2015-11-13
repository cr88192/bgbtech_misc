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

ccpp-ssao
* o: RGB/YUYV vs BGR/UYVY
* * 0=RGB/YUYV
* * 1=BGR/UYVY
* a: Alpha (If pattern can have alpha)
* * 0=Alpha
* * 1=No Alpha
* ss: Stride, 4-1
* pp: Pixel Bit Type
* * 0=Unsigned Byte
* * 1=Half-Float
* * 2=Reserved
* * 3=Float
* cc: Colorspace Type
* * 0=RGBA
* * 1=YUV, Interleaved

Assigned Mode Numbers:
* 00: RGBA
* 01: BGRA
* 02: RGBx
* 03: BGRx
* 04: RGB
* 05: BGR
* 06: RGB (Resv)
* 07: BGR (Resv)
* 08: RG
* 0C: R
* 10: RGBA (H-Float)
* 11: BGRA (H-Float)
* ...
* 30: RGBA (Float)
* 31: BGRA (Float)
* ...
* 40: YUVA (4:4:4:4)
* 42: YUVx (4:4:4)
* 44: YUV (4:4:4)
* 48: YUYV (4:2:2)
* 49: UYVY (4:2:2)
* 4C: Y
* 4E: Planar YUVA (4:2:0:4)
* 4F: Planar AYUV (4:4:2:0)
* ...

