#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>

#define BUFFER_SIZE 512
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN    "\x1b[36m"
#define RESET_COLOR   "\x1b[0m"

int simgrep_r(char *word, char *directory, int l, int n, int c, int w, int i);
int isFile(char *name);

static pid_t g_pid;
static int cont = 0;

void sigint_handler(int signo)
{
	char answer, trash;
	do{
		write(STDOUT_FILENO, "\nAre you sure you want to terminate the program? (Y/N) ", 55);
		fflush(stdout);
		read(STDIN_FILENO, &answer, 1);
		read(STDIN_FILENO, &trash, 1);
		while(trash != '\n')
			read(STDIN_FILENO, &trash, 1);
		if (answer == 'Y' || answer == 'y')
		{
			kill(g_pid, SIGINT);
			exit(2);
		}
		if(answer != 'N' && answer != 'n')
			write(STDOUT_FILENO, "\nWrong answer. Try again.",25);
	} while(answer != 'N' && answer != 'n');
	cont = 1;
}

int main(int argc, char *argv[]) 
{

	if(argc < 2)
	{
		printf("Usage: simgrep [OPTION]... PATTERN [FILE]...\n");
		return 1;
	}
	struct sigaction action;
			action.sa_handler = sigint_handler;
			sigemptyset(&action.sa_mask);
			action.sa_flags = 0;
			if (sigaction(SIGINT, &action, NULL) < 0) {
				fprintf(stderr, "Unable to install SIGINT handler\n");
				exit(1);
			}
	int l = 0, i = 0, w = 0, n = 1, c = 0;

    simgrep_r("for", "textfiles", l, n, c, w, i);

    /*
    for(i = 1; i < argc; i++)
    {
        option = argv[i];
        memcpy(options, &option[1], 1);
    }

    
    if(argc >= 2)
    {
        if(strcmp(argv[1], "-r"))
            simgrep_r(argv[2]);
    }    
    
	if(argc == 2)
	{
		fd1 = STDIN_FILENO;
	} 
    else 
    {
		fd1 = open(argv[2], O_RDONLY);

		if (fd1 == -1) 
        {
			perror(argv[2]);
			return 2;
		}
	}

	char c;
	int m = 0;
	char buffer[BUFFER_SIZE];

	while (read(fd1, &c, 1) > 0) 
    {
		buffer[m] = c;
		m++;

		if(c == '\n')
		{
			char *check_word = malloc(m*sizeof(char));

			strncpy(check_word, buffer, m);
			m = 0;

			if(strlen(check_word) >= strlen(argv[1]))
				getNWordsInSentence(check_word, argv[1]);

            
		}
	}

	/*if (fd1 != STDIN_FILENO) 
    {
		char *check_word = malloc(m * sizeof(char));

		strncpy(check_word, buffer, m);
		m = 0;

		if (strlen(check_word) >= strlen(argv[1]))
			getNWordsInSentence(check_word, argv[1]);
	} */

	//close(fd1); 

	return 0;
}

int reading(char* file,int fd1, char* word, int count, int i, int n, int w,int l, int r)
{
	char c;
	int m = 0, nWords =0, n_line = 0;
	char buffer[BUFFER_SIZE];
	if(n)
		n_line++;

	while (read(fd1, &c, 1) > 0) {
		buffer[m] = c;
		m++;
		if(c == '\n')
		{
			char *check_word = malloc(m*sizeof(char));
			strncpy(check_word, buffer, m);
			m = 0;

			if(strlen(check_word) >= strlen(word))
			{
				if(i){
					if(getWordInSentence(file,check_word, word, count, n_line,l,r)==1)
						{nWords++;

						if(l==1)
							if(fd1==STDIN_FILENO){
							printf(COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
							return 0;}

							else printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
						}
				}
				else if(w){
					if(getWordInSentence(file,check_word, word, count, n_line,l,r)==1)
						{nWords++;

						if(l==1)
							if(fd1==STDIN_FILENO){
							printf(COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
								return 0;}
							else printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
						}
				}
				else{

					if(getWordInSentence(file,check_word, word, count, n_line,l,r)==1)
						{nWords++;

						if(l==1)
							if(fd1==STDIN_FILENO){
							printf(COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
							return 0;}

							else
							{
								printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
								if(r == 1)
									return 0;
							}
						}
			}}
			if(n)
				n_line++;
		}
	}
	if (fd1 != STDIN_FILENO) {
		char *check_word = malloc(m * sizeof(char));
		strncpy(check_word, buffer, m);
		m = 0;
		if(strlen(check_word) >= strlen(word))
			{
				if(i)
					if(getWordInSentence(file,check_word, word, count, n_line,l,r)==1)
						{nWords++;
						if(l==1)
							printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
							return 0;
						}
				else if(w)
					if(getWordInSentence(file,check_word, word, count, n_line,l,r)==1)
						{nWords++;
						if(l==1)
							printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
							return 0;
						}
				else

					if(getWordInSentence(file,check_word, word, count, n_line,l,r)==1)
						{
						nWords++;
						if(l==1)
							printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
							return 0;
						}
			}
	}
	if(count && !r)
		printf("%d\n", nWords);
	else if (count && r)
		printf(COLOR_MAGENTA "%s" COLOR_CYAN ":" RESET_COLOR "%d\n",file, nWords);
	return 0;
}

int simgrep_r(char *word, char *directory, int l, int n, int c, int w, int i)
{
    DIR *d;
    struct dirent *dir;
    int file_type;
    int fd1;
    pid_t pid;
	d = opendir(directory);

	if (d) {
		while ((dir = readdir(d)) != NULL) {
			char* str1 = malloc(BUFFER_SIZE);
			strcat(str1, directory);
			strcat(str1, "/");
			strcat(str1, dir->d_name);
			file_type = isFile(str1);

            switch(file_type)
            {
                case -1:
                    printf("Error\n");
                    return -1;

                case 0:
                	fd1 = open(dir->d_name, O_RDONLY);
                	char* str = malloc(BUFFER_SIZE);
                	strcat(str, directory);
                	strcat(str,"/");
                	strcat(str,dir->d_name);
                	reading(str, fd1, word, c,i,n,w,l,1);
                	close(fd1);
                    break;

                case 1:
                	pid = fork();
                	if(pid == 0)
                	{
                		setpgrp();
                		//sleep(3); //TEST
                		simgrep_r(word, str1, l, n, c, w, i);
                		return 0;
                	}
                	else
                	{
                		g_pid = getpgid(getpid());
                		waitpid(pid, NULL ,0);
                	}
                    break;

                case 2:
                    break;
            }
        }
		closedir(d);
	}
	return 0;
}

int getWordInSentence(char* file, char* sentence, char* word, int notToShow, int nl, int l, int r)
{
	int i = 0, j = 0;
	int number = 0, found = 0;
	char* check_word = malloc(strlen(word)*sizeof(char));
	while (i < strlen(sentence) -strlen(word)) {
		strncpy(check_word, sentence + i, strlen(word));
		int equals = 1;
		for (int m = 0; m < strlen(word); m++) {
			if (*(check_word + m) != *(word + m))
				equals = 0;
		}
		if (equals == 1) {
			number++;

			if(found == 0){
				found = 1;
				if(l==1)
					return 1;
			}
			else
				if(found == 1)
					found = 2;

			if(notToShow)
				break;
			if (i != j) {
				char* to_show = malloc(BUFFER_SIZE);
				strncpy(to_show, sentence + j, i - j);
				if (found == 1 && r != 0)
					printf(COLOR_MAGENTA "%s" COLOR_CYAN ":" RESET_COLOR, file);
				if(found == 1 && nl != 0)
					printf(COLOR_GREEN "%d" COLOR_CYAN ":" RESET_COLOR, nl);
				printf("%s", to_show);
			}
			if (i == 0 && found == 1 && r != 0)
				printf(COLOR_MAGENTA "%s" COLOR_CYAN ":" RESET_COLOR, file);
			if(i == 0 && found == 1 && nl != 0)
				printf(COLOR_GREEN "%d" COLOR_CYAN ":" RESET_COLOR, nl);
			printf(COLOR_RED "%s" RESET_COLOR, check_word);
			j = i + strlen(word);
		}
		i++;
	}
	if(number > 0)
	{
		if(notToShow)
			return 1;
		char* to_show = malloc(BUFFER_SIZE);
		strncpy(to_show, sentence + j, strlen(sentence)-j);
		printf("%s", to_show);
		return 1;
	}

	return 0;
}


/* Returns 0 if it is a file, 1 if a directory, 2 if current directory or directory above,
   -1 if an error occurred*/
int isFile(char *name)
{
    int status;
    struct stat st_buf;

    char* dot = malloc(BUFFER_SIZE), *two_dots = malloc(BUFFER_SIZE);
    strncpy(dot, name + strlen(name)-1, 1);
    strncpy(two_dots, name + strlen(name)-2, 2);
    status = stat(name, &st_buf);

    if(status != 0)
    {
        printf("Error in opening file/directory\n");
        return -1;
    }
    
    if(!strcmp(dot, ".") || !strcmp(two_dots, "..") )
        return 2;

    if(S_ISREG(st_buf.st_mode))
        return 0;
        
            
    if(S_ISDIR(st_buf.st_mode))
        return 1;

    return -1;
}




