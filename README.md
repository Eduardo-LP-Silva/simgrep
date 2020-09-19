# simgrep

A simplified version of the "grep" command developed in the Operative Systems class. It searches files and directories for text 
patterns.

## Overview

The program is called in the following manner: simgrep [options] pattern [file/dir]

### Available options

* -i - Ignores capitalization.
* -l - Shows only the names of the files where the pattern is present.
* -n - Displays the line number where the pattern is present.
* -c - Returns the number of lines in which the pattern occurs.
* -w - The pattern must be a complete word.
* -r - The search is extended to all subdirectories of the one specified.

### Aditional Features

* User interruption: If the user presses CTRL+C, the program will stop the search and will ask the user if he really wants to terminate.
* Execution file logs: For this feature to work, the user must create an environment variable called LOGFILENAME. Then, after each call
of simgrep, each process and child process will register in a file (it is created a new one if it doesn't exist) each main event,
like opening/closing of files, reception and emission of signals, etc...  
The file has the following format: inst - pid -act.  
  * inst - Represents the time instace before writing, measured in miliseconds(ms).
  * pid - Process identifier.
  * act - Event description.
  
## Set up

First compile simgrep.c using the Unix C compiler.

* cc simgrep.c -o simgrep

Then run it like any other program in Unix:

* ./simgrep [options] pattern [file/dir]
