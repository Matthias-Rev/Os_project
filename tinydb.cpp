#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <iostream>
#include <string>
#include <assert.h>
#include <unistd.h> /* fork, write */
#include <stdbool.h> /* false */
#include <stdio.h> /* perror */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/wait.h> /* wait, sleep */
#include <sys/types.h>
#include <sys/mman.h>

#include "db.cpp"
#include "query.cpp"
#include "student.c"
#include "tinydb.h"

#define READ 0
#define WRITE 1
#define QSIZE 256

using namespace std;

static int count=0;
static int count2=0;
const char *db_path = "../students.bin";
database_t db;

int main(int argc, char const *argv[]){

	if (argv[1]){
		db_path = argv[1];
	}

	printf("\nWelcome to the Tiny Database!\n");                //Init_Menu
	db_init(&db);
	db_load(&db, db_path);
	if(mmap(&db, sizeof(student_t)*db.psize, PROT_NONE,
							MAP_SHARED,0,0)){ cout << "sucessfuly shared\n";}
	signal(SIGINT, handler);
	signal(SIGUSR2, handler2);
	signal(SIGUSR1, action);
	int wstatus;	
	int sel_fd[2], ins_fd[2],del_fd[2], up_fd[2], select, insert, deletes, update;
	pipe(sel_fd), pipe(ins_fd), pipe(del_fd), pipe(up_fd);
	pid_t pid;
	
	//creat childs
	
	for(int i=0; i<4; i++){
		if ((pid=fork()) == -1) perror("fork()");
		else if (pid==0){
			switch (i){
			case 0:
				select = getpid();
				break;
			case 1 :
				insert = getpid();
				break;
			case 2 :
				deletes = getpid();
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
	do {
		
		//Child's behavior
		
		if(pid == 0){
			
			signal(SIGINT, no_sigint),signal(SIGINT, no_sigusr1),signal(SIGUSR1, no_sigusr1);
			switch (count2){	
			case 0:
				if(getpid()==select){
					close(sel_fd[WRITE]);
					child1(sel_fd[READ]);
				}
				else if(getpid()==insert){
					close(ins_fd[WRITE]);
					child2(ins_fd[READ]);
				}
				else if(getpid()==deletes){
					close(del_fd[WRITE]);
					child3(del_fd[READ]);
				}
				else if(getpid()==update){
					close(up_fd[WRITE]);
					child4(up_fd[READ]);
				}
				else
					perror("Child Error");
				break;
			case 1:
				close(sel_fd[READ]),close(ins_fd[READ]),close(del_fd[READ]),close(up_fd[READ]);
				exit(1);
			default :
				perror("Error with var count2");
				break;
			}
		}
		//Principal process
		
		else if (pid>0){

			struct sigaction suser;
			suser.sa_flags = 0;
			suser.sa_handler = &action;
			sigaction(SIGUSR1, &suser, NULL);

			close(sel_fd[READ]),close(ins_fd[READ]),close(del_fd[READ]),close(up_fd[READ]);
			parent(sel_fd[WRITE], ins_fd[WRITE],del_fd[WRITE],up_fd[WRITE]);
			
			if(count > 0){
				kill(pid, SIGUSR2),kill(pid-1, SIGUSR2),kill(pid-2, SIGUSR2),kill(pid-3, SIGUSR2); //Ask to childeren to finish
				if(wait(&wstatus)<0)						//Wait childeren's end
					perror("waitpid\n");
				if (wait(&wstatus)<0)
					perror("waitpid\n");
				if (wait(&wstatus)<0)
					perror("waitpid\n");
				if (wait(&wstatus)<0)
					perror("waitpid\n");
					
				//db_save(&db, db_path);					//Save before finish
				printf("Data has been saved !");
				count++;
			}	
		}
	} while (count<2);
	exit(0);
}

		//Sending and Receiving signals
		
void action(int sig){
	switch (sig){
		case SIGUSR1: printf("Fct Action : Signal SIGUSR1 reçu\n db_save\n");
		//db_save(&db, db_path);
	}
}
void handler(int sig){

	switch(sig){
	case SIGINT:
		printf("\nWaiting for requests to terminate...\n");
		count++;
	}
}
void handler2(int sig){
	if (sig == SIGUSR2){
		count2++;
	}
}
		// Avoid Signals
		
void no_sigint(int i){
}
void no_sigusr1(int i){
}
void no_sigusr2(int i){
}
		//Child's Action

void child1(int read_fd){
	char querie[QSIZE];
	read(read_fd,querie,sizeof(querie));
	query_select(&db, querie);
}
void child2(int read_fd){
	char querie[QSIZE];
	read(read_fd,querie,sizeof(querie));
	query_insert(&db, querie);
}
void child3(int read_fd){
	char querie[QSIZE];
	read(read_fd,querie,sizeof(querie));
	query_delete(&db, querie);
}
void child4(int read_fd){
	char querie[QSIZE];
	read(read_fd,querie,sizeof(querie));
	query_update(&db, querie);
}
		//Principal process's Action

void parent(int write_fd, int write_fd2, int write_fd3, int write_fd4){
	char query_nam[QSIZE], query_opt[QSIZE];

	fgets(query_nam, QSIZE, stdin);
	if (isatty(fileno(stdin))){

		query_nam[strlen(query_nam)-1] = '\0';
	}
	char *ptr = query_nam;

	char* token = strtok_r(NULL, " ", &ptr);
	strcpy(query_nam,token);
    if (token == NULL) {
        printf("Error during reading");
    }
	cout << "query_nam: " <<query_nam << "\n";
	token = strtok_r(NULL, "", &ptr);
	strcpy(query_opt, token);
	cout << "query_opt: " <<query_opt<<"\n";
	token = strtok_r(NULL, "", &ptr);

    if (token == NULL)
		printf(" ");

	if (strcmp(query_nam,"select")==0){
		write(write_fd,query_opt,sizeof(query_opt));
	}
	if (strcmp(query_nam, "insert")==0){
			write(write_fd2,query_opt,sizeof(query_opt));
	}
	if (strcmp(query_nam, "deletes")==0){
		write(write_fd3,query_opt,sizeof(query_opt));
	}
	if (strcmp(query_nam, "updates")==0){
		write(write_fd4,query_opt,sizeof(query_opt));
    }
}
