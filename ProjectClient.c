#include <stdio.h> 
#include <string.h>    
#include <sys/socket.h>    
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#define port0 29140
#define port1 29041
#define port2 29142
#define port3 29143
#define ip "192.168.254.13"

int main(){

  //Variables used for waiting for child processes to end
  pid_t wpid;
  int status = 0;

  //Create Pipe() for parent process to talk to child proccesses
  int pipe_1[2];
  int pipe_2[2];
  int pipe_3[2];

  //Ensure all of the pipes were created successfully
  if(pipe(pipe_1) == -1){
    printf("Error piping #1");
    return 0;
  }

  if(pipe(pipe_2) == -1){
    printf("Error piping #1");
    return 0;
  }

  if(pipe(pipe_3) == -1){
    printf("Error piping #1");
    return 0;
  }

  
  //Setup control connection
  int socket_desc;
  struct sockaddr_in ControlAddr;
  ControlAddr.sin_family = AF_INET;
  ControlAddr.sin_port = htons(port0);
  ControlAddr.sin_addr.s_addr = inet_addr(ip);

  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_desc < 0){
    printf("Error creating socket\n");
    return 0;
  }
	
  int conn = connect(socket_desc, (struct sockaddr*)&ControlAddr, sizeof(ControlAddr));
  if(conn < 0){
    printf("could not connect\n");
    return 0;
  }
  else{
    printf("Connected to control\n");
  }

  //setup first child connection
  struct sockaddr_in firstChildAddr;
  firstChildAddr.sin_family = AF_INET;
  firstChildAddr.sin_port = htons(port1);
  firstChildAddr.sin_addr.s_addr = inet_addr(ip);
	
  int socket_desc1;
  socket_desc1 = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_desc1 < 0){
    printf("Error creating socket\n");
    return 0;
  }
  int conn1 = connect(socket_desc1, (struct sockaddr*)&firstChildAddr, sizeof(firstChildAddr));
  if(conn1 < 0){
    printf("could not connect\n");
    return 0;
  }
  else{
    printf("Connected to first child\n");
  }

  //setup second child connection
  struct sockaddr_in secondChildAddr;
  secondChildAddr.sin_family = AF_INET;
  secondChildAddr.sin_port = htons(port2);
  secondChildAddr.sin_addr.s_addr = inet_addr(ip);
	
  int socket_desc2;
  socket_desc2 = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_desc2 < 0){
    printf("Error creating socket\n");
    return 0;
  }
  int conn2 = connect(socket_desc2, (struct sockaddr*)&secondChildAddr, sizeof(secondChildAddr));
  if(conn2 < 0){
    printf("could not connect\n");
    return 0;
  }
  else{
    printf("Connected to second child\n");
  }

  //Setup third child connection
  struct sockaddr_in thirdChildAddr;
  thirdChildAddr.sin_family = AF_INET;
  thirdChildAddr.sin_port = htons(port3);
  thirdChildAddr.sin_addr.s_addr = inet_addr(ip);
	
  int socket_desc3;
  socket_desc3 = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_desc3 < 0){
    printf("Error creating socket\n");
    return 0;
  }
  int conn3 = connect(socket_desc3, (struct sockaddr*)&thirdChildAddr, sizeof(thirdChildAddr));
  if(conn3 < 0){
    printf("could not connect\n");
    return 0;
  }
  else{
    printf("Connected to third child\n");
  }
 

  //Fork() 3 Child processes

  pid_t pids[3];
  pids[0] = fork();
  
  if(pids[0] == 0){ //Child 1

    //Close write end of pipe
    close(pipe_1[1]);

    //Print child pids
    printf("First Child\n");
    printf("my id is %d \n", getpid());
    printf("my parentid is %d \n", getppid());
      
    //Loop to read data sent from parent process to send to server
    char readData[5];
    while(1){

      //Read data from pipe
      read(pipe_1[0], readData, 5);

      //If ending characters present, stop child
      if(readData[0] == 'x' && readData [1] == 'x'){
	printf("Child 1: finished\n");
	break;
      }

      write(socket_desc1, readData, sizeof(readData));
	
      //Print data for testing purposes 
      printf("From Child 1: ");
      int z;
      for(z = 0; z < 5; z++){
	printf("%c", readData[z]);
      }
      printf("\n");
    }
    readData[0] = 'x';
    readData[1] = 'x';
    write(socket_desc1, readData, sizeof(readData));
    //Exit child when all data is sent
    exit(0);
  }
  else{
    //Fork into another child
    pids[1] = fork();
    if(pids[1] == 0){ //Child 2

      //Close write end of pipe
      close(pipe_2[1]);

      printf("Second Child\n");
      printf("my id is %d \n", getpid());
      printf("my parentid is %d \n", getppid());

      //Loop to read data sent from parent process to send to server
      char readData[5];
      while(1){
	read(pipe_2[0], readData, 5);
	
	if(readData[0] == 'x' && readData [1] == 'x'){
	  printf("Child 2: finished\n");
	  break;
	}

	write(socket_desc2, readData, sizeof(readData));


	printf("From Child 2: ");
	int z;
	for(z = 0; z < 5; z++){
	  printf("%c", readData[z]);
	}
	printf("\n");
      }
      readData[0] = 'x';
      readData[1] = 'x';
      write(socket_desc2, readData, sizeof(readData));

      exit(0);
    }
    else{
      pids[2] = fork();
      if(pids[2] == 0){ //Child 3
	close(pipe_3[1]);
	
	printf("Third Child\n");
	printf("my id is %d \n", getpid());
	printf("my parentid is %d \n", getppid());

	char readData[5];
	
	while(1){
	  read(pipe_3[0], readData, 5);

	  if(readData[0] == 'x' && readData [1] == 'x'){
	    printf("Child 3: finished\n");
	    break;
	  }

	  //Send over connection to server
	  write(socket_desc3, readData, sizeof(readData));

	  printf("From Child 3: ");
	  int z;
	  for(z = 0; z < 5; z++){
	    printf("%c", readData[z]);
	  }
	  printf("\n");
	}

	readData[0] = 'x';
	readData[1] = 'x';
	write(socket_desc3, readData, sizeof(readData));

	exit(0);
      }
      else{ //Parent
	//Close read side of the pipes
	close(pipe_1[0]);
	close(pipe_2[0]);
	close(pipe_3[0]);

	//Create buffer for 992 bytes of ASCII characters. 16 repetitions
	char buffer[992];
	int i;
	int index = 0;
	for(i = 0; i < 16; i++){
	  int z;
	  for(z = 48; z < 58; z++){
	    buffer[index] = z;
	    index++;
	  }
	  char c;
	  for(c = 'a'; c <= 'z'; c++){
	    buffer[index] = c;
	    index++;
	  }
	  char d;
	  for(d = 'A'; d <= 'Z'; d++){
	    buffer[index] = d;
	    index++;
	  }
	}

	//Wait for children to finish initiating
	sleep(2);

	//Open file for writing sequence and data to
	FILE *fp;
	char *filename = "Final_Project.txt";
	fp = fopen(filename, "w");


	int position = 0; //Position within buffer
	int pipe = 1; //Which pipe is data sent to in the current iteration
	int sequence = 0; //Sequence number of data
	char sendData[5]; //Send data buffer
	char DSS[6]; //Control connection info. Includes sequence number and child number
	int DSSsize = sizeof(DSS);
	char num[3]; //Convert sequence # to char array for DSS
	DSS[1] = '.';

	while(position <= 991){
	  //Put 4 bytes at a time into sendData buffer
	  int i;
	  for(i = 0; i < 4; i++){
	    sendData[i] = buffer[position + i];
	  }
	  sendData[4] = '\0'; //Add delimiter for easy printing
	  fprintf(fp, "%d - %s\n", sequence, sendData); //Write to file
	  fflush(fp);

	  //Decide what child the sendData goes to.
	  if(pipe == 1){
	    write(pipe_1[1], sendData, strlen(sendData)+1);
	    DSS[0] = 49;
	    pipe = 2;
	  }
	  else if(pipe == 2){
	    write(pipe_2[1], sendData, strlen(sendData)+1);
	    DSS[0] = 50;
	    pipe = 3;
	  }
	  else if(pipe == 3){
	    write(pipe_3[1], sendData, strlen(sendData)+1);
	    DSS[0] = 51;
	    pipe = 1;
	  }
	  DSS[1] = '.';

	  //Send DSS over control connection in the form of: "Child Number"."Sequence Number"
	  sprintf(num, "%d", sequence);
	  strcat(DSS, num);
	  printf("Parent: %s\n", DSS);

	  write(socket_desc, DSS, sizeof(DSS));
	

	  //reset DSS and num arrays
	  memset(DSS, 0, sizeof(DSS));
	  memset(num, 0, sizeof(num));

	  //Increase position to get the next 4 bytes and increase sequence
	  position += 4;
	  sequence++;
	}
	DSS[0] = 'e';
	DSS[1] = 'n';
	DSS[2] = 'd';
	write(socket_desc, DSS, sizeof(DSS));

	printf("Parent: Waiting for children to finish initial data\n");
	sleep(2);

	//Send childs termination code
	sendData[0] = 'x';
	sendData[1] = 'x';

	write(pipe_1[1], sendData, strlen(sendData));
	write(pipe_2[1], sendData, strlen(sendData));
	write(pipe_3[1], sendData, strlen(sendData));
	
	//Wait for child to close then exit
	while ((wpid = wait(&status)) > 0);
	fclose(fp);
	return 0;
      }
   }
  }
}
