
This is gorilla paper encoding.

It is a  compression of float64.

Rewrite to C language from influxdb's go language.


TEST:
root@ubuntu:~/github/gorilla-paper-encode# make
gcc -g gorilla.c test.c -o test
root@ubuntu:~/github/gorilla-paper-encode# ./test
Encoded Bytes: 
10 40 a1 f8 00 00 00 00   00 dc 3e 79 4e d2 3e e2 
98 7e 69 8e f1 7d fa fb   80 00 00 00 00 08 00 
Decoded Float: 
2300.00
2400.00
2500.00
2600.00
2700.00
2800.00
2900.00
3000.00
