//EE 450 Project
//Developed by Victor Fu
//Date Due: 11/17/16

//Description
//This is the Client portion of the project.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

#define AWS_TCP_PORT "25991"
#define MAXDATASIZE 100

int main(int argc, char *argv[])
{
	if (argc != 2) {
        cout<< "Invalid number of arguments"<<endl;
        return -1;
    }

    //The following is taken from Beej's guide with 
    //adjustments made to fit the project requirements

    //establish a TCP Socket

    //establish variables
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    //establish struct components
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP


    if ((rv = getaddrinfo(NULL, AWS_TCP_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    cout <<"The client is up and running."<<endl;

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    //end of Beej code

    //send the type of reduction
    numbytes = send(sockfd, argv[1],3,0);
    if(numbytes == -1){
    	perror("send");
    	exit(1);
	}
    usleep(100);
	cout << "The client has sent the reduction type "<<argv[1]<<" to AWS."<<endl;

	// Read contets of CSV file and store in a vector
	ifstream csv_nums("nums.csv");
	vector<string> num_list;
	while(!csv_nums.eof())
	{
		string nums;
		getline(csv_nums,nums);
        
        
		if(csv_nums.eof())
			break;
		num_list.push_back(nums);

	}

	//send numbers to AWS
	int j=0;
	while (j<num_list.size()){
		if((numbytes = send(sockfd,num_list[j].c_str(),num_list[j].length(),0)) == -1)
        {
            perror("send");
            exit(1);
        }
        j++;
        usleep(100);
	}
   

    
	//Tell the AWS that all numbers from the CSV have been sent
	//complete flag
	if((numbytes = send(sockfd,"complete",8,0)) == -1){
        perror("send");
        exit(1);
    }

    //tell the user how many numbers have been sent
    cout <<"The client has sent "<<num_list.size()<<" numbers to AWS."<<endl;


    //Accept the final reduction value from AWS
    //the following is copied directly from Beej
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    cout<<"The client has received reduction "<<argv[1]<<": "<<buf<<endl;

    close(sockfd);

    return 0;
}
