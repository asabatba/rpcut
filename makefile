
both:
#	gcc -Wall -g -pg -o xp xp.c args.c str_misc.c speedtest.c -I.
	gcc -Wall -O3 -o xp xp.c args.c str_misc.c speedtest.c -I.
#	gcc -Wall -O0 -g -o xp xp.c args.c str_misc.c speedtest.c -I.

	gcc -Wall -O3 -o xp_inv xp_inv.c
	
	
	del xpc.exe
	upx xp.exe -9 -o xpc.exe
	
	del xpc_inv.exe
	upx xp_inv.exe -9 -o xpc_inv.exe