@REM set cc65path="C:\Retro\DEV\cc65-snapshot-win32"
@REM set mypath="C:\Users\Brizio\Documents\GitHub\PortableChase\minimal"
@REM set configpath="C:\Users\Brizio\Documents\GitHub\PortableChase\minimal\experiments"

@REM 
set cc65path="D:\personal\cc65-snapshot-win32"
@REM 
set configpath="D:\Userfiles\fcaruso\Documents\GitHub\ASCII-CHASE\minimal\experiments"
@REM 
set mypath="d:\Userfiles\fcaruso\Documents\GitHub\ASCII-CHASE\minimal"


set Deliverables=%mypath%\deliverables

mkdir %deliverables% 2> NUL

del %deliverables%\*
del %mypath%\atmos\*.o
del %mypath%\c64\*.o
del %mypath%\patch\*.o

cd %cc65path%\bin\



@REM cl65.exe -O -t c16 %mypath%\display_macros.c %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\c16-32k.prg

@REM Atari 5200: Out of memory (fixable) - CONIO and joystick issues
@REM cl65.exe -O -t atari5200 -lib %cc65path%\lib\atari5200.lib %mypath%\display_macros.c %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\atari5200.a52

@REM Atari 2600: Out of memory  CONIO and joystick issues?
@REM cl65.exe -O -t atari2600 -lib %cc65path%\lib\atari2600.lib %mypath%\display_macros.c %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\atari2600.a26

@REM vic20-16k has reached its memory limitations
@REM 
cl65.exe -O -t vic20 --config %mypath%\..\cfg\vic20-16k.cfg %mypath%\display_macros.c %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\cvic20-16k.prg

@REM pce runs out of memory
@REM cl65.exe -O -t pce %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\pcengine.bin


@REM BBC fails looking for bbc.lib
@REM cl65.exe -O -t bbc  %mypath%\display_macros.c %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\bbc.bin




@REM cl65.exe -O -t lynx  %mypath%\display_macros.c %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\lynx.bin


@REM cl65.exe -O -t supervision  %mypath%\display_macros.c %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\supervision.bin


@REM cl65.exe -O -t apple2enh  %mypath%\display_macros.c %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\apple2enh.bin

@REM BBC fails looking for bbc.lib
@REM cl65.exe -O -t atarixl  %mypath%\display_macros.c %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\atarixl.bin

@REM BBC fails looking for bbc.lib
@REM cl65.exe -O -t cbm610  %mypath%\display_macros.c %mypath%\powerUps.c %mypath%\enemy.c %mypath%\invincible_enemy.c %mypath%\level.c %mypath%\character.c %mypath%\text.c %mypath%\missile.c %mypath%\strategy.c %mypath%\input.c %mypath%\main.c  -o %deliverables%\cbm610.bin

