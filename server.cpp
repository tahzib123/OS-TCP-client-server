// Code provided by instructor for basic server and client program.

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
using namespace std;


struct arguments
{
	string symbol = "a";
	string message = "SUP";
};


//provided by microhowto.info site 
void fireman(int sig) {
  while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
}

string findCode(char symbol[], char message[]) {
    string finalCode;
    char* messageLetter;

    for (int i = 0; i < strlen(message); i++) {
        
        if (message[i] == symbol[0]) {
            finalCode += "1";
        }
        else {
            finalCode += "0";
        }
    }

	return finalCode;

}

void forkFunction(int newsockfd){
      int n;
      int m;
      char bb1[256];
      char bb2[256];
      //read message from the client stored in newsockfd
      n = read(newsockfd,bb1,sizeof(bb1));
      if (n < 0){
          cout << "ERROR reading from socket" << endl;
          exit(0);
      }
      m = read(newsockfd,bb2,sizeof(bb2));
      if (m < 0){
          cout << "ERROR reading from socket" << endl;
          exit(0);
      }    

      
      //responds to the client by saying i got your message
      string finalCode = findCode(bb2,bb1);
      const char* sendValue = finalCode.c_str();
      n = write(newsockfd,sendValue,strlen(sendValue));
      if (n < 0){
        cout << "ERROR writing to socket" << endl;
        exit(0);
      }
     
}



int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, pid;
     char buffer[256];

     //serv_addr used in bind function 
     struct sockaddr_in serv_addr, cli_addr;
     if (argc < 2) {
         cout << "ERROR, no port provided" << endl;
         exit(1);
     }

     //socket returns socket desciptor which is an integer AF_INET is the ipv4 standard SOCK_STREAM is saying we want a reliable connection like TCP
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) {
       cout << "ERROR opening socket" << endl;
       exit(0);
     }
     //in this chunk we are assigning properties of serv_addr before we pass it to the bind function   
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     //binds sockfd based on the properties of serv_addr defined above
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
       cout << "ERROR on binding" << endl;
       exit(0);
     }

    //num of connections max is 5 after call to listen the server waits for any connections and then once it accepts them it moves on with the program
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     //accept a connection and returns the client socket descriptor 
     while(1){
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
      if (newsockfd < 0) {
        cout << "ERROR on accept" << endl;
        exit(0);
      }
      pid = fork();
      if (pid < 0){
        cout << "ERROR on fork" << endl;
      }
      if (pid == 0) {
          close(sockfd);
          forkFunction(newsockfd);
          exit(0);
      }
      signal(SIGCHLD,fireman);
     }


}
