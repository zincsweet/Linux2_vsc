.PHONY: all
all: loop mykill

loop: loop.cpp
	g++ -o $@ $^
mykill: mykill.cpp
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f mykill loop