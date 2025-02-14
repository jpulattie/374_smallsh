/**
 * A sample program for parsing a command line. If you find it useful,
 * feel free to adapt this code for Assignment 4.
 * Do fix memory leaks and any additional issues you find.
 */

#include <fcntl.h> 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define INPUT_LENGTH 2048
#define MAX_ARGS		 512

int new_out;
int new_in;
int exit_status;
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
	//printf("current directory after check: %s\n", current_dir);
	if (directory == NULL) {
		directory = getenv("HOME");
	}
	if (strcmp(directory, current_dir) !=0) {
		//printf("DIFFERENT DIRECTORY INDICATED\n");
		//printf("directory: %s\n", directory);
		//printf("current home: %s\n", current_dir);
		if (chdir(directory) == 0) {
			//printf("Changed directory to: %s\n", directory);
    		} else {
        		perror("chdir failed");
    		};
		//printf("checking...\n");
		getcwd(current_dir, sizeof(current_dir));
		//printf("current directory after check: %s\n", current_dir);
	}
	//printf("Directory: %s\n", directory);
	// do the change directory stuffs up here
	return 0;
}

int status() {
	
	printf("exit value %d\n", exit_status);
}

int execute(struct command_line *ex){
	pid_t spawnpid = -5; 
	int childStatus; 
	int childPid; 

	
	//printf("executable command function: %s\n", ex);
	spawnpid = fork();
	
	switch (spawnpid) {
		case -1:
			perror("fork() failed!"); 
			exit_status = 1;
			exit(1); 
			break; 
		case 0:
			//printf("I am the child. My pid = %d Going to sleep now!\n", getpid()); 
			execvp(ex->argv[0], ex->argv);
			close(new_out);
			close(new_in);
			perror("process failed");
			exit_status =1;
			//printf("exit status: %d\n", exit_status);
			exit(1);
			break;
		default:
			//printf("I am the parent. My pid = %d\n", getpid()); 
			childPid = waitpid(spawnpid,&childStatus, 0); 
			if(WIFEXITED(childStatus)) {
				exit_status = WEXITSTATUS(childStatus);
			}
			close(new_out);
			close(new_in);
			//printf("Exit status is now: %d\n", exit_status);
			//printf("Parent's waiting is done as the child with pid %d exited\n", childPid); 
			break;  //

	}
		//printf("here\n");
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
	char *input_file_name = NULL;
	char *output_file_name = NULL;


	while(token && breaker ==0){
		//printf("token top of while loop: %s\n", token);
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
			//printf("EXITING\n");
			executable[0] = '\0';
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
			executable[0] = '\0';
			break;
			// getenv('HOME')			
			// open that directory 
			// handle one more token after cd?  
			// maybe make this a separate function?????
		}
		else if(!strcmp(token,"<")){
			curr_command->input_file = strdup(strtok(NULL," \n"));
			//printf("command input catch: %s\n", curr_command->input_file);
			input_file_name = (char *)malloc(strlen(curr_command->input_file) + 2);
			strcat(input_file_name, "\""); 
			strcat(input_file_name, strdup(curr_command->input_file)); 
			strcat(input_file_name, "\""); 
			//printf("input file name: %s\n", input_file_name);
			int open_new_input = open(input_file_name, O_RDONLY);
			if (open_new_input == -1 ) {
				perror("error opening file:");
				exit_status = 1;
				//exit(1);
			} 
			else {
				int new_in = dup2(open_new_input, 0);
			  }
			command_count++;
			
		} else if(!strcmp(token,">")){
			curr_command->output_file = strdup(strtok(NULL," \n"));
			//printf("command output catch: %s\n", curr_command->output_file);
			output_file_name = (char *)malloc(strlen(curr_command->output_file) + 2);
			//strcat(output_file_name, "\""); 
			strcat(output_file_name, strdup(curr_command->output_file));
			//strcat(output_file_name, "\""); 
			//printf("output file name: %s\n", output_file_name);
			int open_new_output = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if (open_new_output == -1 ) {
				perror("error opening file:");
				exit_status = 1;
				//exit(1);
			} 
			else {
				int new_out = dup2(open_new_output, 1);
			  }
			command_count++;
			
		} else if(!strcmp(token,"&")){
			curr_command->is_bg = true;
			command_count++;
			
		} else {
			curr_command->argv[curr_command->argc++] = strdup(token);
			if (strlen(executable) == 0){
				//printf("blank executable\n");
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
	
	//printf("ready to execute -%s-\n", executable);
	//printf("curr input: %s | curr output: %s\n", input_file_name, output_file_name);
	if (strcmp(executable, "status") == 0) {
		status();
	} else if (strlen(executable) > 0){
	execute(curr_command);}
	//printf("resetting executable\n");
	executable[0] = '\0';
	//printf("executable is now -%s-\n", executable);
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