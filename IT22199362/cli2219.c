#include "header.h" // Include socket headers

int main(int argc, char **argv){

        int     srvfd, a, selected_file_number;
        char    buffer[BUFFER_SIZE];  //Buffer for receiving data from the server
	char    send_buffer[BUFFER_SIZE];  //Buffer for sending data to the server

        struct sockaddr_in servsock;     // structure to hold server's address information
        // Creating a socket (TCP) using SOCK_STREAM
        if ((srvfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
                printf("Socket creation failed\n");
                exit(1);
        }
        //Clear the servsock structure to avoid garbage values
        bzero(&servsock, sizeof(servsock));

        //Set up the server address (IPv4, port 9362)
        servsock.sin_family = AF_INET;
        servsock.sin_port   = htons(9362);  // convert port number to network byte order
        servsock.sin_addr.s_addr = inet_addr(argv[1]); // Convert the Ip address from the argument to binary
        //Attempt to connect to the server
        a = connect(srvfd, (struct sockaddr*)&servsock,  sizeof(servsock));

        if(a == 0){
                printf("Connection Successful \n");
        }
        else{
                printf("Connect Error");
                exit(1);
        }

        // Clear the buffer before use
  	bzero(buffer,BUFFER_SIZE);
        
        //Read available files list from the server
        read(srvfd, buffer, BUFFER_SIZE);
        printf("Available files:\n%s\n", buffer);
        
        //Prompt the user to enter the file number they want to download
	printf("Enter the number of the file you want to download: ");
	scanf("%d", &selected_file_number);
	
        //Send the selected file number to the server
	snprintf(send_buffer, sizeof(send_buffer), "%d", selected_file_number);
        write(srvfd, send_buffer, strlen(send_buffer));

        //Clear the buffer before receiving the file
	bzero(buffer,BUFFER_SIZE);
	read(srvfd, buffer, sizeof(buffer));

	// Call the function to receive the file
	recv_file(srvfd,buffer);         
      
}

