# Operating-System
Linux Shell

Shell supports the following internal commands: 
1. cd <directory> - Change the current default directory to <directory>. If the <directory> argument is not present, report the current directory. If the directory does not exist, an appropriate error should be reported. This command should also change the PWD
environment variable. 
2. clr – Clear the screen 
3. dir <directory> - List the contents of directory <directory>. 
4. environ – List all the environments strings. 
5. echo <comment> - Display <comment> on the display followed by a new line
(multiple spaces/tabs may be reduced to a single space). 
6. help – Display the user manual using the more filter. 
7. pause – Pause operation of the shell until ‘Enter’ is pressed. 
8. The shell environment contain shell=<pathname>/myshell where <pathname>/myshell is the full path for the shell executable (not a hardwired path back to the directory, but the one from which it was executed)

Also Supports:
1. The internal shell command "exit" which terminates the shell. 
  Concepts: shell commands, exiting the shell 
  System calls: exit() 
2. A command with no argumentsExample: ls 
  Details: Your shell must block until the command completes and, if the return code is abnormal, print out a message to that effect. 
  Concepts: Forking a child process, waiting for it to complete, synchronous execution 
  System calls: fork(), execvp(), exit(), wait() 
3. A command with arguments
  Example: ls -l 
  Details: Argument 0 is the name of the command 
  Concepts: Command-line parameters
4. A command, with or without arguments, executed in the background using &.For simplicity, assumed that if present the & is always 
  the last thing on the line.
  Example: xemacs & 
  Details: In this case, your shell must execute the command and return immediately, not blocking until the command finishes. 
  Concepts: Background execution, signals, signal handlers, processes, asynchronous execution 
  System calls: sigset() 
5. A command, with or without arguments, whose output is redirected to a file
  Example: ls -l > foo 
  Details: This takes the output of the command and put it in the named file 
  Concepts: File operations, output redirection 
  System calls: freopen()
6. A command, with or without arguments, whose input is redirected from a file
  Example: sort < testfile 
  Details: This takes the named file as input to the command 
  Concepts: Input redirection, more file operations 
  System calls: freopen() 
7.A command, with or without arguments, whose output is piped to the input of another command.
  Example: ls -l | more 
  Details: This takes the output of the first command and makes it the input to the second command 
  Concepts: Pipes, synchronous operation 
  System calls: pipe()
