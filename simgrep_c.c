#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 512

void getNWordsInSentence(char* sentence, char* word, int nl, int *no_lines);

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
	int m = 0, n_line = 1, no_lines = 0;
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
				getNWordsInSentence(check_word, argv[1], n_line, &no_lines);

            n_line++;
		}
	}

	if (fd1 != STDIN_FILENO) 
    {
		char *check_word = malloc(m * sizeof(char));

		strncpy(check_word, buffer, m);
		m = 0;

		if (strlen(check_word) >= strlen(argv[1]))
			getNWordsInSentence(check_word, argv[1], n_line, &no_lines);
	} 

    printf("%d\n", no_lines);

	close(fd1);

	return 0;
}

void getNWordsInSentence(char* sentence, char* word, int nl, int *no_lines)
{
	int i = 0;
	int found = 0;
	char* check_word = malloc(strlen(word)*sizeof(char));

	while (i < strlen(sentence) -strlen(word)) 
    {
		strncpy(check_word, sentence + i, strlen(word));

		int equals = 1;

		for (int m = 0; m < strlen(word); m++) 
        {
			if (*(check_word + m) != *(word + m))
				{
                    equals = 0;
		}       }

		if (equals == 1) 
        {
            if(found == 0)
                found = 1;
    
			
		}

		i++;
	}

    if(found == 1)
        *no_lines = *no_lines + 1;
}
