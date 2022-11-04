#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h> /* false */
#include <stdio.h> /* perror */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/wait.h> /* wait, sleep */
#include <unistd.h> /* fork, write */

#define READ 0
#define WRITE 1

int count=0;

void handler(int signal);
void child1(int read_fd);
void child2(int read_fd);
void child3(int read_fd);
void child4(int read_fd);
void parent(int write_fd, int write_fd2, int write_fd3, int write_fd4);

static void action(int sig){
	switch (sig){
		case SIGUSR1: printf("Fct Action : Signal SIGUSR1 reçu\n db_save\n");
		//db_save;
	}
}

int main(){	
	int sel_fd[2], ins_fd[2],del_fd[2], up_fd[2], select, insert, delete, update;
	pipe(sel_fd);
	pipe(ins_fd);
	pipe(del_fd);
	pipe(up_fd);	
	pid_t pid;
	//creat childs
	for(int i=0; i<4; i++){
		if ((pid=fork()) == 0){
			printf("mon pid est %d\n",getpid());
			switch (i){
			case 0:
				select = getpid();
				break;
			case 1 :
				insert = getpid();
				break;
			case 2 :
				delete = getpid();
				break;
			case 3 :
				update = getpid();
				break;
			default :
				break;
			}	
			break;
		}
	}
	//~ int stat;
	do {
		
		//Principal process
		
		if(pid>0){
			
			
			struct sigaction suser;
			suser.sa_flags = 0;
			suser.sa_handler = &action;
			sigaction(SIGUSR1, &suser, NULL);
			
			signal(SIGINT, handler);
			signal(SIGUSR1, action);
			
			close(sel_fd[READ]);
			close(ins_fd[READ]);
			close(del_fd[READ]);
			close(up_fd[READ]);
			parent(sel_fd[WRITE], ins_fd[WRITE],del_fd[WRITE],up_fd[WRITE]);
			
			if(count == 1){
				printf("CTRL+C enclenché\n");
				wait(NULL);
				close(sel_fd[WRITE]);
				close(ins_fd[WRITE]);
				close(del_fd[WRITE]);
				close(up_fd[WRITE]);
			}
		}
			
		//Child's behavior
		
		else if(pid == 0 && getpid()==select){
			close(sel_fd[WRITE]);
			child1(sel_fd[READ]);
			if(count == 1){
				kill(getppid(), SIGUSR1);
				close(sel_fd[READ]);
			}
		}
		else if(pid == 0 && getpid()==insert){
			close(ins_fd[WRITE]);
			child2(ins_fd[READ]);
			if(count==1){
				kill(getppid(), SIGUSR1);
				close(ins_fd[READ]);
			}
		}
		else if(pid == 0 && getpid()==delete){
			close(del_fd[WRITE]);
			child3(del_fd[READ]);
			if(count==1){
				kill(getppid(), SIGUSR1);
				close(del_fd[READ]);
			}
		}
		else if(pid == 0 && getpid()==update){
			close(up_fd[WRITE]);
			child4(up_fd[READ]);
			kill(getppid(), SIGUSR1);
			if(count ==1){
				kill(getppid(), SIGUSR1);
				close(up_fd[READ]);
			}
		}
	} while (count<1);
	return 0;
}



		//Sending and Receiving signals
		
void handler(int signal){
	
	switch(signal){
	case SIGINT:
		printf("\nWaiting for requests to terminate...\n");
		++count;
		break;
	}
}
		//Child's Action
		
void child1(int read_fd){
	char querie[16];
	read(read_fd,querie,sizeof(querie));
	if(strcmp(querie,"select")==0)
		printf("je peux lire %s\n",querie);
}
void child2(int read_fd){
	char querie[16];
	read(read_fd,querie,sizeof(querie));
	if(strcmp(querie,"insert")==0)
		printf("je peux lire %s\n",querie);
}
void child3(int read_fd){
	char querie[16];
	read(read_fd,querie,sizeof(querie));
	if(strcmp(querie,"delete")==0)
		printf("je peux lire %s\n",querie);
}
void child4(int read_fd){
	char querie[16];
	read(read_fd,querie,sizeof(querie));
	if(strcmp(querie,"update")==0)
		printf("je peux lire %s\n",querie);
}

		//Principal process's Action
		
void parent(int write_fd, int write_fd2, int write_fd3, int write_fd4){
	char querie[16];
	printf("Entrez votre requete : \n");
	scanf("%s", querie);
	write(write_fd,querie,sizeof(querie));
	write(write_fd2,querie,sizeof(querie));
	write(write_fd3,querie,sizeof(querie));
	write(write_fd4,querie,sizeof(querie));
}



