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

int init_count=0;
int TAB[4];

int tab[4];
static int Pcount=0;
static int count=0;
static int count2=0;
const char *db_path = "../students.bin";
database_t *db = (database_t*)mmap(NULL, sizeof(database_t)*4096, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);
dock *s = (dock*)mmap(NULL, sizeof(dock), PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);


int main(int argc, char const *argv[]){

	if (argv[1]){
		db_path = argv[1];
	}

	printf("\nWelcome to the Tiny Database!\n");                //Init_Menu
	db_init(db);
	db_load(db, db_path);

	s->update = 0;
	s->deletes = 0;
	s->insert = 0;
	s->select= 0;
	
	
	int wstatus;	
	int sel_fd[2], ins_fd[2],del_fd[2], up_fd[2], select, insert, deletes, update;
	pipe(sel_fd), pipe(ins_fd), pipe(del_fd), pipe(up_fd);
	pid_t pid;
	
	//creat childs
	
	for(int i=0; i<4; i++){
		if ((pid=fork()) == -1) perror("fork()");
		else if (pid==0){
			TAB[i]=getpid();
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

			pause();
			switch (count2){	
			case 0:
				printf("case 0");
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
				sleep(2);
			case 1:
				close(sel_fd[WRITE]),close(ins_fd[WRITE]),close(del_fd[WRITE]),close(up_fd[WRITE]);
				printf("après le close");
				exit(1);
			default :
				perror("Error with var count2");
				break;
			}
		}
		//Principal process
		
		else if (pid>0){
			signal(SIGINT, handler);
			signal(SIGUSR2, handler2);
			signal(SIGUSR1, action);
			for(int i=0; i++; i<4){TAB[i]=(pid-3+i);}
			struct sigaction suser;
			suser.sa_flags = 0;
			suser.sa_handler = &action;
			sigaction(SIGUSR1, &suser, NULL);
			TAB[0]=pid-3;
			TAB[1]=pid-2;
			TAB[2]=pid-1;
			TAB[3]=pid;
			
			close(sel_fd[READ]),close(ins_fd[READ]),close(del_fd[READ]),close(up_fd[READ]);			//Close pipes
			parent(sel_fd[WRITE], ins_fd[WRITE],del_fd[WRITE],up_fd[WRITE]);
		}
	} while (Pcount<1);
	printf("c'est encore loris le problème");
	kill(pid, SIGUSR2),kill(pid-1, SIGUSR2),kill(pid-2, SIGUSR2),kill(pid-3, SIGUSR2); //Ask to childeren to finish
	printf("pas étonnant");
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
	exit(0);
}

		//Sending and Receiving signals
		
void action(int sig){
	switch (sig){
		case SIGUSR1:
		db_save(db, db_path);
	}
}

void handler(int sig){

	switch(sig){
	case SIGINT:
		printf("L'utilsateur a appuyé sur CTRL+C\n");
		int num_stop = 0;
		while(num_stop < 1){
			/*size_t i;
			char buffer[256];
			for(i=0; i<db->lsize;i++){
				cout << "stp mec\n";
				student_t *s = &db->data[i];
				student_to_str(buffer, s);
			}*/
			cout << "select value" <<s->select;
			cout << "\ncheck state" <<check(*s) << "\n";
			cout << "Committing database changes to the disk...";
			db_save(db, db_path);
			printf("On a save la db");
			if(check(*s)==true){
				cout << "jsp";
				for(int i =0;i<4 ;i++){
					
					int pid = TAB[i];
					kill(pid, SIGKILL);
				}
				num_stop++;
			}
		}
	}
}
void handler2(int sig){
	if (sig == SIGUSR2){
	}
}
		// Avoid Signals
		
void no_sigint(int i){
	printf("sigint\n");
}
void no_sigusr1(int i){
	printf("sigusr1\n");
	cout << "je suis ici7\n";
	pause();	
}
void no_sigusr2(int i){
	printf("sigusr2\n");
}
		//Child's Action

void child1(int read_fd){
	char querie[QSIZE];
	pause();
	cout << "je suis ici6\n";	
	read(read_fd,querie,sizeof(querie));
	cout << "je suis ici4\n";	
	query_select(db, querie);
	cout << "je suis ici5\n";	
	//kill(getppid(),SIGUSR2);
	s->select = 0;
}
void child2(int read_fd){
	char querie[QSIZE];
	read(read_fd,querie,sizeof(querie));
	query_insert(db, querie);
	//kill(getppid(),SIGUSR2);
	s->insert = 0;
}
void child3(int read_fd){
	char querie[QSIZE];
	read(read_fd,querie,sizeof(querie));
	query_delete(db, querie);
	//kill(getppid(),SIGUSR2);
	s->deletes = 0;
}
void child4(int read_fd){
	char querie[QSIZE];
	read(read_fd,querie,sizeof(querie));
	query_update(db, querie);
	//kill(getppid(),SIGUSR2);
	s->update = 0;
}
		//Principal process's Action

void parent(int write_fd, int write_fd2, int write_fd3, int write_fd4){
	char query_nam[QSIZE], query_opt[QSIZE];
	cout << "je suis ici1\n";	
	if (init_count>0){
		int correct_insert=0;
		while(correct_insert<1){
			cout << "je suis ici2\n";													//Check stdin's corresponding to a query 
			fgets(query_nam, QSIZE, stdin);
			cout << "je suis ici3\n";	
			//~ cout << strlen(query_nam)<<"\n";
			if ((strlen(query_nam)>1) & (query_nam[strlen(query_nam)-1]== '\n')){
				cout << "strlen(query_nam): " <<strlen(query_nam) << "\n";
				cout << "query_nam1: " <<query_nam << "\n";
				if (isatty(fileno(stdin))) query_nam[strlen(query_nam)-1] = '\0';
				cout << "query_nam2: " <<query_nam << "\n";
				char *ptr = query_nam, *token = strtok_r(NULL, " ", &ptr);
				strcpy(query_nam,token);
				cout << "query_nam3: " <<query_nam << "\n";
				cout << "ptr: " <<ptr << strlen(ptr)<< "\n";
				cout << "token: " << token << "\n";
				if ((strlen(token) != 0) & strlen(ptr)!=0){
					cout << "matthias suces\n";
					token = strtok_r(NULL, "", &ptr);
					strcpy(query_opt, token);
					cout << "query_opt: " <<query_opt<<"\n";
					token = strtok_r(NULL, "", &ptr);
					if (token == NULL){
						printf(" ");
					}
				}
				else {
					printf("Please recheck\n");
					kill(getpid(),SIGINT);
					//db_save(db, db_path);
				}
			}
			else if ((strlen(query_nam)>1)){

				Pcount++;
				char *ptr = query_nam, *token = strtok_r(NULL, " ", &ptr);
				strcpy(query_nam,token);
				cout << "query_nam3: " <<query_nam << "\n";
				cout << "ptr: " <<ptr << strlen(ptr)<< "\n";
				if ((token != NULL) & strlen(ptr)!=0){
					cout << "mtthias suce\n";
					token = strtok_r(NULL, "", &ptr);
					strcpy(query_opt, token);
					cout << "query_opt: " <<query_opt<<"\n";
					token = strtok_r(NULL, "", &ptr);
					if (token == NULL){
						printf(" ");
						correct_insert++;		
					}
					//else printf("db save 1\n"), db_save(db, db_path), kill(getpid(), SIGKILL);
				}
				//else printf("db save 2\n"), db_save(db, db_path), kill(getpid(), SIGKILL);
			}
			//else printf("Please recheck\n");
			if(strcmp(query_nam,"select")==0 || strcmp(query_nam,"insert")==0 || strcmp(query_nam,"update")==0 || strcmp(query_nam,"delete")==0)
				if(strlen(query_opt)<2) correct_insert++;
				
			//else printf("db save 3\n"), db_save(db, db_path), kill(getpid(), SIGKILL);

			
		}
		
		if (strcmp(query_nam,"select")==0){
			write(write_fd,query_opt,sizeof(query_opt));
			s->select = 1;
			cout << s->select;
		}
		if (strcmp(query_nam, "insert")==0){
			write(write_fd2,query_opt,sizeof(query_opt));
			s->insert = 1;
		}
		if (strcmp(query_nam, "delete")==0){
			write(write_fd3,query_opt,sizeof(query_opt));
			s->deletes = 1;
		}
		if (strcmp(query_nam, "update")==0){
			write(write_fd4,query_opt,sizeof(query_opt));
			s->update = 1;
		}
		for(int i=0; i<4;i++){
			int childpid;
			childpid=TAB[i];
			kill(childpid,SIGUSR2);
		}
		printf("\nP : je me mets en pause, PID =%d\n\n",getpid());
		cout << "PPPPPP : je me reveille\n";
	}
	init_count++;
}

bool check(dock s){
	cout << s.update << "u" <<"\n";
	cout << s.deletes << "d" <<"\n";
	cout << s.insert << "i" <<"\n";
	cout << s.select << "s" <<"\n";
	if (s.select ==0 & s.deletes==0 & s.update == 0 & s.insert == 0)
		return true;
	else return false;
}
