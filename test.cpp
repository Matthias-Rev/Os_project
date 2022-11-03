#include "student.c"
#include "query.cpp"
#include "db.cpp"
#include <iostream>
#include <string>
using namespace std;
//#define NUMBER_THREAT 1

int main(int argc, char const *argv[]) {
  const char *db_path = "../students.bin";
  //char querie[10];
  //string *queries_l[] = {"select", "insert","delete","update","transert"};

  // create 4 forks
  // pipe fork
  // 
  database_t db;
  cout << "\nWelcome to the Tiny Database!\n" << endl;                //Init_Menu
  db_init(&db);
  db_load(&db, db_path);

  //Test de insert:
  //char query[256] = "Francis Robert 101 math 11/02/1900";
  //query_insert(&db, query);
  
  //test de delete 
  //fname V | id V | lname V (works but not on all of them) | section V | birthdate V
  //char query[256] = "lname=Leclerc";
  //query_delete(&db, query);
  //db_save(&db, db_path);
  
  //test de update:
  //fname V | id V | lname V (works but not on all of them) || section V | birthdate V
  //char query[256] = "birthdate=10/08/1996 set birthdate=11/07/1999";
  //query_update(&db, query);
  
  //test de select:
  //fname V | id V | lname V (works but not on all of them) || section V | birthdate V
  char query[256] = "birthdate=10/08/1996";
  query_select(&db, query);

  char buffer[128];
  for (size_t i=0; i<db.lsize; i++){
	  student_to_str(buffer, &db.data[i]);
  }
  return 0;
}
