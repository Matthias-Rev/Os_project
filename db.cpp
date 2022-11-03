#include "db.h"
#include <iostream>
#include "student.h"
#include <vector>
#include <string>
#include <iterator>
using namespace std;

// works fine
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

//works fine
void db_load(database_t *db, const char *path) {
    FILE *file = fopen(path, "rb");
    cout << "Loading the database....\n" << endl;
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

//works fine
void db_init(database_t *db) {
  // Your code here
  db->lsize = 0;
  db->psize = 100;
  db->data = (student_t *) malloc ( sizeof (student_t) * db->psize);
}

//works fine
void db_add(database_t *db, student_t student) {
  // Your code here
  char buffer[128];
  db_extend(db);
  db->data[db->lsize] = student;
  student_to_str(buffer, &student);
  db->lsize++;
}

//works fine
void db_extend(database_t *db){
	if(db->lsize >= db->psize){
		student_t* old_values = db->data;
		size_t old_size = db->psize;
		db->psize = db->psize * 2;
        db->data = (student_t *) malloc(sizeof(student_t) * db->psize);
		memcpy(db->data, old_values, old_size* sizeof(student_t));
        free(old_values);
	}
}

//works fine
bool db_remove(database_t *db, size_t indice){
	if (indice >= db->lsize){
		return false;
	}
	db->lsize--;
	for (size_t i = indice; i < db->lsize; i++){
		db->data[i] = db->data[i+1];
	}
	return true;
}

//works fine
void db_search(database_t *db, char* value, vector<student_t> *list_value, char* field){
	for (size_t i = 0; i<db->lsize; i++){
		student_t student = db->data[i];
		if (strcmp(field,"id")==0){
			char buffer[10];
			snprintf(buffer, sizeof(buffer), "%u", student.id);
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
			strftime(time_b, sizeof(time_b), "%d/%m/%Y", &student.birthdate);
			if(strcmp(time_b, value)==0){
				list_value->push_back(student);
			}
		}
	}
}
