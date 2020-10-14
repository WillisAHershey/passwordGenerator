password: password.c
	gcc -o password -Wall -O3 password.c
.PHONY: clean

clean:
	rm password
