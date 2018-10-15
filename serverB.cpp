// EE 450 Project
//Developed by Victor Fu
//Date Due: 11/17/16

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


#define Server_B_UDP_PORT "22991"
#define AWS_UDP_PORT "24991"

#define MAXDATASIZE 100

int main(void)
{
    char buf[MAXDATASIZE];
    //The following is taken from Beej's guide with
    //adjustments made to fit the project requirements
    
    //establish a UDP Socket
    
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(NULL, Server_B_UDP_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }
        
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }
    freeaddrinfo(servinfo);
    cout << "Server B is up and running using UDP on port " << Server_B_UDP_PORT<<"."<<endl;
    
    //create outgoing socket
    //edited version of Beej code
    
    int sockfd_out;
    struct addrinfo hints_out, *servinfo_out, *p_out;
    int rv_out;
    //int numbytes;
    
    memset(&hints_out, 0, sizeof hints_out);
    hints_out.ai_family = AF_UNSPEC;
    hints_out.ai_socktype = SOCK_DGRAM;
    
    if ((rv_out = getaddrinfo("127.0.0.1", AWS_UDP_PORT, &hints_out, &servinfo_out)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_out));
        return 1;
    }
    // loop through all the results and make a socket
    for(p_out = servinfo_out; p_out != NULL; p_out = p_out->ai_next) {
        if ((sockfd_out = socket(p_out->ai_family, p_out->ai_socktype,
                                 p_out->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }
    
    
    //end of Beej code
    
    // process numbers recieved from AWS
    
    
    
    //recieve the reduction type
    while(1){
        numbytes=0;
        while (numbytes <=0)
        {
            //copied from Beej
            if ((numbytes = recvfrom(sockfd, buf, MAXDATASIZE-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
                
                perror("recvfrom");
                exit(1);
            }
            buf[numbytes] = '\0';
        }
        
        buf[numbytes] = '\0';
        
        string type(buf);
        
        
        
        //recieve the numbers until confirmation of end
        int count=0;
        int sum=0;
        int sos=0;
        int min=1000;
        int max=-1000;
        
        while (strcmp(buf, "complete") != 0)
        {
            
            if ((numbytes = recvfrom(sockfd, buf, MAXDATASIZE- 1, 0,
                                     (struct sockaddr *) &their_addr, &addr_len)) == -1) {
                
                perror("recvfrom");
                exit(1);
            }
            buf[numbytes] = '\0';
            int value = atoi(buf);
            
            if (strcmp(buf, "complete") == 0)
            {
                continue;
            }
            
            //begin computation
            
            
            if (type == "sum"){
                
                sum=sum+value;
            }
            if (type == "sos"){
                sos=sos+value*value;
            }
            if (type == "max") {
                if (value > max){
                    max=value;
                }
            }
            if (type == "min"){
                if (value < min){
                    min= value;
                }
            }
            count++;
        }
        
        cout << "The Server B has received " << count << " numbers."<<endl;
        
        cout << "The Server B has successfully finished the reduction " << type <<":";
        
        //prepare to send the results to AWS
        stringstream backend_total;
        backend_total<<"B"; //pack the server name into the string stream with the answer
        
        if (type == "sum"){
            cout << sum << endl;
            backend_total<<sum;
        }
        if (type == "sos"){
            cout << sos << endl;
            backend_total<<sos;
        }
        if (type == "max") {
            cout << max << endl;
            backend_total<<max;
        }
        if (type == "min"){
            cout << min << endl;
            backend_total << min;
        }
        
        //send the values back to the AWS
        while (strcmp(buf, "ready") != 0)
        {
            if ((numbytes = recvfrom(sockfd, buf, MAXDATASIZE - 1, 0,
                                     (struct sockaddr *) &their_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            buf[numbytes] = '\0';
        }
        
        if ((numbytes = sendto(sockfd_out, backend_total.str().c_str(), backend_total.str().length(), 0,
                               p_out->ai_addr, p_out->ai_addrlen)) == -1)
        {
            perror("talker: sendto");
            exit(1);
        }
        
        cout << "The Server B has successfully finished sending the reduction value to AWS server." << endl;
        
    }
    close(sockfd);
    close(sockfd_out);
    return 0;
    
    
}


























