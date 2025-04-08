@echo off
for /l %%x in (3, 1, 100) do (
   echo %%x
   ..\src\final.exe ..\tests\primality.bin %%x.txt 11 %%x
)