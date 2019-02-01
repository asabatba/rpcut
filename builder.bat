

call windres ico.rc -O coff -o ico.res || goto err

call gcc -o xp.o -c xp.c || goto err

call g++ -o xp.exe xp.o ico.res || goto err

del xpc.exe || goto err

call upx xp.exe -9 -o xpc.exe || goto err

echo Ok!
exit /b 0

:err

echo Error!
exit /b 1