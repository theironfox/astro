main: main.o ipanel.o
	g++ -o main main.o ipanel.o `wx-config --cxxflags` `wx-config --libs` -fPIC -L. -lswe -lm -ldl -o ./go

main.o: main.cpp main.h
	g++ -c main.cpp `wx-config --cxxflags`

ipanel.o: ipanel.cpp ipanel.h
	g++ -c ipanel.cpp `wx-config --cxxflags`

clean:
	rm main.o ipanel.o
	clear
