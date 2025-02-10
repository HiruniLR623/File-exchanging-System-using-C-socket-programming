#include "header.h" // include socket headers

int main(int argc, char **argv){

        int listenfd;
	int connfd[MAX_CLIENTS];
	int i=0,trans_status;
	pid_t	childpid;
	socklen_t clilen;
	const char *ptr;
	void sig_chld(int);     // function to handle child process
	char buffer[BUFFER_SIZE];    // Buffer for data transmission
	char file_name[BUFFER_SIZE];  // Buffer to store files names
	FILE *fp;
	FILE *log_file;
	char *direc = "/home/hirunirathnayake/Desktop/Assignment/send";
	// Directory path where files are stored	

	//creating socket addrr stucture
        struct sockaddr_in servsock, clisock;

	//open log file
	log_file = fopen("server_log.txt", "a");
    	if (log_file == NULL) {
        	perror("Failed to open log file");
        	exit(1);
    	}
	
	//creating socket (TCP) for server listening
        if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
                perror("Socket creation failed\n");
                exit(1);
        }

	//make all 0 (clear the server address structure)
        bzero(&servsock,sizeof(servsock));	

	//assigneing port and ip addr (IPv4, port 9362)
        servsock.sin_family = AF_INET;
        servsock.sin_port = htons(9362);  // set server port
        servsock.sin_addr.s_addr = inet_addr(argv[1]);
        //convert IP address from command-line argument

	//bind the server socket to the specified address and port
        if ((bind(listenfd, (struct sockaddr*)&servsock, sizeof(servsock))) < 0){
                perror("bind failed\n");
                exit(1);
        }        
	printf("waiting for client connections.....");
	
	//start listening for client connections(max backlog of 20)
        if(listen(listenfd,20) < 0){
                perror("listen failed\n");
                exit(1);
        }
	//handle zombie processes when child process exit
	signal(SIGCHLD,sig_chld);	

        // Main loop to accept and process client  connections
        for(i=0; i < MAX_CLIENTS; i++){
				
		bzero(&clisock,sizeof(clisock)); // clear client address structure
		clilen = sizeof(clisock);
		
		//accept a connection from a client
	        if( (connfd[i] = accept(listenfd, (struct sockaddr*)&clisock , &clilen)) < 0 ){
			if(errno == EINTR){
				continue;
			}
			else{
				perror("accept error\n");
				exit(1);
			}
		}	
		
	    
		//getting clients details
		if( (ptr = inet_ntop(AF_INET, &clisock.sin_addr, buffer, sizeof(buffer))) == 0 ) {
			perror("inet_ntop error \n");
			exit (1);
		}
		// print client IP address and port number
		printf("connection from %s, port %d\n", ptr, ntohs(clisock.sin_port));
		ptr = NULL;
		
		
		//creating child server
		if((childpid =  fork())==0)
		{
			close(listenfd);			
			send_list(connfd[i]);			
			
			int file_number, file_count = 1;
			struct dirent *de;     //structure for directory entries
			DIR *dr = opendir(direc);  //open directory to read files

			if (dr == NULL) {
    				perror("Could not open directory");
    				exit(1);
			}

			bzero(buffer,BUFFER_SIZE); // clear the buffer
			
			//get number from client
			read(connfd[i], buffer, sizeof(buffer));
			file_number = atoi(buffer);

			//loop through directory entries and find the selected file
			while ((de = readdir(dr)) != NULL) {
				if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) { 
					if (file_count == file_number){ 
						
						char full_path[BUFFER_SIZE];

						//** delete get file name from de and append it with folder path
            					snprintf(full_path, sizeof(full_path), "%s/%s", direc, de->d_name); 

						//open file in read-binary mode
            					fp = fopen(full_path, "rb");

						if (fp == NULL) {
							perror("Error opening file");
							exit(1);
	    					}
						printf("File opened successfully.\n");
						
						//copy file name and send it to the client
						strcpy(file_name,de->d_name);
						send(connfd[i], file_name, sizeof(file_name), 0);
						
					}

					file_count++; // increment file count
				}
			}

			//sending file to the client
			trans_status = send_file(fp,connfd[i]);

			// log the transfer information to the log file
			log_info(log_file, &clisock, file_name, trans_status);

			fclose(fp);   // close the file
			close(connfd[i]);   // close the client connection
			exit(0);	 // exit the child process					
		
		}
		
		close(connfd[i]); // Parent process closes the client connection

        }

      

        return 0;

}












