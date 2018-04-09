#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFFER_SIZE 512
#define COLOR_RED     "\x1b[31m"
#define RESET_COLOR   "\x1b[0m"

int simgrep_r(char *word, char *directory);
int isFile(char *name);
int getNWordsInSentence(char* sentence, char* word);

int main(int argc, char *argv[]) 
{
	int i, c;

	if(argc < 2)
	{
		printf("Usage: simgrep [OPTION]... PATTERN [FILE]...\n");
		return 1;
	}

    simgrep_r("teste", ".");

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

int simgrep_i(char *word, char *file) 
{
	int fd1, n_read;

	fd1 = open(file, O_RDONLY);

	if (fd1 == -1) 
    {
		perror(file);
		return 2;
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

			if(strlen(check_word) >= strlen(word))
				getNWordsInSentence(check_word, word);

            
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

	close(fd1);

	return 0;
}

int simgrep_r(char *word, char *directory) 
{
    DIR *d;
    struct dirent *dir;
    int file_type;

    d = opendir(directory);
    
    if(d)
    {
        while((dir = readdir(d)) != NULL)
        {
            file_type = isFile(dir->d_name);

            switch(file_type)
            {
                case -1:
                    printf("Error\n");
                    return -1;

                case 0:
                    printf("%s is a file\n", dir->d_name);
                    break;

                case 1:
                    printf("%s is a directory\n", dir->d_name);
                    break;

                case 2:
                    continue;
            }
        }
    }

	return 0;
}

int getNWordsInSentence(char* sentence, char* word)
{
	int i = 0, j = 0;
	int number = 0;
	char* check_word = malloc(strlen(word)*sizeof(char));

	while (i < strlen(sentence) -strlen(word)) 
    {
		strncpy(check_word, sentence + i, strlen(word));

		int equals = 1;

		for (int m = 0; m < strlen(word); m++) 
        {
			if (check_word[m] != word[m] && (int) check_word[m] != (int) word[m] + 32
                && (int) check_word[m] != (int) word[m] - 32)
				equals = 0;
		}

		if (equals == 1) 
        {
			number++;

			if (i != j) 
            {
				char* to_show = malloc(BUFFER_SIZE);
				strncpy(to_show, sentence + j, i - j);
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


/* Returns 0 if it is a file, 1 if a directory, 2 if current directory or directory above,
   -1 if an error occurred*/
int isFile(char *name)
{
    int status;
    struct stat st_buf;

    status = stat(name, &st_buf);

    if(status != 0)
    {
        printf("Error in opening file/directory\n");
        return -1;
    }
    
    if(!strcmp(name, ".") || !strcmp(name, ".."))
        return 2;

    if(S_ISREG(st_buf.st_mode))
        return 0;
        
            
    if(S_ISDIR(st_buf.st_mode))
        return 1;

    return -1;
}




