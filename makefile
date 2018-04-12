#Makefile

simgrep: simgrep.c
	cc -Wall -o simgrep simgrep.c

clean:
	rm -f simgrep