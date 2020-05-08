all: start
	
start: main.o
	g++ main.o -o start
	./start

main.o: ./Source/main.cpp
	g++ -c ./Source/main.cpp

clean:
	rm -rf *.o start
