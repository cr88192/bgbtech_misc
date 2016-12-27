cl /Febtic4b_t0.exe llqtst0.c /Zi /arch:SSE2 /O2
rem cl /Febtic4b_t0.exe llqtst0.c /Zi /arch:AVX /O2
rem cl /Febtic4b_t0.exe llqtst0.c /Zi /arch:SSE2
cl /Febtic4b.dll bt4b_multi.c /LD /Zi /DBTIC4B_DLL /arch:SSE2 /O2
