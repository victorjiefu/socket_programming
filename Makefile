all: 
	g++ -g -lsocket -lnsl -lresolv aws.cpp -o awsout
	g++ -g -lsocket -lnsl -lresolv serverA.cpp -o serverAout
	g++ -g -lsocket -lnsl -lresolv serverB.cpp -o serverBout
	g++ -g -lsocket -lnsl -lresolv serverC.cpp -o serverCout
	g++ -g -lsocket -lnsl -lresolv client.cpp -o client


serverA:
	./serverAout


serverB:
	./serverBout


serverC:
	./serverCout


aws:
	./awsout

