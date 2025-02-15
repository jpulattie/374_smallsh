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
#define MAX_ARGS 512

int opened_new_out = 0;
int opened_new_in = 0;
int new_out;
int new_in;
int exit_status;
int running = 0;
struct command_line
{
	char *argv[MAX_ARGS + 1];
	int argc;
	char *input_file;
	char *output_file;
	bool is_bg;
	bool is_internal;
};

int cd(char *directory)
{
	char current_dir[256];
	int size = 256;
	getcwd(current_dir, sizeof(current_dir));
	// printf("current directory after check: %s\n", current_dir);
	if (directory == NULL)
	{
		directory = getenv("HOME");
	}
	if (strcmp(directory, current_dir) != 0)
	{
		// printf("DIFFERENT DIRECTORY INDICATED\n");
		// printf("directory: %s\n", directory);
		// printf("current home: %s\n", current_dir);
		if (chdir(directory) == 0)
		{
			// printf("Changed directory to: %s\n", directory);
		}
		else
		{
			perror("chdir failed");
		};
		// printf("checking...\n");
		getcwd(current_dir, sizeof(current_dir));
		// printf("current directory after check: %s\n", current_dir);
	}
	// printf("Directory: %s\n", directory);
	//  do the change directory stuffs up here
	return 0;
}

int status()
{

	printf("exit value %d\n", exit_status);
}

int execute(struct command_line *ex)
{
	int og_stdin = dup(0);
	int og_stdout = dup(1);
	pid_t spawnpid = -5;
	int childStatus;
	int childPid;
	char *input_file_name = ex->input_file;
	char *output_file_name = ex->output_file;

	spawnpid = fork();

	switch (spawnpid)
	{
	case -1:
		perror("fork() failed!");
		exit_status = 1;
		exit(1);
		break;
	case 0:

	/// NEED IF THENS TO CATCH IF THERE IS AN INPUT,OUTPUT, OR BOTH AND THEN EXECUTE ACCORDINGLY!!!!


		printf("I am the child. My pid = %d Going to sleep now!\n", getpid());
		printf("%s is executing\n", ex->argv[0]);
		int open_new_input = open(input_file_name, O_RDONLY);
		//int open_new_output = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);

		printf("input file name: %s\n", input_file_name);
		//printf("output file name: %s\n", output_file_name);
		printf("checking file opened: %d\n", open_new_input);
		/*
			//output_file_name = (char *)malloc(strlen(curr_command->output_file) + 2);
			// strcat(output_file_name, "\"");
			//strcat(output_file_name, strdup(curr_command->output_file));
			// strcat(output_file_name, "\"");
			// 
			
*/
			if (open_new_input == -1)
			{
				perror("error opening file:");
				exit_status = 1;
				break;
			}
			//if (open_new_output == -1)
			//{
			//	perror("error opening file:");
			//	exit_status = 1;
			//	break;
			//}
			else {
				opened_new_in = 1;
				new_in = dup2(open_new_input, 0);
				printf("opened new input at: %s\n", input_file_name);
				//opened_new_out = 1;
				//new_out = dup2(open_new_output, 1);
				//printf("opened new output at: %s\n", output_file_name);
			}
		execvp(ex->argv[0], ex->argv);

	default:
		printf("I am the parent. My pid = %d\n", getpid());
		childPid = waitpid(spawnpid, &childStatus, 0);
		if (WIFEXITED(childStatus))
		{
			exit_status = WEXITSTATUS(childStatus);
		}
		printf("%s was executed\n", ex->argv[0]);
		printf("input/output tracking = %d\n", opened_new_in);
		if (opened_new_in == 1)
		{

			close(new_in);
			opened_new_in = 0;
			dup2(og_stdin, 0);
			printf("input/output tracking AFTER CLOSE = %d\n", opened_new_in);
		}
		if (opened_new_out == 1)
		{

			close(new_out);
			opened_new_out = 0;
			dup2(og_stdout, 1);
		}
		// close(new_out);
		// close(new_in);
		// perror("process failed");
		// exit_status = 1;
		// printf("exit status: %d\n", exit_status);

		printf("Parent's waiting is done as the child with pid %d exited\n", childPid);
		break; //
	}
	return 0;
	
}

struct command_line *parse_input()
{
	char input[INPUT_LENGTH];
	struct command_line *curr_command = (struct command_line *)calloc(1, sizeof(struct command_line));
	int command_count = 0;
	char get_direc[256];
	int iterations = 0;
	char *cd_home;
	int loop = 1;
	int breaker = 0;

	//printf("SHOULD BE BLANK COMMAND:\n. first arg: %s\n. arg count:%d\n. in: %s\n. out: %s\n", curr_command->argv[0], curr_command->argc, curr_command->input_file, curr_command->output_file);
	printf("\nINPUT: -%s-\n", input);

	// Get input
	printf(": ");
	fflush(stdout);
	fgets(input, INPUT_LENGTH, stdin);
	//printf("\nNEW INPUT: -%s-\n", input);
	// Tokenize the input
	char *token = strtok(input, " \n");
	char *directory = NULL;
	
	while (token && breaker == 0)
	{
		// printf("ITERATIONS: %d\n", iterations);
		iterations = iterations + 1;
		printf("token top of while loop: -%s-\n", token);
		if (token[0] == '#' && command_count == 0)
		{
			// printf("found comment: %s\n", token);
			while (token != NULL && (strcmp(token, "\n") != 0))
			{
				token = strtok(NULL, "\n");
			}
		}
		else if ((token[0] != '#' || command_count != 0) && (strcmp(token, "exit") == 0))
		{
			// printf("token: %s\n", token);
			command_count++;
			curr_command->is_internal = true;
			printf("exit indicator: %d\n",(strcmp(token, "exit")==0));
			running = 1;
			printf("EXITING\n");
			breaker = 1;
			break;
		}
		else if (strcmp(token, "cd") == 0)
		{
			// printf("CATCH HERE\n");
			// cd_home = getenv("HOME");
			// getcwd(get_direc, sizeof(get_direc));
			// printf("Before cd runs, current working directory is: %s\n", get_direc);
			// printf("cd is: %s\n", cd_home);
			curr_command->is_internal = true;
			token = strtok(NULL, " \n");

			while (token != NULL && (strcmp(token, "\n") != 0))
			{
				// printf("Loop %d start\n", loop);
				// printf("token after CD: %s\n", token);
				directory = (char *)malloc(strlen(token) + 1);
				directory = token;
				// printf("directory string: %s\n", directory);
				token = strtok(NULL, " \n");
				// printf("loop %d end\n", loop);
			}
			cd(directory);
			breaker = 1;
			break;
			// getenv('HOME')
			// open that directory
			// handle one more token after cd?
			// maybe make this a separate function?????
		}
		else if (strcmp(token, "status") == 0)
		{
			curr_command->is_internal = true;
			command_count++;
			breaker = 1;
			status();
			break;
		}
		else if (!strcmp(token, "<"))
		{
			curr_command->input_file = strdup(strtok(NULL, " \n"));
			token = strtok(NULL, " \n");
			printf("command input catch: %s\n", curr_command->input_file);
			command_count++;
		}
		else if (!strcmp(token, ">"))
		{
			curr_command->output_file = strdup(strtok(NULL, " \n"));
			printf("command output catch: %s\n", curr_command->output_file);
			token = strtok(NULL, " \n");
			
			command_count++;
			printf("\nend of output chunk\n");
		}
		else if (!strcmp(token, "&"))
		{
			curr_command->is_bg = true;
			command_count++;
		}
		else
		{
			curr_command->argv[curr_command->argc++] = strdup(token);
			command_count++;
		}

		token = strtok(NULL, " \n");



		// printf("running number: %d\n", running);
		// printf("Command Count: %d\n", command_count);
	}


	printf("ready to execute -%s-\n", curr_command->argv[0]);
	//printf("curr input: %s | curr output: %s\n", input_file_name, output_file_name);
	printf("catching a blank area\n");
	if (curr_command->argc > 0 && curr_command->is_internal != true)
	{
		printf("executing\n");
		execute(curr_command);
		printf("back from execute\n");
	}
	if (curr_command->is_internal == true)
	{
		printf("one of three built in commands\n");
	}


	// if (curr_command->argv[0] && (strcmp(curr_command->argv[0], "status") == 0))
	//{
	//	status();
	// }
	return curr_command;
}

int main()
{
	struct command_line *curr_command;
	int i = 0;

	while (running != 1)
	{
		// printf("running # in main: %d\n", running );
		curr_command = parse_input();
		printf("sent back to main\n");
		//printf("Current command:\n. first arg: %s\n. arg count:%d\n. in: %s\n. out: %s\n", curr_command->argv[0], curr_command->argc, curr_command->input_file, curr_command->output_file);
		printf("\n END OF MAIN LOOP- LOOPING AGAIN\n");
		
	}

	return EXIT_SUCCESS;
}