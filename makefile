#Makefile

simgrep: simgrep.h simgrep.c
	cc -Wall -o simgrep simgrep.c

clean:
	rm -f simgrep