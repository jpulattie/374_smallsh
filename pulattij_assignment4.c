/**
 * A sample program for parsing a command line. If you find it useful,
 * feel free to adapt this code for Assignment 4.
 * Do fix memory leaks and any additional issues you find.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_LENGTH 2048
#define MAX_ARGS		 512

int running = 0;
struct command_line
{
	char *argv[MAX_ARGS + 1];
	int argc;
	char *input_file;
	char *output_file;
	bool is_bg;
};


struct command_line *parse_input()
{
	char input[INPUT_LENGTH];
	struct command_line *curr_command = (struct command_line *) calloc(1, sizeof(struct command_line));
	int command_count = 0;
	int hashtags = 0;
	// Get input
	printf(": ");
	fflush(stdout);
	fgets(input, INPUT_LENGTH, stdin);

	// Tokenize the input
	char *token = strtok(input, " \n");
	while(token){
		//printf("token top of while loop: %s", token);
		if (token[0] == '#' && command_count == 0) {
			//printf("found comment: %s\n", token);
			while (token != NULL && (strcmp(token, "\n") != 0)) {
				token = strtok(NULL, "\n");
			}
		} else if (token[0] != '#' || command_count != 0) {
			//printf("token: %s\n", token);
			command_count++;
			if (strcmp(token, "exit")==0){
			//printf("exit indicator: %d",(strcmp(token, "exit")==0));
			running = 1;
			break;}
		} else if (strcmp(token, "cd")) {
			// getenv('HOME')			
			// open that directory 
			// handle one more token after cd?  
			// maybe make this a separate function?????
		}
		else if(!strcmp(token,"<")){
			curr_command->input_file = strdup(strtok(NULL," \n"));
			command_count++;
			
		} else if(!strcmp(token,">")){
			curr_command->output_file = strdup(strtok(NULL," \n"));
			command_count++;
			
		} else if(!strcmp(token,"&")){
			curr_command->is_bg = true;
			command_count++;
			
		} else {
			curr_command->argv[curr_command->argc++] = strdup(token);
			command_count++;
		}
		token=strtok(NULL," \n");
		//printf("running number: %d\n", running);
		//printf("Command Count: %d\n", command_count);
	}
	return curr_command;
}

int main()
{
	struct command_line *curr_command;

	while(running != 1)
	{
		//printf("running # in main: %d\n", running );
		curr_command = parse_input();

	}
	return EXIT_SUCCESS;
}