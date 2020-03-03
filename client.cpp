#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
using namespace std;

struct arguments {
    string message;
    string symbol;
    int i;
};
struct Node
{
    string data;
    Node *link = NULL;
};
Node * head = NULL;
Node * tail = NULL;
/*globals for shared memory */
int portN;
char* arg1;
string* binaryCodes = new string[256];

 
int containsLetter(string * arr, char s,int length);
void insert(string x);
string deleteNode();
void print();
bool isEmpty();
string findCode(string message, string currentLetter);
string removeCurrentSymbol(string message, string symbol);
void* serverFunction(void *arg_void_ptr);

int main(int argc, char *argv[])
{

    portN = atoi(argv[2]);
    arg1=argv[1];

    string message;
	char c;
	while(cin.get(c)){
		if(c == '\n'){
			message+= "\n";
		}else{
			message+= c;
		}
	}
 
    
 
    int messageLength = message.length();
    //freq of symbol array sorted like ['aaa', 'bb', 'c']
    string *letterQuantity = new string[message.length()];
 
 
    //tracks the open index of letterQuantity array for a unique character not seen before. can also be used as length for sorting algo
    int nextOpenIndex = 0;
 
    for (int i = 0; i < message.length(); i++) {
 
        //contains function returns an index to place the letter if it is found if it isnt found it will return -1 so check if statement for -1
        char messageLetter = message[i];
        int indexOfLetter = containsLetter(letterQuantity, messageLetter,message.length());
 
        //element not in letterQuantity array
        if (indexOfLetter == -1) {
            letterQuantity[nextOpenIndex] = message[i];
            nextOpenIndex++;
        }
        else if (indexOfLetter >= 0) {
            letterQuantity[indexOfLetter] += message[i];
        }
        
    }
 
    //find the maximum for every letter and put in a queue 
    for (int i = 0; i < nextOpenIndex; i++) {
        int max = 0;
        int index = 0;
        for (int j = 0; j < nextOpenIndex; j++) {
            if (letterQuantity[j] == "null") {
 
            }
            else if (letterQuantity[j].length() == max){
                //compare ascii values if tied frequency
                if (int(letterQuantity[index][0]) > int(letterQuantity[j][0])) {
                    max = letterQuantity[j].length();
                    index = j;
                }
            }
            else if (letterQuantity[j].length() > max) {
                max = letterQuantity[j].length();
                index = j;
            }
        }
        //put the max letters in the queue here
        if (letterQuantity[index][0] == '\n'){
            cout <<  "<EOL> frequency" <<  " = " << max << endl;
        }else{
            cout << letterQuantity[index][0] << " frequency" <<  " = " << max << endl;
        }
        //insert most frequent character in the queue
        insert(letterQuantity[index]);
        letterQuantity[index] = "null";
        
    }
    
 
    string currentValue;
    //message after removing the letter
    string remainingMessage = message;
    //count how many symbols have been removed from queue
    int counter = 0;
    int pos =0;
    //same as counter
    int numOfProcesses =0;
    string toSearch = "\n";
    string replaceStr= "<EOL>";
    //stores list of the current symbols 
    string *symbolQueue = new string[message.length()];
    //keeps track of the message after each removed letter
    string * remainingMessageArray = new string[message.length()];
    //0 index is original message
    remainingMessageArray[0] = message;
    static struct arguments arg[20];
    pthread_t tid[20];
    //remove each letter from queue one by one and create a process for each
    while (!isEmpty()) {
        currentValue = deleteNode()[0];
        symbolQueue[counter] = currentValue;



        arg[counter].message = remainingMessageArray[counter];
        arg[counter].i = counter;
        arg[counter].symbol = symbolQueue[counter];

        if(pthread_create(&tid[counter], NULL, serverFunction, &arg[counter])) {
            
            cout << "Error creating thread" <<endl;
            return 1;
            
        }         

        //function to return char code
 
        //if pid ==0 break
    

        
        // remove ! and replace with <EOL>
        pos = remainingMessageArray[counter].find("\n");
        while( pos != std::string::npos)
        {
            
            remainingMessageArray[counter].replace(pos, toSearch.size(), replaceStr);
            
            pos =remainingMessageArray[counter].find(toSearch, pos + replaceStr.size());
        }
        numOfProcesses++;
        counter++;
        remainingMessage = removeCurrentSymbol(remainingMessage, currentValue);
        remainingMessageArray[counter] = remainingMessage;
    }
 
	for(int i = 0; i < numOfProcesses; i++){
        pthread_join(tid[i], NULL);
    }
 
    pos = message.find("\n");
    while( pos != std::string::npos)
        {
            // Replace this occurrence of Sub String
            message.replace(pos, toSearch.size(), replaceStr);
            // Get the next occurrence from the current position
            pos =message.find(toSearch, pos + replaceStr.size());
        }
    
 
    string line;
    cout << "Original Message: " << remainingMessageArray[0] << endl;

    for(int i = 0; i < counter; i++){

            //replace with reading from ith element of binary code array

        if (symbolQueue[i] == "\n"){
            cout << "Symbol <EOL> code: " << binaryCodes[i] << endl;
        }else{
            cout << "Symbol " << symbolQueue[i] << " code: " << binaryCodes[i] << endl;
        }
        if (i != counter -1){
            cout << "Remaining Message: " << remainingMessageArray[i+1] << endl;
        }

        
    }

 
}
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
string findCode(string message,string currentLetter) {
    string finalCode;
    string messageLetter;
    for (int i = 0; i < message.length(); i++) {
        messageLetter = message[i];
        if (messageLetter == currentLetter) {
            finalCode += "1";
        }
        else {
            finalCode += "0";
        }
    }
    return finalCode;
}
 
 
 
 
 
void insert(string x) {
    Node * temp = new Node();
    if (tail == NULL) {
        tail = temp;
    }
    temp->data = x;
    temp->link = head;
    head = temp;
}
string deleteNode() {
    Node * temp1 = head;
    Node * temp2;
 
    if ((head == NULL) && (tail == NULL)){
        return "error";
    }
    else if (head == tail) {
        Node * deletedNode = head;
        string dataReturned = head->data;
        delete deletedNode;
        head = NULL;
        tail = NULL;
        return dataReturned;
 
    }
    else {
        while (temp1->link != tail) {
            temp1 = temp1->link;
        }
        temp2 = tail;
        string returnValue = temp2->data;
        delete temp2;
        tail = temp1;
        temp1->link = NULL;
 
        return returnValue;
    }
 
    
}
 
void print() {
    Node * temp = head;
    if (temp == NULL) {
        cout << "Empty" << endl;
    }else{
        while (temp != NULL) {
            cout << temp->data << "-> ";
            temp = temp->link;
        }
        cout << endl;
    }
 
}
 
bool isEmpty() {
    if (head == NULL) {
        return true;
    }
    return false;
 
}
 
int containsLetter(string * arr, char s, int length) {
    string arrayValue;
 
    for (int i = 0; i < length; i++) {
        arrayValue = arr[i];
        if (arrayValue[0] == s) {
            return i;
        }
    }
    return -1;
 
 
}
 
string removeCurrentSymbol(string message, string symbol) {
    string remainingMessage;
 
    for (int i = 0; i < message.length(); i++) {
        if (message[i] != symbol[0]) {
            remainingMessage += message[i];
        }
    }
 
 
    return remainingMessage;
}

void* serverFunction(void *arg_void_ptr){
    struct arguments *arg_ptr = (struct arguments *)arg_void_ptr;
    int sockfd, portno, n, m;

    //used for passing to bind function
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    portno = portN;
    //socket returns a socket descriptor which is an integer AF_NET is for ipv4 addresses SOCK_STREAM is saying we wanta reliable connection like TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //socket failed 
    if (sockfd < 0) {
        cout << "Error sockfd < 0" << endl;
        exit(0);
    }
    //gets ip from dns 
    server = gethostbyname(arg1);
    if (server == NULL) {
        cout << "ERROR, no such host" << endl;
        exit(0);
    }

    //in this chunk we are setting properties of the serv_addr variable before we pass it to the connect function there is no bind function for client
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        cout << "Error connecting" << endl;
        exit(0);
    }
   
    
    int sizeArr1 = arg_ptr->message.length();
    
    char messageBuffer[256];
    char currentLetter[256];
    
    strcpy(messageBuffer,arg_ptr->message.c_str());
    strcpy(currentLetter,arg_ptr->symbol.c_str());
    


    n = write(sockfd,messageBuffer,sizeof(messageBuffer));
    if (n < 0) {
         cout << "Error writing to socket" << endl;
         exit(0);
    }
    m = write(sockfd,currentLetter,sizeof(currentLetter));
    if (m < 0) {
         cout << "Error writing to socket" << endl;
         exit(0);
    }
    bzero(buffer,256);
    char finalCodeSeq[256];
    n = read(sockfd,finalCodeSeq,256);
    if (n < 0) {
         cout << "Error reading from socket" << endl;
         exit(0);
    }
    string ss(finalCodeSeq);
    binaryCodes[arg_ptr->i] = ss;
}