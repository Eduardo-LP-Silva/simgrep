#include "simgrep.h"

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

int main(int argc, char *argv[]) {
	int fd1, fromKeyboard = 0, indexPat = argc - 1;
	int c = 0, i = 0, n = 0, w = 0, l = 0, r = 0; //to check if -c, -i, -n, -w, -l, -r is called

	if(argc < 2)
	{
		printf("Usage: simgrep [OPTION]... PATTERN [FILE]...\n");
		return 1;
	}

	char* a = malloc(BUFFER_SIZE);

	if(strlen(argv[argc-1]) > 4)
	{
		/*strncpy(a, argv[argc-1] + strlen(argv[argc-1]) - 4, 4);
		if(a[0] != '.' || a[1] != 't' || a[2] != 'x' || a[3] != 't')*/
		if(isFile(argv[argc-1]) == -1)
			fromKeyboard = 1;
	}
	if(fromKeyboard || strlen(argv[argc-1]) <= 4)
	{
		fd1 = STDIN_FILENO;
	} else {
		fd1 = open(argv[argc-1], O_RDONLY);
		if (fd1 == -1) {
			perror(argv[argc-1]);
			return 2;
		}
		indexPat = argc-2;
	}

	for (int j = 1; j < argc; j++) {
		strncpy(a, argv[j], 2);
		if(a[0] != '-')
			break;
		if (a[1] == 'c')
			c = 1;
		else if (a[1] == 'i')
			i = 1;
		else if (a[1] == 'l')
			l = 1;
		else if (a[1] == 'n')
			n = 1;
		else if (a[1] == 'w')
			w = 1;
		else if (a[1] == 'r')
			r = 1;
	}

	int isfile;
	if(r == 1)
	{
		isfile = isFile(argv[argc-1]);
		if(isfile == 0)
			r = 0;
		else if(isfile == 1 || isfile == 2)
			indexPat = argc - 2;
	}

	struct sigaction action;
	action.sa_handler = sigint_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	if (sigaction(SIGINT, &action, NULL) < 0) {
		fprintf(stderr, "Unable to install SIGINT handler\n");
		exit(1);
	}

	if(r == 1)
	{
		close(fd1);
		if(isfile != -1)
			simgrep_r(argv[argc-2], argv[argc-1], l, n, c, w, i);
		else

			simgrep_r(argv[argc-1], ".", l, n, c, w, i);
		return 0;
	}

	goingon: reading(argv[argc-1],fd1, argv[indexPat], c, i, n,w, l, r);

	if(cont)
	{
		cont = 0;
		goto goingon;
	}

	close(fd1);
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
				if(i && !w){
					if(getWordInSentence_i(file,check_word, word, count, n_line,l,r)==1)
						{nWords++;

						if(l==1){
							if(fd1==STDIN_FILENO){
							printf(COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
							return 0;}

							else
							{
								printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
								return 0;
							}
					}}
				} else if (i && w) {
					if (getWordInSentence_w_i(file, check_word, word, count,
							n_line, l, r) == 1) {
						nWords++;

						if (l == 1){
							if (fd1 == STDIN_FILENO) {
								printf(
										COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
								return 0;
							} else
							{
								printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
								return 0;
							}
					}}
				} else if (!i && w) {
					if (getWordInSentence_w(file, check_word, word, count,n_line, l, r) == 1) {
						nWords++;

						if (l == 1){
							if (fd1 == STDIN_FILENO) {
								printf(COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
								return 0;
							} else
							{
								printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
								return 0;
							}
				}}
				} else {

					if(getWordInSentence(file,check_word, word, count, n_line,l,r)==1)
						{nWords++;

						if(l==1){
							if(fd1==STDIN_FILENO){
							printf(COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
							return 0;}

							else
							{
								printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
								return 0;
							}}
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
				if(i && !w){
					if(getWordInSentence_i(file,check_word, word, count, n_line,l,r)==1)
						{nWords++;
						if(l==1)
							printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
							return 0;
						}}
				else if(w && i){
					if(getWordInSentence_w_i(file,check_word, word, count, n_line,l,r)==1)
						{nWords++;
						if(l==1)
							printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
							return 0;
						}}
				else if(w && !i){
					if(getWordInSentence_w(file,check_word, word, count, n_line,l,r)==1)
						{nWords++;
						if(l==1)
							printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
							return 0;
						}}
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

int getWordInSentence_i(char* file, char* sentence, char* word, int notToShow, int nl,int l, int r)
{
	int i = 0, j = 0;
	int number = 0, found = 0;
	char* check_word = malloc(strlen(word)*sizeof(char));
	while (i < strlen(sentence) -strlen(word)) {
		strncpy(check_word, sentence + i, strlen(word));
		int equals = 1;
		for (int m = 0; m < strlen(word); m++) {
			if (check_word[m] != word[m] && (int) check_word[m] != (int) word[m] + 32
					&& (int) check_word[m] != (int) word[m] - 32)
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

int getWordInSentence_w(char* file, char* sentence, char* word, int notToShow, int nl, int l, int r)
{
	int i = 0, j = 0;
	int number = 0, found = 0;
	char space=' ';

	char* check_word = malloc(strlen(word)*sizeof(char));

	if(strlen(sentence) == strlen(word)+1)
		{
		int equals = 1;
		strncpy(check_word, sentence + i, strlen(word));
		for (int m = 0; m < strlen(word); m++) {
			if (*(check_word + m) != *(word + m))
				equals = 0;
		}

		if(equals==1){
			if(l==1)
				return 1;
			if(r != 0)
				printf(COLOR_MAGENTA "%s" COLOR_CYAN ":" RESET_COLOR, file);
			if(nl!=0)
				printf(COLOR_GREEN "%d" COLOR_CYAN ":" RESET_COLOR, nl);
			printf(COLOR_RED "%s" RESET_COLOR "\n", word);
			return 1;
		}
		return 0;}
	
		
	while (i < strlen(sentence) -strlen(word)-1) {
		int equals = 1;

		if (i == 0) {
			strncpy(check_word, sentence + i, strlen(word) + 1);

			for (int m = 0; m < strlen(word) + 1; m++) {

				if (m == strlen(word)) {
					if ((*(check_word + m)) != space) {
						if (ispunct(*(check_word + m)) == 0) {
							equals = 0;
							break;
						}
					}
				}
				else if (*(check_word + m) != *(word + m)) {
					equals = 0;
					break;
				}
			}

		}
		else if(i==strlen(sentence) -strlen(word)-2)
		{
			strncpy(check_word, sentence + i, strlen(word)+1);
			
		for (int m = 0; m < strlen(word)+1; m++) {
			if (m==0){
			
				
				if((*check_word)!=space && ispunct(*(check_word)) == 0){
			
				equals=0;
				break;}
				}
					
			else if (*(check_word + m) != *(word + m-1))
				{				
				equals = 0;
				break;
				}	
			
		}
		}
		else {
		strncpy(check_word, sentence + i, strlen(word)+1+1);
		

		for (int m = 0; m < strlen(word)+2; m++) {
			if (m==0){
				
				if((*check_word)!=space){
				equals=0;
				break;}
				}

			else if(m== strlen(word)+1){
				if(((*(check_word+m))!=space)){
				if(ispunct(*(check_word+m))==0){
				equals=0;
				break;
					}}
			}
				
			else if (*(check_word + m) != *(word + m-1))
				{				
				equals = 0;
				break;
				}		
			
		}
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
			if(i!=0)
				printf("%c", *check_word); //changed here

			printf(COLOR_RED "%s" RESET_COLOR, word);
			
			if(i!=strlen(sentence) -strlen(word)-2)
			printf("%c",*(check_word+strlen(check_word)-1));

			if(i==0 || (i==strlen(sentence) -strlen(word)-2))
			j = i + strlen(word)+1;
			else			
			j = i + strlen(word)+2;
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

int getWordInSentence_w_i(char* file, char* sentence, char* word, int notToShow, int nl, int l, int r)
{
	int i = 0, j = 0;
	int number = 0, found = 0;
	char space = ' ';

	char* check_word = malloc(strlen(word) * sizeof(char));

	if (strlen(sentence) == strlen(word) + 1) {
		int equals = 1;
		strncpy(check_word, sentence + i, strlen(word));
		for (int m = 0; m < strlen(word); m++) {
			if (check_word[m] != word[m] && (int) check_word[m] != (int) word[m] + 32
					&& (int) check_word[m] != (int) word[m] - 32)
				equals = 0;
		}

		if (equals == 1) {
			if (l == 1)
				return 1;
			if (r != 0)
				printf(COLOR_MAGENTA "%s" COLOR_CYAN ":" RESET_COLOR, file);
			if(nl != 0)
				printf(COLOR_GREEN "%d" COLOR_CYAN ":" RESET_COLOR, nl);
			printf(COLOR_RED "%s" RESET_COLOR "\n", check_word);
			return 1;
		}
		return 0;
	}

	while (i < strlen(sentence) - strlen(word) - 1) {
		int equals = 1;

		if (i == 0) {
			strncpy(check_word, sentence + i, strlen(word) + 1);

			for (int m = 0; m < strlen(word) + 1; m++) {

				if (m == strlen(word)) {

					if (((*(check_word + m)) != space)) {

						if (ispunct(*(check_word + m)) == 0) {
							equals = 0;
							break;
						}
					}
				}

				else if (check_word[m] != word[m]
						&& (int) check_word[m] != (int) word[m] + 32
						&& (int) check_word[m] != (int) word[m] - 32) {
					equals = 0;
					break;
				}
			}

		} else if (i == strlen(sentence) - strlen(word) - 2) {

			strncpy(check_word, sentence + i, strlen(word) + 1);

			for (int m = 0; m < strlen(word) + 1; m++) {
				if (m == 0) {

					if ((*check_word) != space) {

						equals = 0;
						break;
					}
				}

				else if (check_word[m] != word[m-1] && (int) check_word[m] != (int) word[m-1] + 32
						&& (int) check_word[m] != (int) word[m-1] - 32) {
					equals = 0;
					break;
				}

			}
		}
		else {
			strncpy(check_word, sentence + i, strlen(word) + 1 + 1);

			for (int m = 0; m < strlen(word) + 2; m++) {
				if (m == 0) {

					if ((*check_word) != space) {
						equals = 0;
						break;
					}
				}

				else if (m == strlen(word) + 1) {
					if (((*(check_word + m)) != space)) {
						if (ispunct(*(check_word + m)) == 0) {
							equals = 0;
							break;
						}
					}
				}

				else if (check_word[m] != word[m-1] && (int) check_word[m] != (int) word[m-1] + 32
						&& (int) check_word[m] != (int) word[m-1] - 32) {
					equals = 0;
					break;
				}

			}
		}
		if (equals == 1) {
			number++;

			if (found == 0) {
				found = 1;
				if (l == 1)
					return 1;
			} else if (found == 1)
				found = 2;

			if (notToShow)
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
			if (i != 0)
				printf("%c", *check_word); //changed here

			char* str = malloc(strlen(word));
			if(strlen(check_word) == strlen(word) + 1 && i == 0)
				strncpy(str, check_word, strlen(word));
			else
				strncpy(str, check_word+1, strlen(word));
			printf(COLOR_RED "%s" RESET_COLOR, str);

			if (i != strlen(sentence) - strlen(word) - 2)
				printf("%c", *(check_word + strlen(check_word) - 1));

			if (i == 0 || (i == strlen(sentence) - strlen(word) - 2))
				j = i + strlen(word) + 1;
			else
				j = i + strlen(word) + 2;
		}
		i++;
	}
	if (number > 0) {
		if (notToShow)
			return 1;
		char* to_show = malloc(BUFFER_SIZE);
		strncpy(to_show, sentence + j, strlen(sentence) - j);
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
    if(strlen(name)>=2)
    	strncpy(two_dots, name + strlen(name)-2, 2);
    status = stat(name, &st_buf);


    if(status != 0)
    {
       // printf("Error in opening file/directory:\n", name);
        return -1;
    }

    if(!strcmp(dot, ".") || (strlen(name)>=2 && !strcmp(two_dots, "..")) )
        return 2;

    if(S_ISREG(st_buf.st_mode))
    {
    	char* txt = malloc(BUFFER_SIZE), *c = malloc(BUFFER_SIZE);
    	if(strlen(name)>=4)
    	{
    		strncpy(txt, name + strlen(name)-4, 4);
    		if(!strcmp(txt,".txt"))
    			return 0;
    	}
    	if(strlen(name)>=2)
    	{
    		strncpy(c, name + strlen(name) - 2, 2);
    		if(!strcmp(c,".c"))
    		    return 0;
    	}
    	return 2;
    }


    if(S_ISDIR(st_buf.st_mode))
        return 1;

    return -1;
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
                    return 0;

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
