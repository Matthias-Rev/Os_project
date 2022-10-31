#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "student.h"

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
    printf("Loading the database....\n");
    if (!file) {
        perror("Could not open the DB file");
        exit(1);
    }
    student_t student;
    while (fread(&student, sizeof(student_t), 1, file)) {
		if (db->lsize <= 100){
			db_add(db, student);
		} else {break;}
	}
    fclose(file);
	printf("Done !");
}

void db_init(database_t *db) {
  // Your code here
  db->lsize = 0;
  db->psize = 100;
  db->data = (student_t *) malloc ( sizeof (student_t) * db->psize);
}

void db_add(database_t *db, student_t student) {
  // Your code here
  char buffer[128];
  db_extend(db);
  db->data[db->lsize] = student;
  student_to_str(buffer, &student);
  db->lsize++;
}

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
