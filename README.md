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
