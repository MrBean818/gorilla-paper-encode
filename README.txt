This is gorilla paper encoding.

It is a  compression of float64.

Rewrite to C language from influxdb's go language.

It is only encoding by now, decoding will be pushed later.

TEST:
[ckun@localhost gorilla]$ gcc -g test.c gorilla.c    
[ckun@localhost gorilla]$ 
[ckun@localhost gorilla]$ ./a.out 
10 40 a1 f8 00 00 00 00   00 dc 3e 79 4e d2 3e e2 
98 7e 69 c5 f7 eb d5 00   00 00 00 00 20 00 