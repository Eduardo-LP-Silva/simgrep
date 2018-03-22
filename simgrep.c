#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[]) {
	int fd1, n_read;
	unsigned char buffer[BUFFER_SIZE];

	if(argc < 2)
	{
		printf("number of arguments invalid\n");
		return 1;
	}
	if(argc == 2)
	{
		fd1 = STDIN_FILENO;
	} else {
		fd1 = open(argv[2], O_RDONLY);
		if (fd1 == -1) {
			perror(argv[2]);
			return 2;
		}
	}
	char *check_word = malloc(strlen(argv[1]) * sizeof(char));

	while ((n_read = read(fd1, buffer, BUFFER_SIZE)) > 0) {
		int i = 0, j = 0, k = 0;

		//gets sentence until \n: j->where the sentence starts; k->where it ends
		while (i < n_read) {
			while (buffer[i] != '\n' && i != n_read)
				i++;
			j = k;
			k = i;
			i = j;

			//checks if sentence contains the wanted word and shows sentence if yes
			while ((i < k - strlen(argv[1])) && (k-j >= strlen(argv[1]))) {
				strncpy(check_word, buffer + i, strlen(argv[1]));
				int equals = 1;
				for (int m = 0; m < strlen(argv[1]); m++) {
					if (*(check_word + m) != *(argv[1] + m))
						equals = 0;
				}
				if (equals == 1) {
					char* to_show = malloc(BUFFER_SIZE);
					strncpy(to_show, buffer + j, k-j);
					write(STDOUT_FILENO, to_show, k-j);
					if(argc == 2)
						write(STDOUT_FILENO, "\n", 1);
					break;
				}
				i++;
			}
			i = k+1;
		}
	}
	write(STDOUT_FILENO, "\n", 1);

	close(fd1);
	return 0;
}
