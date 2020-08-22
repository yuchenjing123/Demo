.PHONY:clean
out:main.o Cfile.o mmanager.o thread_pool.o
	g++ -Wall -g main.o Cfile.o mmanager.o thread_pool.o -lpthread -o out
main.o:main.cpp
	g++ -Wall -g -c main.cpp -o main.o
Cfile.o:Cfile.cpp Cfile.h
	g++ -Wall -g -c Cfile.cpp -o Cfile.o
mmanager.o:mmanager.cpp mmanager.h
	g++ -Wall -g -c mmanager.cpp -o mmanager.o
thread_pool.o:thread_pool.cpp thread_pool.h
	g++ -Wall -g -c thread_pool.cpp -o thread_pool.o

clean:
	rm -rf *.o out
