/**
 * A sample program for parsing a command line. If you find it useful,
 * feel free to adapt this code for Assignment 4.
 * Do fix memory leaks and any additional issues you find.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define INPUT_LENGTH 2048
#define MAX_ARGS		 512

char executable[INPUT_LENGTH + 1];
int running = 0;
struct command_line
{
	char *argv[MAX_ARGS + 1];
	int argc;
	char *input_file;
	char *output_file;
	bool is_bg;
};

int cd (char *directory){
	char current_dir[256];
	int size = 256;
	getcwd(current_dir, sizeof(current_dir));
	printf("current directory after check: %s\n", current_dir);
	if (directory == NULL) {
		directory = getenv("HOME");
	}
	if (strcmp(directory, current_dir) !=0) {
		printf("DIFFERENT DIRECTORY INDICATED\n");
		printf("directory: %s\n", directory);
		printf("current home: %s\n", current_dir);
		if (chdir(directory) == 0) {
			printf("Changed directory to: %s\n", directory);
    		} else {
        		perror("chdir failed");
    		};
		printf("checking...\n");
		getcwd(current_dir, sizeof(current_dir));
		printf("current directory after check: %s\n", current_dir);
	}
	printf("Directory: %s\n", directory);
	// do the change directory stuffs up here
	return 0;
}

int execute(char *ex){
	printf("executable command: %s", ex);
	return 0;
}

struct command_line *parse_input()
{
	char input[INPUT_LENGTH];
	struct command_line *curr_command = (struct command_line *) calloc(1, sizeof(struct command_line));
	int command_count = 0;
	char get_direc[256];
	int hashtags = 0;
	char *cd_home;
	int loop = 1;
	int breaker = 0;
	// Get input
	printf(": ");
	fflush(stdout);
	fgets(input, INPUT_LENGTH, stdin);

	// Tokenize the input
	char *token = strtok(input, " \n");
	char *directory = NULL;
	while(token && breaker ==0){
		printf("token top of while loop: %s\n", token);
		if (token[0] == '#' && command_count == 0) {
			//printf("found comment: %s\n", token);
			while (token != NULL && (strcmp(token, "\n") != 0)) {
				token = strtok(NULL, "\n");
			}
		} else if ((token[0] != '#' || command_count != 0) && (strcmp(token, "exit")==0)) {
			//printf("token: %s\n", token);
			command_count++;
			//printf("exit indicator: %d\n",(strcmp(token, "exit")==0));
			running = 1;
			printf("EXITING\n");
			breaker =1;
			break;
		} else if (strcmp(token, "cd") == 0) {
			//printf("CATCH HERE\n");
			//cd_home = getenv("HOME");
			//getcwd(get_direc, sizeof(get_direc));
			//printf("Before cd runs, current working directory is: %s\n", get_direc);
			//printf("cd is: %s\n", cd_home);
			token = strtok(NULL, " \n");

			while (token != NULL && (strcmp(token, "\n") != 0)) {
				//printf("Loop %d start\n", loop);
				//printf("token after CD: %s\n", token);
				directory = (char *)malloc(strlen(token) + 1);
				directory = token;
				//printf("directory string: %s\n", directory);
				token = strtok(NULL, " \n");
				//printf("loop %d end\n", loop);
			}
			cd(directory);
			breaker = 1;
			break;
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
			if (strlen(executable) == 0){
				printf("blank executable\n");
				strcat(executable, strdup(token)); 
			} else if (strlen(executable) !=0 ) {
				strcat(executable, " "); 
				strcat(executable, strdup(token)); 
			}
			command_count++;
		}
		token=strtok(NULL," \n");
		//printf("running number: %d\n", running);
		//printf("Command Count: %d\n", command_count);
	}
	printf("ready to execute -%s-", executable);

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