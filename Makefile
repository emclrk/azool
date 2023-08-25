azool:
	mkdir -p bin
	g++ src/*.cc -I./include -o bin/azool -Werror -Weffc++ -std=c++11
