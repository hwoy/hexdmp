cc = gcc
bin = hexdmp
CFLAGS =  -O2  -ansi -Wpedantic -Wall -Werror

.PHONY: all clean

all:	$(bin)

$(bin):	main.o opt.o function.o
	$(cc) -o $(bin) main.o opt.o function.o
main.o:
	$(cc) -c -o main.o $(CFLAGS) main.c
function.o:
	$(cc) -c -o function.o $(CFLAGS) function.c
opt.o:
	$(cc) -c -o opt.o $(CFLAGS) opt.c
clean:
	rm -rf *.o $(bin) *~

