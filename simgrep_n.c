#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 512
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN  "\x1B[32m"
#define RESET_COLOR   "\x1b[0m"

int getNWordsInSentence(char* sentence, char* word, int nl);

int main(int argc, char *argv[]) 
{
	int fd1, n_read;

	if(argc < 2)
	{
		printf("number of arguments invalid\n");
		return 1;
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
	int m = 0, n_line = 1;
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
				getNWordsInSentence(check_word, argv[1], n_line);

            n_line++;
		}
	}

	if (fd1 != STDIN_FILENO) 
    {
		char *check_word = malloc(m * sizeof(char));

		strncpy(check_word, buffer, m);
		m = 0;

		if (strlen(check_word) >= strlen(argv[1]))
			getNWordsInSentence(check_word, argv[1], n_line);
	} 

	close(fd1);

	return 0;
}

int getNWordsInSentence(char* sentence, char* word, int nl)
{
	int i = 0, j = 0;
	int number = 0;
	char* check_word = malloc(strlen(word)*sizeof(char));

	while (i < strlen(sentence) -strlen(word)) 
    {
		strncpy(check_word, sentence + i, strlen(word));

		int equals = 1;
        int found = 0;

		for (int m = 0; m < strlen(word); m++) 
        {
			if (*(check_word + m) != *(word + m))
				{
                    equals = 0;
		}       }

		if (equals == 1) 
        {
			number++;

            if(found == 0)
                found = 1;
            else
                if(found == 1)
                    found = 2;

			if (i != j) 
            {
				char* to_show = malloc(BUFFER_SIZE);
				strncpy(to_show, sentence + j, i - j);

                if(found == 1)
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
		char* to_show = malloc(BUFFER_SIZE);
		strncpy(to_show, sentence + j, strlen(sentence)-j);
		printf("%s", to_show);
	}

	return number;
}
