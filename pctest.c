#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define READ 0
#define WRITE 1
#define NANO 1000000000LL

long long timeout, min_time, max_time = 0;
long long sum_time = 0;
struct timespec begin, end;
pid_t curr_target;
void target(int tar_pipe[2], char *testfile, char *targetfile) {
	close(tar_pipe[READ]);
	dup2(tar_pipe[WRITE], 1);
	char targetpath[20];
	sprintf(targetpath, "./%s", targetfile);
	int fp = open(testfile, O_RDONLY);
	dup2(fp, 0);
	execl(targetpath, targetpath,"", (char*)0x0);
}
void solution(int sol_pipe[2], char* testfile, char *solutionfile) {
	close(sol_pipe[READ]);
	dup2(sol_pipe[WRITE], 1);
	char solutionpath[20];
	sprintf(solutionpath, "./%s", solutionfile);
	int fp = open(testfile, O_RDONLY);
	dup2(fp, 0);
	execl(solutionpath, solutionpath, "", (char*)0x0);
}

void finish(int sig) {
	clock_gettime(CLOCK_MONOTONIC, &end);
}
void alarms(int sig) {
	kill(curr_target, SIGKILL);
}
int main(int argc, char*argv[]) {
	signal(SIGCHLD, finish);
	signal(SIGALRM, alarms);
	DIR* dir_info;
	struct dirent* dir_entry;

	char dirpath[20];
	sprintf(dirpath, "./%s/", argv[1]);
	dir_info = opendir(dirpath);

	char *solution_file = argv[3];
	char *target_file = argv[4];

	pid_t pid[2];
	int status;
	size_t numRead_sol;
	size_t numRead_tar;

	timeout = atoi(argv[2])*10000;
	min_time = timeout;
	long long diff;
	

	int suc_count = 0;
	int fail_count = 0;
	while(dir_entry = readdir(dir_info)) {
		char testfile[20];
		if (dir_entry->d_name[0] != 't') continue;
		sprintf(testfile, "%s%s",dirpath, dir_entry->d_name);
		int sol_pipe[2];pipe(sol_pipe);
		int tar_pipe[2];pipe(tar_pipe);
		for (int i = 0;i < 2;i++) {
			pid[i] = fork();
			pid_t waitPID;

			if (pid[i] == 0) //child, solution, target
			{

				if (i == 0) {

					solution(sol_pipe, testfile, solution_file);
				}
				else {
					curr_target = pid[i];
					alarm(atoi(argv[2]));
					target(tar_pipe, testfile, target_file);
				}
				exit(i);
			}
			if (pid[i] > 0) {
				waitPID = wait(&status);
				if (i == 0) {
					clock_gettime(CLOCK_MONOTONIC, &begin);
				}
				if (i == 1) {
					diff = NANO * (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec);
					diff /= 1000000;
					sum_time += diff;
					min_time = MIN(diff, min_time);
					max_time = MAX(diff, max_time);
					if (diff > timeout) { fail_count++; continue; } // timeout
					if (WIFSIGNALED(status)) { fail_count++; continue; } // segmantation fault, wrong termination
					close(sol_pipe[WRITE]);
					close(tar_pipe[WRITE]);

					char buf_sol[255];
					char buf_tar[255];

					numRead_sol = read(sol_pipe[READ], buf_sol, 255);
					numRead_tar = read(tar_pipe[READ], buf_tar, 255);
					if (numRead_sol != numRead_tar || strncmp(buf_sol, buf_tar, numRead_sol) != 0) {
						fail_count++;
						break;
					}
					if (strncmp(buf_sol, buf_tar, numRead_tar) == 0) {
						suc_count++;
						break;
					}
				}
			}
		}
	}
	closedir(dir_info);
	if (fail_count > 0) printf("----------FAIL---------\n");
	else printf("-------SUCCESS--------\n");
	printf("   Suceeded count: %d\n", suc_count);
	printf("   Failed count: %d\n", fail_count);
	printf("   MIN time: %lldms\n", min_time);
	printf("   MAX time: %lldms\n", max_time);
	printf("   Total time: %lldms\n", sum_time);
	printf("-----------------------\n");
	return 0;
}
