Infection: main.o
	g++ main.o -g -o Infection -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system

main.o: main.cpp
	g++ -g -c main.cpp

clean:
	-rm *.o $(objects) Infection
