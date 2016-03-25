/**
Author: Marufa Rahmi
Course: COEN283
Programming Assignment #1

System: Linux
OS    : Ubuntu 14.04 LTS
*/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define LINE_BUFSIZE 1024
#define TOKEN_BUFSIZE 256

int cd_command(char **args);
int help_command(char **args);
int exit_command();
int pause_command();
int clear_command();
void set_input_output(char **args);
int pipeHandler(char **args);
int launch(char **args);
int environ_command();

bool input, output;
char *inputFile, *outputFile;

/**
Initializing input = false, ouput = false,
that means default input output from stdin.
if input is true that means input from a file.
if output is true that means output to a file.
*/
void reset_input_ouput(){
    input = false;
    output = false;
}

/**
Check if there exists '<' or '>'
'<' means input from a file.
'>' means output to a file.
*/
void set_input_output(char **args){

    int i = 0;

    while(args[i] != NULL){             /*loop through the command line input*/

        if (!strcmp(args[i], "<")){
            strcpy(args[i], "\0");
            inputFile = args[i+1];
            input = true;
        }
        else if (!strcmp(args[i], ">")){
            outputFile = args[i+1];
            args[i] = NULL;
            output = true;
            break;
        }
        i++;
    }
}

/**
Check if the command runs in background. if the
command contains & that means it should run in
background.
*/
bool check_background(char **args){
    int i = 0;
    bool wait = true;
    while(args[i] != NULL){
        if (!strcmp(args[i], "&")){
            wait = false;
            args[i] = NULL; /*remove the & and set to NULL so that the commmand will work. */
        }
        i++;
    }
    return wait;
}

/**
Check if the input contains more than one command.
*/
bool is_pipe(char **args){
    int i = 0;
	while (args[i] != NULL){
		if (strcmp(args[i],"|") == 0){
			return true;
		}
		i++;
	}
	return false;
}

/**
This function check the first word of the command and
calls the appropriate function.
*/
int execute(char **args)
{
    int i;

    if (args[0] == NULL) {          /*An empty command was entered.*/
        return 1;
    }

    if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "CD") == 0) {
        return cd_command(args);
    }
    else if (strcmp(args[0], "help") == 0 || strcmp(args[0], "HELP") == 0) {
        return help_command(args);
    }
    else if (strcmp(args[0], "clr") == 0 || strcmp(args[0], "CLR") == 0) {
        return clear_command();
    }
    else if (strcmp(args[0], "pause") == 0 || strcmp(args[0], "PAUSE") == 0) {
        return pause_command();
    }
    else if (strcmp(args[0], "environ") == 0 || strcmp(args[0], "ENVIRON") == 0) {
        return environ_command();
    }
    else if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "EXIT") == 0) {
        return exit_command();
    }

    if(is_pipe(args)){
        return pipeHandler(args);
    }
    else {
        return launch(args);
    }
}

/**
 Change the current default directory to <directory>.
*/
int cd_command(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "Shell: expected argument to \"cd\"\n");
    }
    else {
        if (chdir(args[1]) != 0) {
            perror("Shell");
        }
    }
  return 1;
}

/**
Display the user manual using the more filter.
*/
int help_command(char **args)
{
    int i;
    printf("\nProgramming Assignment 1\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n\n");

    printf("cd\t\tTo change directory\n");
    printf("exit\t\tTo terminate the shell\n");

    printf("dir\t\tShow the directories\n");
    printf("clr\t\tClear the screen\n");
    printf("environ\t\tList all the environments strings\n");
    printf("echo <comment>\tDisplay comment\n");
    printf("pause\t\tPause operation of the shell until ‘Enter’ is pressed\n");
    printf("ls\t\tlist all the file and folder name of current directory\n");
    printf("ls -l\t\tlist details all the file and folder of current directory\n");
    printf("ls | wc -l\t\twc -l counts the number of elements output from ls\n");
    printf("ls > out.txt\t\tWrite output of ls to out.txt file\n");
    printf("store < in.txt\t\tGet input from in.txt file\n");

    //printf("Use the man command for information on other programs.\n");
    return 1;
}

/**
which terminates the shell.
*/
int exit_command()
{
    return 0;
}

/**
Clear the screen.
*/
int clear_command()
{
    system("clear");
    return 1;
}

/**
Pause operation of the shell until ‘Enter’ is pressed.
*/
int pause_command()
{
    getpass("Press Enter to continue...");
    return 1;
}

/**
Get all the environment string and show in the screen.
*/
int environ_command(){

    extern char **environ;
    int i = 1;
    char *s = *environ;

    for (; s; i++) {
        printf("%s\n", s);
        s = *(environ+i);
    }
    return 1;
}

/**
This function executes the commands that needs fork
but not pipe().
*/
int launch(char **args)
{
    pid_t pid, wpid;
    int status;
    bool wait = true;
    int fd[2];

    wait = check_background(args);
    pid = fork();

    if (pid == 0) {                     /*Child process*/
        if(output == true)
            freopen(outputFile, "w", stdout);

        if (execvp(args[0], args) == -1) {
            perror("Shell");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        perror("Shell");                /* Error forking*/
    }
    else {                               /* Parent process*/
        do {
            if(wait)wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

  return 1;
}

/**
This function counts the number commands separated
by '|'
*/
int command_count(char **args){

    int i =0, num_cmds = 0;
	while (args[i] != NULL){
		if (strcmp(args[i],"|") == 0){
			num_cmds++;
		}
		i++;
	}
	num_cmds++;

	return num_cmds;
}

/**
This function handle multiple command separated with |.
A command, with or without arguments, whose output is
piped to the input of another command.
*/
int pipeHandler(char **args){

	int filedes[2];             /* pos. 0 output, pos. 1 input of the pipe */
	int filedes2[2];

	int num_cmds = 0;
	char *command[LINE_BUFSIZE];

	pid_t pid;
	int err = -1;
	int end = 0;

	int i = 0; int j = 0; int k = 0; int l = 0;

    num_cmds = command_count(args);

	while (args[j] != NULL && end != 1){
		k = 0;

		while (strcmp(args[j],"|") != 0){
			command[k] = args[j];
			j++;
			if (args[j] == NULL){
				/*'end' variable used to keep the program from entering
				 again in the loop when no more arguments are found*/
				end = 1;
				k++;
				break;
			}
			k++;
		}
		/* Last position of the command will be NULL to indicate that
		   it is its end when we pass it to the exec function*/
		command[k] = NULL;
		j++;

		/* Depending on whether we are in an iteration or another, we
		   will set different descriptors for the pipes inputs and
		   output. This way, a pipe will be shared between each two
		   iterations, enabling us to connect the inputs and outputs of
		   the two different commands.*/
		if (i % 2 != 0){
			pipe(filedes);              // for odd i
		}else{
			pipe(filedes2);             // for even i
		}

		pid=fork();

		if(pid==-1){
			if (i != num_cmds - 1){
				if (i % 2 != 0){
					close(filedes[1]); // for odd i
				}else{
					close(filedes2[1]); // for even i
				}
			}
			printf("Child process could not be created\n");
			return 1;
		}
		if(pid==0){
			// If we are in the first command
			if (i == 0){
				dup2(filedes2[1], STDOUT_FILENO);
			}
			/* If we are in the last command, depending on whether it
               is placed in an odd or even position, we will replace
			   the standard input for one pipe or another. The standard
			   output will be untouched because we want to see the
			   output in the terminal*/
			else if (i == num_cmds - 1){
				if (num_cmds % 2 != 0){             // for odd number of commands
					dup2(filedes[0],STDIN_FILENO);
				}
				else{                               // for even number of commands
					dup2(filedes2[0],STDIN_FILENO);
				}
			/* If we are in a command that is in the middle, we will
			   have to use two pipes, one for input and another for
			   output. The position is also important in order to choose
			   which file descriptor corresponds to each input/output*/
			}else{                                  // for odd i
				if (i % 2 != 0){
					dup2(filedes2[0],STDIN_FILENO);
					dup2(filedes[1],STDOUT_FILENO);
				}
				else{                               // for even i
					dup2(filedes[0],STDIN_FILENO);
					dup2(filedes2[1],STDOUT_FILENO);
				}
			}

			if (execvp(command[0],command)==err){
				kill(getpid(),SIGTERM);
			}
		}

		if (i == 0){
			close(filedes2[1]);
		}
		else if (i == num_cmds - 1){
			if (num_cmds % 2 != 0){
				close(filedes[0]);
			}
			else{
				close(filedes2[0]);
			}
		}else{
			if (i % 2 != 0){
				close(filedes2[0]);
				close(filedes[1]);
			}
			else{
				close(filedes[0]);
				close(filedes2[1]);
			}
		}
		waitpid(pid,NULL,0);
		i++;
	}
	return 1;
}

/**
This function tokenize the input command and stores in
an array *args.
*/
char **tokenizer(char *line)
{
    int bufsize = TOKEN_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "Shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " \t\n");          /*separators are space, tab and newline.*/
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOKEN_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "Shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " \t\n");
    }
    tokens[position] = NULL;
    return tokens;
}


/**
This function reads command until enter is pressed.
*/
char *read_line(void){

    int bufsize = LINE_BUFSIZE;
    char *buffer = malloc(sizeof(char) * bufsize);
    int index = 0, c;

    if (!buffer) {
        fprintf(stderr, "Shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();                  /* Read a character*/

        if (c == EOF || c == '\n') {    /*If we hit EOF, replace it with a null character and return.*/
            buffer[index] = '\0';
            return buffer;
        }
        else {
            buffer[index] = c;
        }
        index++;

        if (index >= bufsize) {         /*If exceeded the buffer, reallocate.*/
            bufsize += LINE_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if(!buffer) {
                fprintf(stderr, "Shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/**
 *	Displays the prompt for the shell
 */
void shellPrompt(){
	char host_name[LINE_BUFSIZE] = "";
	static char* currentDirectory;
	currentDirectory = (char*) calloc(LINE_BUFSIZE, sizeof(char));
	gethostname(host_name, sizeof(host_name));
    printf("%s %s> ",host_name, getcwd(currentDirectory, LINE_BUFSIZE));

}

/**
This function get command one by one until exit is pressed.
*/
void get_command(void)
{
    char *line;
    char **args;
    int status;

    do {
        reset_input_ouput();                /* reset */

        //printf("$> ");
        shellPrompt();
        line = read_line();                 /*read a command line */
        args = tokenizer(line);             /* tokenize the command to get all arguments */
        set_input_output(args);             /* check if the command redirecting to a file */

        if(input){                          /* if command redirect to get input from file */

            printf("input from file. ");
            FILE *f = fopen(inputFile, "r");

            while(fgets(line, LINE_BUFSIZE, f)){ /* read upto end of file*/
                //puts(line);
                printf("\n***Output of %s",line);
                args = tokenizer(line);
                status = execute(args);         /* execute command*/
            }
            fclose(f);
        }
        else{
            status = execute(args);
        }
        free(line);
        free(args);

    } while (status);
}

/**
This function sets shell environment.
*/
void set_shell_env(){

    char home_path[LINE_BUFSIZE];
    getcwd(home_path, LINE_BUFSIZE);
    strcat(home_path, "/Rahmi_shell");
    setenv("shell", home_path, 1);
}

/**
Main function
*/
int main(int argc, char **argv)
{
    set_shell_env();    /* set shell environment*/
    get_command();      /* get command and run*/

    return EXIT_SUCCESS;
}

/**
Test commands:

cd <directory>
cd ..
dir <directory>
help
environ
clr
pause
echo <This is a shell>
ls
ls -l
ls &
ls -l > out.txt
sort < in.txt
ls | wc -l
*/
