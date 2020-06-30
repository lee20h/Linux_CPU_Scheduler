#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sched.h>
#include <string.h>
#include <stdint.h>
#define ROW 50
#define COL ROW
#define	SCHED_MY	7
const long NANOS = 1000000000L;
const long MICROS = 1000000L;
/*
struct timespec {
	time_t tv_sec; // seconds
	long tv_nsec; // nanoseconds
};
*/
struct sched_attr {
	uint32_t size;
	uint32_t sched_policy;
	uint64_t sched_flags;
	int32_t sched_nice;

	uint32_t sched_priority;

	uint64_t sched_runtime;
	uint64_t sched_deadline;
	uint64_t sched_period;
};
static int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags) {
	return syscall(SYS_sched_setattr, pid, attr, flags);
}


long long entire_time = 0;
int p_num;

void my_sig_handler(int signo) {
	printf("HALT !! Process Number : %d Execution time : %lld ms \n",p_num, entire_time/MICROS);
	wait(NULL);
	exit(0);
}


void time_diff(struct timespec *begin, struct timespec *mid, struct timespec *end) {
	if((mid->tv_nsec - begin->tv_nsec) < 0) {
		end -> tv_sec = mid -> tv_sec - begin -> tv_sec - 1;
		end -> tv_nsec = mid -> tv_nsec - begin -> tv_nsec + NANOS; 
	}
	else {
		end -> tv_sec = mid -> tv_sec - begin -> tv_sec;
		end -> tv_nsec = mid -> tv_nsec - begin -> tv_nsec;
	}
}

void calc(int time, int cpu) {
	struct timespec begin, mid, end;
	long p_time; // ms
	int matrixA[ROW][COL];
	int matrixB[ROW][COL];
	int matrixC[ROW][COL];

	p_num = cpu;
	
	long count = 0;
	int cpuid;
	int i, j, k;
	cpuid = cpu;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	long long save = 0;
	while(1) {
		for (i=0; i<ROW; i++) {
			for (j=0; k<COL; j++) {
				for(k=0; k<COL; k++) {
					matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
				}
			}
		}
		
		clock_gettime(CLOCK_MONOTONIC, &mid);
		time_diff(&begin,&mid,&end);

		count++;
	
		save = end.tv_sec * NANOS + end.tv_nsec;
		
		if(entire_time >= time * NANOS) break;
		if(save >= 100000000) { // 100ms
			begin = mid;
			entire_time += save;
			printf("PROCESS #%02d count = %04ld %04lld ms\n", cpuid, count, save/MICROS);
			save = 0;
		}	
	}
	printf("Done!! PROCESS #%02d : %04ld %04lld ms\n",cpuid, count, entire_time/MICROS);
}


int main(int argc, char *argv[]) {
	char* temp_cpu;
	char* temp_time;
	int i;
	int cpu, time; // 프로세스 갯수와 소요시간 
	pid_t pids[100]; // 프로세스 갯수를 100으로 가정 
	
	struct sched_attr attr; // 스케쥴링을 위한 구조체 
	memset(&attr, 0, sizeof(attr));
	attr.size = sizeof(struct sched_attr);
	
	attr.sched_policy = SCHED_MY;
	attr.sched_priority = 0;
	
	

	signal(SIGINT, (void *)my_sig_handler); // signal_handler 추가 


	for (i=1; i<=argc; i++) { // Command line arguments 변환 
		if(i==1) temp_cpu = argv[i];
		if(i==2) temp_time = argv[i];
	}
	cpu = atoi(temp_cpu);
	time = atoi(temp_time);
	
	printf("** START: Processes = %02d Time %02d s\n", cpu, time); // 시작 
	
	pid_t pid = 1;
   for (i = 0; i<cpu-1; i++) {
      printf("Creating Process: #%d\n", i);
      pids[i] = fork();
      pid = pids[i];
      if (pid < -1 ) return -1;
      else if (pid != 0) {
         //calc(time,i);
         break;
      }

   }
	int result = sched_setattr(getpid(), &attr, 0); // attr 구조체에 담아서 sched_setattr 함수에 보낸다.
   if (result == -1) perror("Error calling sched_setattr.\n"); // 에러 검출 
	sleep(i);
   if(pid==0){
   	calc(time,cpu-1);
   }
   	
   else
   	calc(time,i);
   wait(NULL);
	return 0;
}
