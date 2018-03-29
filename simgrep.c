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
#define RESET_COLOR   "\x1b[0m"

static int cont = 0;

int getWordInSentence(char* sentence, char* word, int notToShow, int nl);

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
		if (answer == 'Y')
			exit(2);
		if(answer != 'N')
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

	goingon: reading(fd1, argv[indexPat], c, i, n);

	if(cont)
	{
		cont = 0;
		goto goingon;
	}

	close(fd1);
	return 0;
}

int reading(int fd1, char* word, int count, int i, int n)
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
				if(i)
					nWords += getWordInSentence_i(check_word, word, count, n_line);
				else
					nWords += getWordInSentence(check_word, word, count, n_line);
			}
			if(n)
				n_line++;
		}
	}
	if (fd1 != STDIN_FILENO) {
		char *check_word = malloc(m * sizeof(char));
		strncpy(check_word, buffer, m);
		m = 0;
		if (strlen(check_word) >= strlen(word))
			if (i)
				nWords += getWordInSentence_i(check_word, word, count, n_line);
			else
				nWords += getWordInSentence(check_word, word, count, n_line);
	}
	if(count)
		printf("%d\n", nWords);
	return 0;
}

int getWordInSentence(char* sentence, char* word, int notToShow, int nl)
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

			if(found == 0)
				found = 1;
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

int getWordInSentence_i(char* sentence, char* word, int notToShow, int nl)
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

			if(found == 0)
				found = 1;
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
