// EE 450 Project
//Developed by Victor Fu
//Date Due: 11/17/16
//Version 1.0

//Description
//This is the AWS portion of the project. 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

#define Server_A_UDP_PORT "21991"
#define Server_B_UDP_PORT "22991"
#define Server_C_UDP_PORT "23991"
#define AWS_UDP_PORT "24991"
#define AWS_TCP_PORT "25991"

#define BACKLOG 10
#define MAXDATASIZE 100

int main(void)
{
	

	//establishment of TCP socket
	// This is largely copied from Beej's guide with modification to fit the project 

    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv, numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, AWS_TCP_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    /*
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
	*/
    char buf[MAXDATASIZE];
    cout<<"The AWS is up and running."<<endl;

    //This is also a portion of Beej's code by is modified for the project
    while(1) {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        
        /*
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        */
        
        
        //this is still influenced by Beej, but modified
        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            
            
            //this portion is modified to recieved the reduction type
            if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) 
            {
                perror("recv");
                exit(1);
            }
            buf[numbytes]='\0';
            string type(buf);
            
            vector<string> numbers;
            int count=0;
            
            //This receives and stores all the numbers from the client into a vector
            while (strcmp(buf,"complete")!=0) 
            {
                if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) { //recieve 
                    perror("recv");
                    exit(1);
                }
                
                buf[numbytes]='\0'; //place number in buffer

                if(!(strcmp(buf,"complete")==0)) //check if numbers have endedd
                {
                    numbers.push_back(buf);
                    count++;
                }
            }

            cout<< "The AWS has received " << numbers.size()<< " numbers from the client using TCP over port " << AWS_TCP_PORT << endl;




    //The next portion established 3 UDP sockets for the A,B,C servers
    //using a slightly modified version of Beej's code
    //The 3 servers are established identically, so the code are the same with different names

            //Server A
            int sockfd_A;
            struct addrinfo hints_A, *servinfo_A, *p_A;
            int rv_A;
            memset(&hints_A, 0, sizeof hints_A);
            hints_A.ai_family = AF_UNSPEC;
            hints_A.ai_socktype = SOCK_DGRAM;

            if ((rv_A = getaddrinfo("127.0.0.1", Server_A_UDP_PORT, &hints_A, &servinfo_A)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_A));
                return 1;
            }
            // loop through all the results and make a socket
            for(p_A = servinfo_A; p_A != NULL; p_A = p_A->ai_next) {
                if ((sockfd_A = socket(p_A->ai_family, p_A->ai_socktype,
                                     p_A->ai_protocol)) == -1) {
                    perror("talker: socket");
                    continue;
                }
                break;
            }
            if (p_A == NULL) {
                fprintf(stderr, "talker: failed to create socket\n");
                return 2;
            }

            //Server B

            int sockfd_B;
            struct addrinfo hints_B, *servinfo_B, *p_B;
            int rv_B;
            memset(&hints_B, 0, sizeof hints_B);
            hints_B.ai_family = AF_UNSPEC;
            hints_B.ai_socktype = SOCK_DGRAM;

            if ((rv_B = getaddrinfo("127.0.0.1", Server_B_UDP_PORT, &hints_B, &servinfo_B)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_B));
                return 1;
            }
            // loop through all the results and make a socket
            for(p_B = servinfo_B; p_B != NULL; p_B = p_B->ai_next) {
                if ((sockfd_B = socket(p_B->ai_family, p_B->ai_socktype,
                                      p_B->ai_protocol)) == -1) {
                    perror("talker: socket");
                    continue;
                }
                break;
            }
            if (p_B == NULL) {
                fprintf(stderr, "talker: failed to create socket\n");
                return 2;
            }

            //Server C 
			int sockfd_C;
            struct addrinfo hints_C, *servinfo_C, *p_C;
            int rv_C;
            memset(&hints_C, 0, sizeof hints_C);
            hints_C.ai_family = AF_UNSPEC;
            hints_C.ai_socktype = SOCK_DGRAM;

            if ((rv_C = getaddrinfo("127.0.0.1", Server_C_UDP_PORT, &hints_C, &servinfo_C)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_C));
                return 1;
            }
            // loop through all the results and make a socket
            for(p_C = servinfo_C; p_C != NULL; p_C = p_C->ai_next) {
                if ((sockfd_C = socket(p_C->ai_family, p_C->ai_socktype,
                                      p_C->ai_protocol)) == -1) {
                    perror("talker: socket");
                    continue;
                }
                break;
            }
            if (p_C == NULL) {
                fprintf(stderr, "talker: failed to create socket\n");
                return 2;
            }

            //send the type of reduction to backend servers A,B,C
            //This is a modified version of Beej's sendto code
            //store the reduction in a string
            
 			if ((numbytes = sendto(sockfd_A, type.c_str(), 3, 0,
                                   p_A->ai_addr, p_A->ai_addrlen)) == -1) {
                perror("talker: sendto");
                exit(1);
            }
            if ((numbytes = sendto(sockfd_B, type.c_str(), 3, 0,
                                   p_B->ai_addr, p_B->ai_addrlen)) == -1) {
                perror("talker: sendto");
                exit(1);
            }
            if ((numbytes = sendto(sockfd_C, type.c_str(), 3, 0,
                                   p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                perror("talker: sendto");
                exit(1);
            }

// This next portion sends 1/3 of the numbers to backend A,B, C
//Code structure was influenced by Beej's code

            //Server A
            for(int i=0; i< (numbers.size()/3); i++) //recieve 0 to 1/3
            {
                if ((numbytes = sendto(sockfd_A, numbers[i].c_str(), numbers[i].length(), 0,
                                       p_A->ai_addr, p_A->ai_addrlen)) == -1) {
                    perror("talker: sendto");
                    exit(1);
                }
                usleep(100);
            
            }


            //Server B
            for(int i=(numbers.size()/3); i< (numbers.size()*2)/3; i++) // recieve 1/3 to 2/3
            {

                if ((numbytes = sendto(sockfd_B, numbers[i].c_str(), numbers[i].length(), 0,
                                       p_B->ai_addr, p_B->ai_addrlen)) == -1) {
                    perror("talker: sendto");
                    exit(1);
                }
              usleep(100);
            }


            //Server C
            for(int i=(numbers.size()*2/3); i<numbers.size(); i++) // receive 2/3 to total
            {
                if ((numbytes = sendto(sockfd_C, numbers[i].c_str(), numbers[i].length(), 0,
                                       p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                    perror("talker: sendto");
                    exit(1);
                }
                usleep(100);
            }


// This next portion sends comfirmations to backend Server A,B,C
//that all the numbers have been sent
//Code structure was influenced by Beej's code        

            //Server A
             if ((numbytes = sendto(sockfd_A, "complete", 8, 0,
                                   p_A->ai_addr, p_A->ai_addrlen)) == -1) 
            {
                perror("talker: sendto");
                exit(1);
            }
            cout<<"The AWS sent "<<numbers.size()/3<<" numbers to Backend Server A."<<endl;

            //Server B
            if ((numbytes = sendto(sockfd_B, "complete", 8, 0,
                                   p_B->ai_addr, p_B->ai_addrlen)) == -1) 
            {
                perror("talker: sendto");
                exit(1);
            }
            cout<<"The AWS sent "<<numbers.size()/3<<" numbers to Backend Server B."<<endl;

            //Server C
            if ((numbytes = sendto(sockfd_C, "complete", 8, 0,
                                   p_C->ai_addr, p_C->ai_addrlen)) == -1) 
            {
                perror("talker: sendto");
                exit(1);
            }
            cout<<"The AWS sent "<<numbers.size()/3<<" numbers to Backend Server C."<<endl;


//The AWS needs a UDP socket to accept incoming data from backend servers A,B,C
//This is Beej's code with slight variations

            int sockfd_UDP;
            struct addrinfo hints_UDP, *servinfo_UDP, *p_UDP;
            int rv_UDP;
            struct sockaddr_storage their_addr_UDP;

            char backend_answers[MAXDATASIZE];

            socklen_t addr_len_UDP;
            char s_UDP[INET6_ADDRSTRLEN];
            memset(&hints_UDP, 0, sizeof hints_UDP);
            hints_UDP.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
            hints_UDP.ai_socktype = SOCK_DGRAM;
            if ((rv_UDP = getaddrinfo("127.0.0.1", AWS_UDP_PORT, &hints_UDP, &servinfo_UDP)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_UDP));
                return 1;
            }
            // loop through all the results and bind to the first we can
            for(p_UDP = servinfo_UDP; p_UDP != NULL; p_UDP = p_UDP->ai_next) {
                if ((sockfd_UDP = socket(p_UDP->ai_family, p_UDP->ai_socktype,
                                     p_UDP->ai_protocol)) == -1) {
                    perror("listener: socket");
                    continue;
                }
                if (bind(sockfd_UDP, p_UDP->ai_addr, p_UDP->ai_addrlen) == -1) {
                    close(sockfd_UDP);
                    perror("listener: bind");
                    continue;
                }
                break;
            }
            if (p_UDP == NULL) {
                fprintf(stderr, "listener: failed to bind socket\n");
                return 2;
            }

// AWS communicated to the backend servers A,B,C 
// giving them the signal that it is ready to recieve the processed numbers

            //Server A
            if ((numbytes = sendto(sockfd_A, "ready", 5, 0,
                                   p_A->ai_addr, p_A->ai_addrlen)) == -1) {
                perror("talker: sendto");
                exit(1);
            }

            //Server B
            if ((numbytes = sendto(sockfd_B, "ready", 5, 0,
                                   p_B->ai_addr, p_B->ai_addrlen)) == -1) {
                perror("talker: sendto");
                exit(1);
            }

            //Server C
             if ((numbytes = sendto(sockfd_C, "ready", 5, 0,
                                   p_C->ai_addr, p_C->ai_addrlen)) == -1) {
                perror("talker: sendto");
                exit(1);
            }
//AWS now recieves the incoming data from the backend servers
            int loop = 0;
            vector <string> backend_solution(1000000);
            while(loop<3) {
                if ((numbytes = recvfrom(sockfd_UDP, buf, MAXDATASIZE - 1, 0,
                                         (struct sockaddr *) &their_addr_UDP, &addr_len_UDP)) == -1) {
                    perror("recvfrom");
                    exit(1);
                }
                if(numbytes<=0)
                {
                    continue;
                }
                buf[numbytes] = '\0';
   
                //the result of the calculations and the name of the server come back in one char array package.
                //the following code divides it.
                string result(buf);
                backend_solution[loop] = result.substr(1);
                char server = buf[0];
                cout << "The AWS received reduction result of " << type << " from Backend Server "
                << server << " using UDP over port " << AWS_UDP_PORT << " and it is " << backend_solution[loop] << endl;
                
                loop++;
            }


//AWS calculates the final result

            stringstream solution;
            if(type == "min")
            {
                int tempmin=1000;
                for(int i=0; i<3;i++)
                {
                    if(atoi(backend_solution[i].c_str())<tempmin)
                    {
                        tempmin = atoi(backend_solution[i].c_str());
                    }
                }
                solution<<tempmin;
                cout<<"The AWS has successfully finished the reduction "<<type<<": "<<tempmin<<endl;
            }
            else if(type == "max")
            {
                int tempmax=-1000;
                for(int i=0; i<3;i++)
                {
                    if(atoi(backend_solution[i].c_str())>tempmax)
                    {
                        tempmax = atoi(backend_solution[i].c_str());
                    }
                }
                solution<<tempmax;
                cout<<"The AWS has successfully finished the reduction "<<type<<": "<<tempmax<<endl;
                
            }
            else if(type == "sum")
            {
                long long sum(0);
                for(int i=0; i<3;i++)
                {
                    sum+= atoll(backend_solution[i].c_str());
                }
                solution<<sum;
                cout<<"The AWS has successfully finished the reduction "<<type<<": "<<sum<<endl;
                
            }
            else if(type == "sos")
            {
                long long sos(0);
                for(int i=0; i<3;i++)
                {
                    sos+= atoll(backend_solution[i].c_str());
                }
                solution<<sos;
                cout<<"The AWS has successfully finished the reduction "<<type<<": "<<sos<<endl;
               
            }



//AWS sends the final result to the client

				if((numbytes = send(new_fd,solution.str().c_str(),solution.str().length(),0)) == -1)
                {
                    perror("send");
                    exit(1);
                }

                 cout<<"The AWS has successfully finished sending the reduction value to client."<<endl;


//Close the sockets
			freeaddrinfo(servinfo_A);
			close(new_fd);
            close(sockfd_A);
            close(sockfd_B);
            close(sockfd_C);


            close(sockfd_UDP);
            exit(0);

            
        }
        close(new_fd);  // parent doesn't need this**
    }

    return 0;
}
