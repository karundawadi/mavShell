# Mav Shell
A C program that works as a shell when run.

## Code features  
- Implements a queue to display the history of commands and process ids 
- Uses fork(), exec(), and wait() functions 

## Features 
- Support all commands that redirect to:
    1. Current Working Directory 
    2. /usr/local/bin 
    3. /usr/bin
    4. /bin
- Supports up to 10 command line parameters 
- Handles cases for whitespace and return conditions 
- Identifies options that are invalid 
- Idenfies which commands are not supported 
- Supports change directories commands 
- Displays 15 processes ids when *showpids* is typed 
- *!n* runs the command present in the nth position in *history* tree 
- Exits when "quit" or "exit" is typed 
### To Run 
To run the program, clone the repo, navigate shell to the folder,and type gcc msh.c -o msh
