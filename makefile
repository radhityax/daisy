cc= cc
src= main.c
out= daisy

install:
	$(cc) $(src) -o $(out)

clean:
	rm -rf source
	rm -rf target
	rm $(out)
