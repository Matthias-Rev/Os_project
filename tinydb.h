
	//function protoypes of tinydb

void action(int sig);
void handler(int sig);
void handler2(int sig);
void no_sigint(int i);
void no_sigusr1(int i);
void no_sigusr2(int i);
void child1(int read_fd);
void child2(int read_fd);
void child3(int read_fd);
void child4(int read_fd);
void parent(int write_fd, int write_fd2, int write_fd3, int write_fd4);

