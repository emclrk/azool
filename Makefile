azool:
	mkdir -p bin
	g++ src/*.cc -I./include -o bin/azool -Werror -Weffc++ -std=c++11 -DBOOST_ALLOW_DEPRECATED_HEADERS -DBOOST_BIND_GLOBAL_PLACEHOLDERS

clean:
	rm bin/*
