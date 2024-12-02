all: MBKBPZigzag

MBKBPZigzag: .obj/main.o 
	g++ -g .obj/main.o -o MBKBPZigzag

.obj/main.o: main.cpp
	g++ -g -c -O3 -std=c++20 -I ./include -o .obj/main.o main.cpp


clean:
	rm -r .obj
	mkdir .obj