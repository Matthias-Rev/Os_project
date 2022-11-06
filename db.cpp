#include "db.h"
#include "student.h"
#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#include <sys/mman.h>
using namespace std;
int count1 =0;

void db_save(database_t *db, const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        perror("Could not open the DB file");
        exit(1);
    }
    if (fwrite(db->data, sizeof(student_t), db->lsize, f) < 0) {
        perror("Could not write in the DB file");
        exit(1);
    }
    fclose(f);
}


void db_load(database_t *db, const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("Could not open the DB file");
        exit(1);
    }
    student_t student;
    while (fread(&student, sizeof(student_t), 1, file)) {
		if (db->lsize <= 10){
			db_add(db, student);
		} else {break;}
	}
    fclose(file);
	cout << "Done !" << "\n" << endl;
}


void db_init(database_t *db) {
  db->lsize = 0;
  db->psize = 1000;
  //if(mmap(db->data, sizeof(student_t)*db->psize, PROT_READ | PROT_WRITE,
							//MAP_SHARED | MAP_ANONYMOUS,-1,0)){ cout << "sucessfuly shared\n";}
  db->data = (student_t*)mmap(NULL,sizeof (student_t) * db->psize, PROT_READ|PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS,-1,0);
  if(db->data == MAP_FAILED){
    printf("Mapping Failed\n");
  }
}

// même principe que pour query_result_add
void db_add(database_t *db, student_t student) {
  char buffer[128]; // permet l'écriture de l'étudiant rajouté
  db_extend(db);
  db->data[db->lsize] = student;
  student_to_str(buffer, &student);
  db->lsize++;
}

// même principe que pour query_result_extend
void db_extend(database_t *db){
	if(db->lsize >= db->psize){
		student_t* old_values = db->data;
		size_t old_size = db->psize;
		db->psize = db->psize * 2;
        db->data = (student_t *) mmap(NULL,sizeof (student_t) * db->psize, PROT_READ|PROT_WRITE,MAP_SHARED,-1,0);
		memcpy(db->data, old_values, old_size* sizeof(student_t));
        free(old_values);
	}
}

//permet de retier un étudiant de la database
bool db_remove(database_t *db, size_t indice){
	if (indice >= db->lsize){
		return false;
	}
	db->lsize--;
	// on réecrit la suite de la db, à la place de l'étudiant supprimé
	for (size_t i = indice; i < db->lsize; i++){
		db->data[i] = db->data[i+1];
	}
	return true;
}

/*
 * La fonction db_search permet de chercher un champ particulier dans la db
 * Cette fonction prend en argument l'addresse de la database, ainsi que
 * la valeur cherché, une liste stockant les étudiants concernés, et enfin
 * le champ recherché.  
 */

void db_search(database_t *db, char* value, vector<student_t> *list_value, char* field){
	for (size_t i = 0; i<db->lsize; i++){
		student_t student = db->data[i];
		if (strcmp(field,"id")==0){
			char buffer[10];
			snprintf(buffer, sizeof(buffer), "%u", student.id); //convertion de int -> char
			if(strcmp(buffer,value)==0){
				list_value->push_back(student);
			}
		}
		if (strcmp(field,"fname")==0){
			if(strcmp(student.fname, value)==0){
				list_value->push_back(student);
			}
		}
		if (strcmp(field, "lname")==0){
			if(strcmp(student.lname, value)==0){
				list_value->push_back(student);
			}
		}
		if (strcmp(field, "section")==0){
			if(strcmp(student.section, value)==0){
				list_value->push_back(student);
			}
		}
		if (strcmp(field, "birthdate")==0){
			char time_b[11];
			strftime(time_b, sizeof(time_b), "%d/%m/%Y", &student.birthdate); //conversion struct tm -> char
			if(strcmp(time_b, value)==0){
				list_value->push_back(student);
			}
		}
	}
}
