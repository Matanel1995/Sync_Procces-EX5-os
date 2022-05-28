CC = clang++-9
o = -o
pt = -pthread

#Make function for threads code
thread: t_server t_client

t_server: server.cpp Stack.cpp Node.hpp
	$(CC) $(pt) server.cpp $(o) th_server

t_client: client.cpp Stack.cpp Node.hpp
	$(CC) $(pt) client.cpp $(o) th_client

clean:
	rm -f *.o th_server th_client