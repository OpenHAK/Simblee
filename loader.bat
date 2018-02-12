@echo off

set loader=%1
set port=%2
set build_path=%3
set build_project_name=%4
set base=%5
set toolchain=%6

if 0 == 1 (
  %toolchain%/arm-none-eabi-objdump -h -S %build_path%/%build_project_name%.elf >%build_path%/%build_project_name%.s

  taskkill /fi "imagename eq explorer.exe" /fi "windowtitle eq build*" >null
  %windir%\explorer.exe "%build_path%"
)

%toolchain%/arm-none-eabi-nm %build_path%/%build_project_name%.elf >%build_path%/%build_project_name%.syms

set files=%build_path%/%build_project_name%.hex

findstr "ota_bootloader_single_bank" %build_path%\%build_project_name%.syms >nul
if %errorlevel% == 0 set files=%files% %base%/variants/Simblee/ota_bootloader_single_bank.hex

findstr "ota_bootloader_dual_bank" %build_path%\%build_project_name%.syms >nul
if %errorlevel% == 0 set files=%files% %base%/variants/Simblee/ota_bootloader_dual_bank.hex

%base%\%loader% -q %port% %files%
