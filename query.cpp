#include "query.h"
#include "parsing.c"
#include "db.h"
#include "student.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <cstring>
using namespace std;

void query_result_init(query_result_t *result,char query[256]){
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
	result->status = QUERY_SUCCESS;
	result->lsize = 0;
	result->psize = 10;
	result->students = (student_t *) malloc ( sizeof (student_t) * result->psize);
	strcpy(result->query,query);
}


void query_select(database_t *db, char *query){
	//read from the pipe
	query_result_t result_s;
	query_result_init(&result_s, query);
	char field[64];
	char value[64];
	char buffer[64];
	if (parse_selectors(query, field, value) == false){ perror("error has occured"); }
	vector<student_t> list_value;
	db_search(db, value, &list_value, field);
	for (auto i=list_value.begin(); i<list_value.end();i++){
		query_result_add(&result_s, *i);
		student_t student = *i;
		student_to_str(buffer, &student);
		cout << "\n";
	}
	//return query
}



void query_update(database_t *db, char* query){
	//read from the pipe
	query_result_t result_u;
	query_result_init(&result_u, query);
	char field_filter[64];
	char value_filter[64];
	char field_to_update[64];
	char update_value[64];
	char buffer[10];
	char time_b[11];
	if (parse_update(query,field_filter, value_filter, field_to_update, update_value) == false){ perror("error has occured"); }
	for (size_t y = 0; y<db->lsize; y++){
		student_t student = db->data[y];
		if (strcmp(field_filter,"id")==0){ // verify if field_filter is an id
			snprintf(buffer, sizeof(buffer), "%u", student.id);
		}
		if (strcmp(field_filter,"birthdate")==0){ // verify if field_filter is an birthdate
			strftime(time_b, sizeof(time_b), "%d/%m/%Y", &student.birthdate);
		}

		if (strcmp(buffer,value_filter)==0 || strcmp(student.fname, value_filter)==0 || strcmp(student.lname ,value_filter)==0 || strcmp(student.section,value_filter)==0 || strcmp(time_b, value_filter) == 0){
			if (strcmp(field_to_update, "id")==0){
				unsigned int id;
				id = (unsigned)atol(update_value);
				db->data[y].id = id;
			}
			if (strcmp(field_to_update, "fname")==0){
				strcpy(db->data[y].fname, update_value);
			}
			if (strcmp(field_to_update, "lname")==0){
				strcpy(db->data[y].lname, update_value);
			}
			if (strcmp(field_to_update,"section")==0){
				strcpy(db->data[y].section, update_value);
			}
			if (strcmp(field_to_update, "birthdate")==0){
				struct tm birthdate;
				strptime(update_value, "%d/%m/%Y", &birthdate); 
				db->data[y].birthdate = birthdate;
			}
			query_result_add(&result_u, student);
		}
	}//return query_result
}



// works fine
void query_insert(database_t *db, char* query){  
	query_result_t result_i;
	query_result_init(&result_i, query);
	char fname[64]; 
	char lname[64];
	char section[64];
	unsigned int id;
	struct tm birthdate;
	if (parse_insert(query, fname, lname, &id, section, &birthdate) == false){ perror("Problem has occured"); }
	student_t new_student ;
	strcpy(new_student.fname, fname);
	strcpy(new_student.lname, lname);
	strcpy(new_student.section, section);
	new_student.id = id;
	new_student.birthdate = birthdate;
	query_result_add(&result_i, new_student);
	size_t i;
	for (i=0; i < db->lsize; i++){
		if(db->data[i].id == new_student.id){
			printf("the insert goes wrong...");
			break;
		}
	}
	if (i==db->lsize){
		cout << "adding the new student \n";
		db_add(db,new_student);
	}
	//return query
}


// works fine
void query_delete(database_t *db,char* query){
	query_result_t result_d;
	query_result_init(&result_d, query);
	char field[64];
	char value[64];
	if (parse_selectors(query, field, value) == false) { perror("Problem has occured\n"); }
	for (size_t y = 0; y<db->lsize; y++){
		student_t student = db->data[y];
		if (strcmp(field,"id")==0){
			char buffer[10];
			snprintf(buffer, sizeof(buffer), "%u", student.id);
			if(strcmp(buffer,value)==0){
				query_result_add(&result_d, student);
				if(db_remove(db, y)){cout << "remove correctly\n" << endl;}
				y--;
			}
		}
		if (strcmp(field, "fname")==0){
			if(strcmp(student.fname, value)==0){
				query_result_add(&result_d, student);
				if(db_remove(db, y)){cout << "remove correctly\n" << endl;}
				y--;
			}
		}
		if (strcmp(field,"lname")==0){
			//cout << student.lname << value << "\n";
			//cout <<strlen(student.lname) << " longueur " << strlen(value)<<"\n";
			//cout <<"cmp= " <<strcmp(student.lname, value);
			if(strcmp(student.lname, value)==0){
				cout << "here";
				query_result_add(&result_d, student);
				if(db_remove(db, y)){cout << "remove correctly\n" << endl;}
				y--;
			}
		}
		if (strcmp(field,"section")==0){
			if(strcmp(student.section,value)==0){
				query_result_add(&result_d, student);
				if(db_remove(db, y)){cout << "remove correctly\n" << endl;}
				y--;
			}
		}
		if (strcmp(field,"birthdate")==0){
			char time_b[11];
			strftime(time_b, sizeof(time_b), "%d/%m/%Y", &student.birthdate);
			if(strcmp(time_b,value)==0){
				query_result_add(&result_d, student);
				if(db_remove(db, y)){cout << "remove correctly\n" << endl;}
				y--;
			}
		}
	}
		
}





void query_result_add(query_result_t *query, student_t student) {
  // Your code here
  query_result_extend(query);
  query->students[query->lsize] = student;
  query->lsize++;
}



void query_result_extend(query_result_t *query){
	if(query->lsize >= query->psize){
		student_t* old_values = query->students;
		size_t old_size = query->psize;
		query->psize = query->psize * 2;
        query->students = (student_t *) malloc(sizeof(student_t) * query->psize);
		memcpy(query->students, old_values, old_size* sizeof(student_t));
        free(old_values);
	}
}

