# bgbtech_misc
BGB related misc stuff.

tst_btic1h: BTIC1H VfW codec (Test).
This was an initial test of the design for BTIC1H.
This implementation does frame-at-a-time encoding, and does not support incremental encoding.
A version used for streaming video from a robot would likely be a little more minimalist and
support incremental encoding. This is a "too be done" thing.


tst_fastbcn: Test for a "reasonably fast" BC7 encoder.
It is single-threaded plain C.
Tests have it getting around 50-60 megapixels/second from RGBA on a 3.4GHz AMD K10 (Phenom II).

Possible: throw on code for JPEG decoding, possibly with an Alpha extension.
Alpha can be done via shoving another JPEG inside the first JPEG via APPn markers
(representing a monochrome alpha channel).
This is reasonably transparent, but non-aware programs can't use the alpha channel.

Also possible: shim transcode from BTIC1H metablock format to BC7.
JPEG would be slower, but compresses better.  BTIC1H could be faster.
Would either way likely need to use a planar YUV format or similar as an intermediate format for sake of mipmaps.
