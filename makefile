cc= cc
cflags = -Wall -Wextra -std=c99 -pedantic
src= main.c
out= daisy

install:
	$(cc) $(cflags) $(src) -o $(out)

clean:
	rm -rf source
	rm -rf target
	rm $(out)
