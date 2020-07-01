all: build

build: quadtree.c
	gcc quadtree.c -o quadtree

clean:
	rm quadtree
