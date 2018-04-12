#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define BUFFER_SIZE 512
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_MAGENTA "\x1B[35m"
#define RESET_COLOR   "\x1b[0m"


static int cont = 0;

int getWordInSentence(char* sentence, char* word, int notToShow, int nl,int l);
int getWordInSentence_w(char* sentence, char* word, int notToShow, int nl,int l);
int getWordInSentence_i(char* sentence, char* word, int notToShow, int nl,int l);

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
			exit(2);
		if(answer != 'N' && answer != 'n')
			write(STDOUT_FILENO, "\nWrong answer. Try again.",25);
	} while(answer != 'N');
	cont = 1;
}

int main(int argc, char *argv[]) {
	int fd1, n_read, fromKeyboard = 0, indexPat = argc - 1;
	int c = 0, i = 0, n = 0, w = 0, l = 0, r = 0; //to say if -c, -i, -n, -w, -l, -r is called

	if(argc < 2)
	{
		printf("Usage: simgrep [OPTION]... PATTERN [FILE]...\n");
		return 1;
	}

	char* a = malloc(BUFFER_SIZE);

	if(strlen(argv[argc-1]) > 4)
	{
		strncpy(a, argv[argc-1] + strlen(argv[argc-1]) - 4, 4);
		if(a[0] != '.' || a[1] != 't' || a[2] != 'x' || a[3] != 't')
			fromKeyboard = 1;
	}
	if(fromKeyboard || strlen(argv[argc-1]) <= 4)
	{
		fromKeyboard == 1;
		fd1 = STDIN_FILENO;
	} else {
		fd1 = open(argv[argc-1], O_RDONLY);
		if (fd1 == -1) {
			perror(argv[argc-1]);
			return 2;
		}
		indexPat = argc-2;
	}

	for (int j = 1; j < indexPat; j++) {
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

	struct sigaction action;
	action.sa_handler = sigint_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	if (sigaction(SIGINT, &action, NULL) < 0) {
		fprintf(stderr, "Unable to install SIGINT handler\n");
		exit(1);
	}

	goingon: reading(argv[argc-1],fd1, argv[indexPat], c, i, n,w, l);

	if(cont)
	{
		cont = 0;
		goto goingon;
	}

	close(fd1);
	return 0;
}

int reading(char* file,int fd1, char* word, int count, int i, int n, int w,int l)
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
					if(getWordInSentence_i(check_word, word, count, n_line,l)==1)
						{nWords++;
						
						if(l==1)
							if(fd1==STDIN_FILENO){
							printf(COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
							return 0;}
								
							else printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
						}
				}
				else if (i && w){
					if(getWordInSentence_w_i(check_word, word, count, n_line,l)==1)
						{nWords++;

						if(l==1)
							if(fd1==STDIN_FILENO){
							printf(COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
							return 0;}

							else printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
						}
				}
				else if(w){
					if(getWordInSentence_w(check_word, word, count, n_line,l)==1)
						{nWords++;
						
						if(l==1)
							if(fd1==STDIN_FILENO){
							printf(COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
								return 0;}
							else printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
						}
				}
				else{
					
					if(getWordInSentence(check_word, word, count, n_line,l)==1)
						{nWords++;
						
						if(l==1)
							if(fd1==STDIN_FILENO){
							printf(COLOR_MAGENTA "(standard input)" RESET_COLOR "\n");
							return 0;}
								
							else printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
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
					if(getWordInSentence_i(check_word, word, count, n_line,l)==1)
						{nWords++;
						if(l==1)
							printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
							return 0;
						}
				} else if (i && w) {
					if (getWordInSentence_w_i(check_word, word, count, n_line, l)== 1) {
						nWords++;
						if (l == 1)
							printf(COLOR_MAGENTA "%s" RESET_COLOR "\n", file);
						return 0;
					}
				}
				else if(w){
					if(getWordInSentence_w(check_word, word, count, n_line,l)==1)
						{nWords++;
						if(l==1)
							printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
							return 0;
						}}
				else
					
					if(getWordInSentence(check_word, word, count, n_line,l)==1)
						{nWords++;
						if(l==1)
							printf(COLOR_MAGENTA "%s" RESET_COLOR "\n",file);
							return 0;
						}
			}
	}
	if(count)
		printf("%d\n", nWords);
	return 0;
}
//normal
int getWordInSentence(char* sentence, char* word, int notToShow, int nl, int l)
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
				if(found == 1 && nl != 0)
					printf(COLOR_GREEN "%d:" RESET_COLOR, nl);
				printf("%s", to_show);
			}
			if(i == 0 && found == 1 && nl != 0)
				printf(COLOR_GREEN "%d:" RESET_COLOR, nl);
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

int getWordInSentence_i(char* sentence, char* word, int notToShow, int nl,int l)
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
				if(found == 1 && nl != 0)
					printf(COLOR_GREEN "%d:" RESET_COLOR, nl);
				printf("%s", to_show);
			}
			if(i == 0 && found == 1 && nl != 0)
				printf(COLOR_GREEN "%d:" RESET_COLOR, nl);
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

int getWordInSentence_w(char* sentence, char* word, int notToShow, int nl, int l)
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
				if(nl!=0)
			printf(COLOR_GREEN "%d:" RESET_COLOR, nl);
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
				if(found == 1 && nl != 0)
					printf(COLOR_GREEN "%d:" RESET_COLOR, nl);
				printf("%s", to_show);
			}
			if(i == 0 && found == 1 && nl != 0)
				printf(COLOR_GREEN "%d:" RESET_COLOR, nl);
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

int getWordInSentence_w_i(char* sentence, char* word, int notToShow, int nl, int l)
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
			if (nl != 0)
				printf(COLOR_GREEN "%d:" RESET_COLOR, nl);
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
				if (found == 1 && nl != 0)
					printf(COLOR_GREEN "%d:" RESET_COLOR, nl);
				printf("%s", to_show);
			}
			if (i == 0 && found == 1 && nl != 0)
				printf(COLOR_GREEN "%d:" RESET_COLOR, nl);
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

