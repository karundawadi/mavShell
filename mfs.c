/*
  Name : Karun Dawadi 
  Student ID : 1001660099
*/
// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017, 2020 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

/*
  Linked list stores the pid and the all the arguments entered for the process 
  Has the index of the process
*/
typedef struct history_pid{
  int pid_id;
  char * storedText;
  int interal_process_id;
  struct history_pid* next_process;
  struct history_pid* prev_process;
}history_pid;

//Our actual queue 
typedef struct actual_queue{
  history_pid *front,*back;
}actual_queue;

int delete_from_queue(actual_queue* queue,int pid_id, char * token,int current_index);
void add_to_queue(actual_queue* queue,int pid_id, char * token,int current_index);
actual_queue* create_queue();
history_pid* new_process(int pid_id,char * token,int current_index);
void print_queue(actual_queue* queue, int which_to_print, int line_no);

//Main function
int main()
{
  //These are defined for the history feature and showpids feature 
  int process_id = -10;
  actual_queue* queue = create_queue();
  int counter = 1;

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
  
    //Handling the case when the first chracter entered is a return key and follows multiple white spaces 
    if((cmd_str[0]=='\n')||(cmd_str[0]==' ')){
      continue;
    }
    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         

    if(cmd_str[0] == '!'){
      char* new_tok = NULL;
      new_tok = strtok(cmd_str,"! ");
      int number_to_search = atoi(new_tok);
      printf("%d \n",number_to_search);
      history_pid* starting_value = queue->front;
      int i =0;
      while(starting_value != NULL){
        ++i;
        if(i == number_to_search){
          strcpy(cmd_str,starting_value->storedText);
        }else{
          starting_value = starting_value->next_process;
        }
      }
      if(i>number_to_search){
        printf("Command not found on history \n");
      }
    }
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
    //Increasing the counter here
    counter++;
    // Adding the details to the linked list 
    add_to_queue(queue,process_id,cmd_str,counter);

    //Print the tokenized input as a debug check
    int token_index  = 0;

    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );  
    }

    //Code starts here
    if((strcmp(token[0],"exit")==0)||(strcmp(token[0],"quit")==0)){
      exit(0);
    }
    else if(strcmp(token[0],"cd")==0){
      //Converting first argument after cd to a string 
      char str_cpu[100];
      strcpy(str_cpu,token[1]);
      int changed_dir = chdir(str_cpu);
    }
    else if(strcmp(token[0],"history")==0){
      //1 means print history and 0 meand re-run specfic task ; the end
      //Number is the number to re-execute again 
      print_queue(queue,1,0);
    }
    
    else if(strcmp(token[0],"showpids")==0){
      //2 means print showpids
      print_queue(queue,2,0);
    }

    else{
      pid_t pid =fork();
      process_id = pid;

      if(pid == -1){
        perror("Fork failed");
        exit(EXIT_FAILURE);
      }

      else if(pid == 0){
        // Running execvp here as we do not specify the file path to it 
        int return_val = execvp(token[0],&token[0]);
        //To check if the command exist or not 
        if (return_val == -1){
          printf("%s: Command not found. \n",token[0]);
        }
        fflush(NULL);
        exit(EXIT_SUCCESS);
      }
      else{
        int status;
        //Waiting for the child to complete finishing 
        waitpid(pid,&status, 0);
        fflush(NULL);
      }
    }
    free( working_root );
    }
  return 0;
}


//This function replaces from the front of the queue, return 1 if able,return 0 if false 
/*
  Takes in the process details and adds it to the front, freeing the front one 
*/
int delete_from_queue(actual_queue* queue,int pid_id, char * token,int current_index){
  history_pid* temp = queue->front;
  queue->front =  temp->next_process;
  free(temp);
  return 1;
}

//Create a new node with all the details 
history_pid* new_process(int pid_id,char * token,int current_index){
  history_pid* temp_process = (history_pid*) malloc(sizeof(history_pid));
  temp_process->next_process = NULL;
  temp_process->pid_id = pid_id;
  temp_process->interal_process_id = current_index;
  temp_process->storedText = (char*) malloc(MAX_COMMAND_SIZE);
  strcpy(temp_process->storedText, token);
  return temp_process;
}

//This creates out actual_queue with process ids and other details
actual_queue* create_queue(){
  actual_queue* queue = (actual_queue*) malloc(sizeof(actual_queue));
  queue->front = NULL;
  queue->back = NULL;
  return queue;
}

//This function adds to the queue
void add_to_queue(actual_queue* queue,int pid_id, char * token,int current_index){
    if(current_index > 15){
      delete_from_queue(queue,pid_id,token,current_index);
    }  
    history_pid* temp_process = new_process(pid_id,token,current_index);
    //When the queue is empty, we have 
    if(queue->back == NULL){
      queue->front = queue->back = temp_process;
      return;
    }
    queue->back->next_process = temp_process;
    queue->back = temp_process;
}

//This process frees all the nodes in the queue
void print_queue(actual_queue* queue, int which_to_print, int line_no){
    // 1 means history 
    if(which_to_print == 1){  
      history_pid* starting_value = queue->front;
      int i =1;
      //In normal printing cases 
      while(starting_value != NULL){
        if (i>15) i=15;
        printf("%d) %s",i,starting_value->storedText);
        i++;
        starting_value = starting_value->next_process;
      }
    }
    // 2 means showpids
    else if(which_to_print == 2){
      history_pid* starting_value = queue->front;
      int i =1;
      //In normal printing cases 
      while(starting_value != NULL){
        if (i>15) i=15;
        if(starting_value->pid_id == -10){
          printf("Terminal Pid : %d. \n",getppid());
        }else{
        printf("%d) %d \n",i,starting_value->pid_id);
        i++;
        }
        starting_value = starting_value->next_process;
      }
    }
    else{
      //do nothing 
    }
};