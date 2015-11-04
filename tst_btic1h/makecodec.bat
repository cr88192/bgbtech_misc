cl /Fetst_bt1h.exe bt1h_main.c /Zi /O2
REM cl /Fetst_bt1h.exe bt1h_main.c /Zi

cl /Febt1hvfw.dll bt1h_vfwdrv.c /Zi /O2 /LD winmm.lib user32.lib driverproc.def
