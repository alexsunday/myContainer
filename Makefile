all:
	gcc -fpic -shared -o clone.so wrap.c -I /usr/include/python3.4
