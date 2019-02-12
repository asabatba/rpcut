
both:
	gcc -Wall -o xp xp.c args.c str_misc.c -I.
	gcc -Wall -o xp_inv xp_inv.c
	
	
	del xpc.exe
	upx xp.exe -9 -o xpc.exe
	
	del xpc_inv.exe
	upx xp_inv.exe -9 -o xpc_inv.exe