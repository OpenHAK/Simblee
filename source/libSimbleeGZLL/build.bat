@echo off

set arduino=c:\arduino-1.6.5-r2
set nordic=c:\nRF51_SDK_8.0.0_5fc2c3a

if not exist %nordic% (
echo Error: The Nordic SDK is required to build libSimbleeGZLL.
goto end
)

if not exist _build\nul mkdir _build
if exist _build\* del /q _build\*

set tools=%arduino%\portable\packages\Simblee\tools\arm-none-eabi-gcc\4.8.3-2014q1
set basedir=%arduino%\portable\packages\Simblee\hardware\Simblee

dir /b %basedir% >%temp%\ver.txt
set /p ver= <%temp%\ver.txt
set basedir=%basedir%\%ver%

set gcc=%tools%\bin\arm-none-eabi-gcc
set ar=%tools%\bin\arm-none-eabi-ar

set includes=-I%nordic%\components\device
set includes=%includes% -I%nordic%\components\properitary_rf\gzll
set includes=%includes% -I%nordic%\components\toolchain
set includes=%includes% -I%nordic%\components\toolchain\gcc

set cflags=-Os -w -mcpu=cortex-m0 -mthumb -DNRF51 -DBOARD_PCA10001 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fno-builtin %includes%

echo compiling...

echo libSimbleeGZLL.c
%gcc% %cflags% -c -o _build\libSimbleeGZLL.o libSimbleeGZLL.c

echo creating library...

set objs=_build\libSimbleeGZLL.o

cd _build
%ar% -x %nordic%\components\properitary_rf\gzll\gcc\gzll_gcc.a
cd ..
set objs=%objs% _build\nrf_assert_internal.o
set objs=%objs% _build\nrf_channel_manager.o
set objs=%objs% _build\nrf_fifo.o
set objs=%objs% _build\nrf_flywheel.o
set objs=%objs% _build\nrf_gzll.o
set objs=%objs% _build\nrf_impala.o
set objs=%objs% _build\nrf_link_core.o
set objs=%objs% _build\nrf_notification_manager.o
set objs=%objs% _build\nrf_packet_manager.o
set objs=%objs% _build\nrf_state_machine.o

for %%f in (%objs%) do %ar% rcs _build/libSimbleeGZLL.a %%f

echo copying libray and header to variants...
copy _build\libSimbleeGZLL.a %basedir%\variants\Simblee\libSimbleeGZLL.a
copy libSimbleeGZLL.h %basedir%\variants\Simblee\libSimbleeGZLL.h

:end
pause
