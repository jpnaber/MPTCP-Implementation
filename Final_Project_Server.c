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

  //Declare connection info for all 4 connections
  struct sockaddr_in control2, thirdsub;
  struct sockaddr_in firstsub, secondsub;

  struct sockaddr_in connections[4];
  struct sockaddr_in acceptConnections[4];

  connections[0].sin_family = AF_INET;
  connections[0].sin_port = htons(port0);
  connections[0].sin_addr.s_addr = inet_addr(ip);

  connections[1].sin_family = AF_INET;
  connections[1].sin_port = htons(port1);
  connections[1].sin_addr.s_addr = inet_addr(ip);

  connections[2].sin_family = AF_INET;
  connections[2].sin_port = htons(port2);
  connections[2].sin_addr.s_addr = inet_addr(ip);

  connections[3].sin_family = AF_INET;
  connections[3].sin_port = htons(port3);
  connections[3].sin_addr.s_addr = inet_addr(ip);

  //Create sockets
  int fd[4];
  int i;
  for(i = 0; i < 4; i++){
    fd[i] = socket(AF_INET, SOCK_STREAM, 0);
    if(fd[i] == -1){
      printf("socket creation failed...\n");
      return 0;
    }

  }
  
  //Bind sockets to connection info
  i = 0;
  for(i = 0; i < 4; i++){
    if ((bind(fd[i], (struct sockaddr*)&connections[i], sizeof(connections[i]))) != 0) {
      printf("%d socket bind failed...\n", i);
      return 0;
    }
  }

  
  //Listen for incoming connections and bind to sockaddr
  if((listen(fd[0], 1)) != 0){
    printf("Listen failed...\n");
    return 0;
  }
  else{
    printf("listening for connection\n");
  }

  int control2Len = sizeof(control2);
  int connfd = accept(fd[0], (struct sockaddr*)&control2, &control2Len);

  if(connfd < 0){
    printf("server accept failed\n");
  }
  else{
    printf("Server accepted\n");
  }

  if((listen(fd[1], 5)) != 0){
    printf("Listen failed...\n");
    return 0;
  }
  else{
    printf("listening for connection\n");
  }

  int firstsublen = sizeof(firstsub);
  int connfd1 = accept(fd[1], (struct sockaddr*)&firstsub, &firstsublen);

  if(connfd1 < 0){
    printf("server accept failed\n");
  }
  else{
    printf("Server accepted\n");
  }

  if((listen(fd[2], 5)) != 0){
    printf("Listen failed...\n");
    return 0;
  }
  else{
    printf("listening for connection\n");
  }

  int secondsublen = sizeof(secondsub);
  int connfd2 = accept(fd[2], (struct sockaddr*)&secondsub, &secondsublen);

  if(connfd2 < 0){
    printf("server accept failed\n");
  }
  else{
    printf("Server accepted\n");
  }

  if((listen(fd[3], 5)) != 0){
    printf("Listen failed...\n");
    return 0;
  }
  else{
    printf("listening for connection\n");
  }

  int thirdsublen = sizeof(thirdsub);
  int connfd3 = accept(fd[3], (struct sockaddr*)&thirdsub, &thirdsublen);

  if(connfd3 < 0){
    printf("server accept failed\n");
  }
  else{
    printf("Server accepted\n");
  }

  //Open file for later writing
  FILE *fp;
  char *filename = "Final_Project_server.txt";
  fp = fopen(filename, "w");

  //Fork into 3 child processes
  //if pid[0] == 0 youre in the first child
  //if pid[1] == 0 youre in the second child and so forth
  //The final else statement is the parent process
  pid_t pid[3];
  pid[0] = fork();
  
  if(pid[0] == 0){
    //Close pipe for writing only
    close(pipe_1[0]);
    char buffer1[5];
    char data1[992];
    int iteration1 = 0;
    while(1){
      //Read incoming information from client and add to data1 array
      read(connfd1, buffer1, sizeof(buffer1));
      if(buffer1[0] == 'x' && buffer1[1] =='x'){
	    break;
      }
      printf("Child 1: ");
      int z;
      for(z = 0; z < 5; z++){
	data1[(iteration1 * 4)+z] = buffer1[z];
	printf("%c", buffer1[z]);
      }
      iteration1++;
      printf("\n");
    }
   
    //Send data1 array to the parent process for processing
    write(pipe_1[1], data1, strlen(data1));
    exit(0);
  }
  else{
    pid[1] = fork();
    if(pid[1] == 0){
      //Close pipe for writing only
      close(pipe_2[0]);

      char data2[992];
      char buffer2[5];
      int iteration2 = 0;
      while(1){
	//Read incoming information from client and add to data2 array
	read(connfd2, buffer2, sizeof(buffer2));
	if(buffer2[0] == 'x' && buffer2[1] =='x'){
	    break;
	}
	printf("Child 2: ");
	int z;
	for(z = 0; z < 5; z++){
	  data2[(iteration2 * 4)+z] = buffer2[z];
	  printf("%c", buffer2[z]);
	}
	iteration2++;
	printf("\n");
      }
      //Send data2 array to the parent process for processing
      write(pipe_2[1], data2, strlen(data2));
      exit(0);
    }
    else{
      pid[2] = fork();
      if(pid[2] == 0){
	//Close pipe for writing only
	close(pipe_3[0]);
	char buffer3[5];
	char data3[992];
	int iteration3 =0;
	while(1){
	  //Read incoming information from client and add to data3 array
	  read(connfd3, buffer3, sizeof(buffer3));
	  if(buffer3[0] == 'x' && buffer3[1] =='x'){
	    break;
	  }
	  printf("Child 3: ");
	  int z;
	  for(z = 0; z < 5; z++){
	    data3[(iteration3 * 4)+z] = buffer3[z];
	    printf("%c", buffer3[z]);
	  }
	  iteration3++;
	  printf("\n");
	}
        //Send data3 array to the parent process for processing
	write(pipe_3[1], data3, strlen(data3));
	exit(0);
      }
      else{
	//Close all pipes for read only
	close(pipe_1[1]);
	close(pipe_2[1]);
	close(pipe_3[1]);

	int child1seq[90], child2seq[90], child3seq[90];
	int child1 = 0;
	int child2 = 0;
	int child3 = 0;
	char buffer[6];
	int y = 0;
	//Read all incoming connection info and put sequence numbers into correlating child array
	while(y < 248){
	  read(connfd, buffer, sizeof(buffer));
	  if(buffer[0] == 'e' && buffer[1] == 'n' && buffer[2] == 'd'){
	    printf("Parent: exiting loop");
	    break;
	  }
	  printf("Parent: ");
	  int z;
	  for(z = 0; z < 6; z++){
	    printf("%c", buffer[z]);
	  }
	  printf("\n");

	  //Add the sequence number to corresponding child array
	  char *token  = strtok(buffer, ".");
	  token = strtok(NULL, ".");
	  if(buffer[0] == '1'){
	    child1seq[child1] = atoi(token);
	    child1++;
	  }
	  else if(buffer[0] == '2'){
	    child2seq[child2] = atoi(token);
	    child2++;
	  }
	  else{
	    child3seq[child3] = atoi(token);
	    child3++;
	  }
	  y++;
	}
    
	//Get 3 arrays from childs with all of the data
	char child1Data[992];
	char child2Data[992];
	char child3Data[992];
	read(pipe_1[0], child1Data, 992);
	read(pipe_2[0], child2Data, 992);
	read(pipe_3[0], child3Data, 992);
	
	//Find the in order sequence numbers within child seq arrays
	//Once the sequence number is found, print the correlating data
	int seq = 0;
	int dataNum = 1;
	int total = 0;
	while(total < 248){
	  
	  //Find what child the sequence number belongs to
	  int child = 0;
	  for(child = 0; child < 90; child++){
	    if(child1seq[child] == seq){
	      dataNum = 1;
	      break;
	    }
	    else if(child2seq[child] == seq){
	      dataNum = 2;
	      break;
	    }
	    else if(child3seq[child] == seq){
	      dataNum = 3;
	      break;
	    }
	  }

	  //Print the sequence number and correlating data
	  if(dataNum == 1){
	      fprintf(fp, "%d - %c%c%c%c\n", child1seq[child], child1Data[child*4], child1Data[child*4 + 1], child1Data[child*4 + 2], child1Data[child*4 + 3]); //Write to file
	      fflush(fp);
	    }
	    else if(dataNum == 2){
	      fprintf(fp, "%d - %c%c%c%c\n", child2seq[child], child2Data[child*4], child2Data[child*4 + 1], child2Data[child*4 + 2], child2Data[child*4 + 3]); //Write to file
	      fflush(fp);
	    }
	    else if(dataNum == 3){
	      fprintf(fp, "%d - %c%c%c%c\n", child3seq[child], child3Data[child*4], child3Data[child*4 + 1], child3Data[child*4 + 2], child3Data[child*4 + 3]); //Write to file
	      fflush(fp);
	    }

	  seq++;
	  total++;
	}

	//Wait for childs to end before exiting
	printf("Parent: Waiting for childs to end\n");
	while ((wpid = wait(&status)) > 0);
      
	return 0;
	
      }

    }
  }
}
    
