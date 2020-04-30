all: show

show: run
	pwd > path
	./start
	./showGraph

run: Source/main.cpp
	g++ ./Source/main.cpp -o start

clean:
	rm -rf start showGraph path
