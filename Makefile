main: main.o CTRNN.o TSearch.o LeggedAgent.o random.o
	g++ -std=c++11 -o main main.o CTRNN.o TSearch.o LeggedAgent.o random.o
random.o: random.cpp random.h VectorMatrix.h
	g++ -std=c++11 -c -O3 random.cpp
CTRNN.o: CTRNN.cpp random.h CTRNN.h
	g++ -std=c++11 -c -O3 CTRNN.cpp
TSearch.o: TSearch.cpp TSearch.h
	g++ -std=c++11 -c -O3 TSearch.cpp
LeggedAgent.o: LeggedAgent.cpp LeggedAgent.h TSearch.h CTRNN.h random.h VectorMatrix.h
	g++ -std=c++11 -c -O3 LeggedAgent.cpp
main.o: main.cpp CTRNN.h LeggedAgent.h TSearch.h
	g++ -std=c++11 -c -O3 main.cpp
clean:
	rm *.o main
