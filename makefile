cc= cc
cflags = -Wall -Wextra -std=c99 -pedantic
src= main.c
out= daisy

ex=doc

install:
	$(cc) $(cflags) $(src) -o $(out)

clean:
	rm -rf source
	rm -rf target
	rm -rf media
	rm $(out)

test: install
	./$(out) new
	cp $(ex) ./source/
	./$(out) build
