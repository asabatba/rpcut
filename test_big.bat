


REM call gcc xp.c -o xp.exe || goto err
call windres ico.rc -O coff -o ico.res
call gcc -o xp xp.o ico.res || goto err

call g++ -o xp xp.o ico.res


REM set /p apps="Apps: "

call xp.exe -i %~f1 -o %~n1_o.xml -a %~2 || goto err

call biserverxmlexec -P Admin123 -I %~n1_o.xml -O .\%~n1_o.rpd || goto err

REM call validaterpd -P Admin123 -I %~n1_o.xml -8 -o %~n1_o_val.txt || goto err

exit /b 0

:err

echo Error!
exit /b 1

