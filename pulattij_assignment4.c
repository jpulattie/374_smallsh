/**
 * A sample program for parsing a command line. If you find it useful,
 * feel free to adapt this code for Assignment 4.
 * Do fix memory leaks and any additional issues you find.
 */
#include <signal.h>
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

pid_t parent_shell;
int current_process;
int bg_process = 0;
int fg_process = 0;
int opened_new_out = 0;
int opened_new_in = 0;
int new_out;
int new_in;
int exit_status;
int running = 0;
pid_t bg_processes[500];
int bg_process_count = 0;
pid_t fg_processes[500];
int fg_process_count;
struct command_line
{
	char *argv[MAX_ARGS + 1];
	int argc;
	char *input_file;
	char *output_file;
	bool is_bg;
	bool is_internal;
};




void handler(int sig) {
	int i;
	int bg_status;
	pid_t result;
	char* message = "terminated by signal 2\n";
	write(STDOUT_FILENO, message, 23);

/*
	printf("fg_process = %d -- pid is %d\n", fg_process, getpid());
	if (fg_process ==1){

		signal(SIGINT, SIG_DFL);
	} else {
		signal(SIGINT, SIG_IGN);
		printf("%d ignored ctrl + c", getpid());
	}
*/
}

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

int bg_check()
{
	int i;
	int bg_status;
	pid_t result;

	for (i = 0; i < bg_process_count; i++)
	{
		result = waitpid(bg_processes[i], &bg_status, WNOHANG);
		//printf("bg process result: %d\n", result);
		if (result > 0)
		{
			if (WIFEXITED(bg_status))
			{
				printf("background pid %d is done: exit value %d\n", result, exit_status);
			}
			if (WIFSIGNALED(bg_status)) {
				printf("background pid %d is done: terminated by signal %d\n", result, WTERMSIG(bg_status));
			}
		}
	}
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
	exit_status = 0;

	if (input_file_name)
	{
		input_file_name = strdup(ex->input_file);
	}
	if (output_file_name)
	{
		output_file_name = strdup(ex->output_file);
	}
	// printf("Bool BG %d", ex->is_bg);
	//  printf("input = %s || output = %s\n", input_file_name, output_file_name);

	// ex->is_bg = 1 when & is present(background), 0 when not present(foreground)
	//! input_file_name = 0 when there is an input file, 1 when there is no input
	//! output_file_name = 0 when there is an output file, 1 when there is no input

	// printf("Trying to execute %s. Exit status is %d\n", ex->argv[0], exit_status);

	if (ex->is_bg == 0)
	{
		// FOREGROUND PROCESS
		spawnpid = fork();

		switch (spawnpid)
		{
		case -1:
			perror("fork() failed!");
			exit_status = 1;
			// exit(1);
			break;
		case 0:
			//printf("I am the child. My pid = %d Going to sleep now!\n", getpid());
			//printf("Child pid? %d\n", getpid());
			current_process = getpid();
			fg_process = 1;
			//printf("current process: %d\n", current_process);
			//printf("fg process marker: %d\n", fg_process);
			
			//printf("spawn pid: %d\n", getpid());
			//printf("process count: %d\n", fg_process_count);
			// printf("%s is executing\n", ex->argv[0]);

			// printf("executable command: %s\n", ex->argv[0]);
			fflush(stdout);

			if (output_file_name)
			{
				// printf("new output\n");
				int open_new_output = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				// printf("output file name: %s\n", output_file_name);
				// printf("output file name: %s\n", output_file_name);
				// printf("checking file created/opened: %d\n", open_new_output);
				if (open_new_output == -1)
				{
					perror("error with output file:");
					exit_status = 1;
					_exit(1);
				}
				else
				{
					opened_new_out = 1;
					new_out = dup2(open_new_output, 1);
					// printf("opened new output at: %s\n", output_file_name);
				}
			}
			if (input_file_name)
			{
				// printf("new input\n");

				int open_new_input = open(input_file_name, O_RDONLY);
				// printf("input file name: %s\n", input_file_name);
				// printf("checking file opened: %d\n", open_new_input);
				if (open_new_input == -1)
				{
					printf("cannot open %s for input\n", ex->input_file);
					exit_status = 1;
					_exit(1);
					break;
					// when badfile is pushed through here it breaks
				}

				else
				{
					opened_new_in = 1;
					new_in = dup2(open_new_input, 0);
				}
			}

			execvp(ex->argv[0], ex->argv);
			// fprintf(stderr, "%s\n", ex->argv[0]);
			perror(ex->argv[0]);
			_exit(1);
			break;

		default:
			//printf("I am the parent. My pid = %d\n", getpid());
			//printf("current process: %d\n", current_process);
			//printf("fg process marker: %d\n", fg_process);
			childPid = waitpid(spawnpid, &childStatus, 0);

			if (WIFEXITED(childStatus))
			{
				exit_status = WEXITSTATUS(childStatus);
			}
			// printf("%s was executed\n", ex->argv[0]);
			// printf("input/output tracking = %d\n", opened_new_in);
			if (opened_new_in == 1)
			{

				close(new_in);
				opened_new_in = 0;
				dup2(og_stdin, 0);
				// printf("input/output tracking AFTER CLOSE = %d\n", opened_new_in);
			}
			if (opened_new_out == 1)
			{
				close(new_out);
				opened_new_out = 0;
				dup2(og_stdout, 1);
			}

			// perror("process failed");
			// exit_status = 1;
			// printf("exit status: %d\n", exit_status);

			// printf("Parent's waiting is done as the child with pid %d exited\n", childPid);
			break; //
		}
	}
	if (ex->is_bg != 0)
	{
		// BACKGROUND PROCESS

		spawnpid = fork();

		switch (spawnpid)
		{
		case -1:
			perror("fork() failed!");
			exit_status = 1;
			// exit(1);
			break;
		case 0:
			fflush(stdout);
			current_process = getpid();
			bg_process = 1;
			//printf("\nBackground pid is %d\n", getpid());
			// printf("%s is executing\n", ex->argv[0]);

			// printf("executable command: %s\n", ex->argv[0]);
			fflush(stdout);
			// printf("child started\n");
			sleep(15);
			//printf("right before process runs\n");
			if (output_file_name)
			{
				// printf("new output\n");
				int open_new_output = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				// printf("output file name: %s\n", output_file_name);
				// printf("output file name: %s\n", output_file_name);
				// printf("checking file created/opened: %d\n", open_new_output);
				if (open_new_output == -1)
				{
					perror("error with output file:");
					exit_status = 1;
					_exit(1);
				}
				else
				{
					opened_new_out = 1;
					new_out = dup2(open_new_output, 1);
					// printf("opened new output at: %s\n", output_file_name);
				}
			}
			if (input_file_name)
			{
				// printf("new input\n");

				int open_new_input = open(input_file_name, O_RDONLY);
				// printf("input file name: %s\n", input_file_name);
				// printf("checking file opened: %d\n", open_new_input);
				if (open_new_input == -1)
				{
					printf("cannot open %s for input\n", ex->input_file);
					exit_status = 1;
					//_exit(1);

					// when badfile is pushed through here it breaks
				}

				else
				{
					opened_new_in = 1;
					new_in = dup2(open_new_input, 0);
				}
			}
			if (!input_file_name != 0 && ex->is_bg != 0)
			{
				// printf("set input to dev null\n");
				int open_new_input = open("/dev/null", O_RDONLY);
				// printf("input file name: %s\n", input_file_name);
				// printf("checking file opened: %d\n", open_new_input);
				if (open_new_input == -1)
				{
					// printf("cannot open %s for input\n", ex->input_file);
					exit_status = 1;
					//_exit(1);
				}

				else
				{
					opened_new_in = 1;
					new_in = dup2(open_new_input, 0);
					// printf("opened new input at: /dev/null\n");
				}
			}
			if (!output_file_name != 0 && ex->is_bg != 0)
			{
				// printf("set output to dev null\n");
				//  printf("new output\n");
				int open_new_output = open("/dev/null", O_WRONLY);
				// printf("output file name: %s\n", output_file_name);
				// printf("output file name: %s\n", output_file_name);
				// printf("checking file created/opened: %d\n", open_new_output);
				if (open_new_output == -1)
				{
					perror("error with output file:");
					exit_status = 1;
					_exit(1);
				}
				else
				{
					opened_new_out = 1;
					// printf("opening new output at: /dev/null\n");
					new_out = dup2(open_new_output, 1);
				}
			}
			execvp(ex->argv[0], ex->argv);
			// fprintf(stderr, "%s\n", ex->argv[0]);
			perror(ex->argv[0]);
			_exit(1);
			return 0;

		default:
			//printf("I am the parent. My pid = %d\n", getpid());
			//printf("Background pid is %d\n", spawnpid);
			bg_processes[bg_process_count] = spawnpid;
			bg_process_count++;

			childPid = waitpid(spawnpid, &childStatus, WNOHANG);
			if (WIFEXITED(childStatus))
			{
				exit_status = WEXITSTATUS(childStatus);
			}
			//printf("%s was executed\n", ex->argv[0]);
			// printf("input/output tracking = %d\n", opened_new_in);
			if (opened_new_in == 1)
			{

				close(new_in);
				opened_new_in = 0;
				dup2(og_stdin, 0);
				// printf("input/output tracking AFTER CLOSE = %d\n", opened_new_in);
			}
			if (opened_new_out == 1)
			{
				close(new_out);
				opened_new_out = 0;
				dup2(og_stdout, 1);
			}

			// perror("process failed");
			// exit_status = 1;
			// printf("exit status: %d\n", exit_status);

			//printf("Parent's waiting is done, child %d is running\n", childPid);
			// printf(": \n");
			fflush(stdout);

			return 0; //
		}
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

	// printf("SHOULD BE BLANK COMMAND:\n. first arg: %s\n. arg count:%d\n. in: %s\n. out: %s\n", curr_command->argv[0], curr_command->argc, curr_command->input_file, curr_command->output_file);
	// printf("\nINPUT: -%s-\n", input);

	// Get input
	bg_check();
	//printf("before input\n");
	printf(": ");
	fgets(input, INPUT_LENGTH, stdin);
	// printf("\nNEW INPUT: -%s-\n", input);
	//  Tokenize the input
	char *token = strtok(input, " \n");
	char *directory = NULL;
	//printf("TOP OF INPUT LOOP\n");
	//printf("breaker = %d\n", breaker);
	while (token && breaker == 0)
	{
		// printf("ITERATIONS: %d\n", iterations);
		iterations = iterations + 1;
		//printf("token top of while loop: -%s-\n", token);
		//printf("exit token? %d\n",strcmp(token, "exit"));
		if (token[0] == '#' && command_count == 0)
		{
			// printf("found comment: %s\n", token);
			while (token != NULL && (strcmp(token, "\n") != 0))
			{
				token = strtok(NULL, "\n");
			}
		}
		else if (strcmp(token, "exit") == 0)
		{
			//printf("exit token: %s\n", token);
			command_count++;
			curr_command->is_internal = true;
			// printf("exit indicator: %d\n", (strcmp(token, "exit") == 0));
			running = 1;
			//printf("running = %d\n", running);
			// printf("EXITING\n");
			breaker = 1;
			//printf("breaker = %d\n", breaker);

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
			// token = strtok(NULL, " \n");
			// printf("command input catch: %s\n", curr_command->input_file);
			command_count++;
		}
		else if (!strcmp(token, ">"))
		{
			curr_command->output_file = strdup(strtok(NULL, " \n"));
			// token = strtok(NULL, " \n");
			// printf("command output catch: %s\n", curr_command->output_file);
			command_count++;
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

	if (curr_command->argc > 0 && curr_command->is_internal != true)
	{
		// printf("executing %s\n", curr_command->argv[0]);
		execute(curr_command);
		// printf("back from execute\n");
	}
	if (curr_command->is_internal == true)
	{
		//printf("one of three built in commands\n");
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

	struct sigaction SIGINT_action = {0};
	SIGINT_action.sa_handler = handler;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL);


	//signal(SIGINT, handler);
	parent_shell = getpid();
	//printf("parent shell pid: %d\n", parent_shell);
	while (running != 1)
	{
		//printf("Main 1 runnning %d\n", running);
		// printf("running # in main: %d\n", running );
		curr_command = parse_input();
		// printf("sent back to main\n");
		//  printf("Current command:\n. first arg: %s\n. arg count:%d\n. in: %s\n. out: %s\n", curr_command->argv[0], curr_command->argc, curr_command->input_file, curr_command->output_file);
		// printf("\n END OF MAIN LOOP- LOOPING AGAIN\n");
		//printf("Main 2 runnning %d\n", running);

	}

	return EXIT_SUCCESS;
}