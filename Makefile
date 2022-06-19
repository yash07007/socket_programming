all:
	g++ -o servermain servermain.cpp
	g++ -o client client.cpp

clean:
	rm -f servermain
	rm -f client
	rm -f ee*

build: clean
	tar cvf ee450_ysolanki.tar *.cpp Make* README*
	gzip ee450_ysolanki.tar