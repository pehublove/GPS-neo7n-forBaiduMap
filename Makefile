test:test.o gpsBaidu.o
	g++ test.o gpsBaidu.o -o test

test.o:
	g++ -c test.cpp

gpsBaidu.o:gpsBaidu.h
	g++ -c gpsBaidu.cpp

clean:
	rm *.o test
