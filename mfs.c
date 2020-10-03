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

//Defining functions below this 
void mavShell(char ** token, int token_count);


//Main function
int main()
{

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

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
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

    //Print the tokenized input as a debug check
    int token_index  = 0;

    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );  
    }
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
      //Need to print out the history 
    }
    else if(strcmp(token[0],"pwd")==0){
      //Prints out the current directory 
      execl("/bin/pwd","pwd",NULL);
    }
    else if(strcmp(token[0],"!")==0){
      //Need to perform task demanded by !
    }
    else{
      pid_t pid =fork();

      if(pid == -1){
        perror("fork failed");
        exit(EXIT_FAILURE);
      }

      else if(pid == 0){
        printf("Child task \n");
        // Running execvp here as we do not specify the file path to it 
        int return_val = execvp(token[0],&token[0]);
        printf("%d \n",return_val);
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
        //(void) removed from here
        waitpid(pid,&status, 0);
        fflush(NULL);
      }
  }

  free( working_root );
  }
  return 0;
}
