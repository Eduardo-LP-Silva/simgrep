#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h> 
#include <wait.h>
#include <ctype.h>

#define BUFFER_SIZE 512
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN    "\x1b[36m"
#define RESET_COLOR   "\x1b[0m"

void sigint_handler(int signo);

int reading(char* file,int fd1, char* word, int count, int i, int n, int w,int l, int r);

int getWordInSentence(char* file, char* sentence, char* word, int notToShow, int nl, int l, int r);

int getWordInSentence_i(char* file, char* sentence, char* word, int notToShow, int nl, int l, int r);

int getWordInSentence_w(char* file, char* sentence, char* word, int notToShow, int nl, int l, int r);

int getWordInSentence_w_i(char* file, char* sentence, char* word, int notToShow, int nl, int l, int r);

int simgrep_r(char *word, char *directory, int l, int n, int c, int w, int i);

int isFile(char *name);