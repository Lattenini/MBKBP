all: MBKBPZigzag

MBKBPZigzag: .obj/main.o 
	clang++ -g .obj/main.o -o MBKBPZigzag

.obj/main.o: main.cpp
	clang++ -g -c -O3 -std=c++20 -I ./include -o .obj/main.o main.cpp


clean:
	rm -rf *o .obj
	mkdir .obj