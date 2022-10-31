#include <stdio.h>
#include <string.h>
#include "db.c"
#include "student.c"


int main(int argc, char const *argv[]) {
  const char *db_path = "../students.bin";
  char querie[10];
  char *queries_l[] = {"select", "insert","delete","update","transert"};
  printf("%ld\n",sizeof(queries_l));

  // create 4 forks
  // pipe fork
  // 

  if (argv[1] == NULL)
  {
    printf("aucun communiqué");
  }
  database_t db;
  printf("\nWelcome to the Tiny Database!\n");                //Init_Menu
  db_init(&db);
  db_load(&db, db_path);
  // Il y a sans doute des choses à faire ici...
  db_save(&db, db_path);

  scanf("%s\n", querie);                                    //Enter querie
  int nbquerie;
  for (int i = 0; i < 5; i++)
  {
    printf("%s %s\n", querie, queries_l[i]);
    if(strcmp(querie, queries_l[i]) == 0)
    {
      nbquerie = i;
    }

  }
  
  switch (nbquerie)
  {
  case 0:
    // pip write on it, the information
    break;

  case 1:
    
    break;
  
  case 2:
    
    break;
  
  case 3:
    
    break;

  case 4:
    
    break;
  }

  printf("Bye bye!\n");
  return 0;
}
