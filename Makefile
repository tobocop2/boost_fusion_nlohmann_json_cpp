.PHONY: all clean

example_main: example.cpp
	$(CXX) $^ -I mpl/include/ -I lexical_Cast/include -I fusion/include -I json/include/ -std=c++14 -o $@

all: example_main

clean:
	rm -rf example_main
