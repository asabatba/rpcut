

call windres ico.rc -O coff -o ico.res || goto err

call gcc -o xp_inv.o -c xp_inv.c || goto err

call g++ -o xp_inv.exe xp_inv.o ico.res || goto err

del xpc_inv.exe || goto err

call upx xp_inv.exe -9 -o xpc_inv.exe || goto err

echo Ok!
exit /b 0

:err

echo Error!
exit /b 1