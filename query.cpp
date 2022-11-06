#include "query.h"
#include "parsing.c"
#include "db.h"
#include "student.h"
#include "utils.c"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <cstring>
using namespace std;

/*
 * Certaines parties du code qui vont suivre se repètent,
 * nous n'avons donc pas prie la peine de re expliquer
 * les bouts de code qui sont réutilisés plusieurs fois.
 */


// Fonction de base
void query_result_init(query_result_t *result,char query[256]){
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
	result->status = QUERY_FAILURE;
	result->lsize = 0;
	result->psize = 10;
	result->students = (student_t *) malloc ( sizeof (student_t) * result->psize);
	strcpy(result->query,query);
}

/*
 * La fonction query_select lié à la requête "select"
 * Cette fonction prend en argument l'addresse de la database, ainsi que
 * la deuxième partie de la requête. 
 * Ex: select lname=Leclerc;
 *            |____________|
 *			  deuxième partie
 */

void query_select(database_t *db, char *query){ 

	// instancie une structure query_result_s pour y stocker les informations
	// lié à la requête (temps pris, liste des étudiants concernés,...)
	query_result_t result_s; 
	query_result_init(&result_s, query);
	char field[64]; //stocke le champ à chercher (lname, fname, ...)
	char value[64]; //stocke la valeur à modifier
	char buffer[64]; //permet l'écriture des étudiants dans le terminal
	
	// parse permet de récuperer la requête et d'en tirer les informations nécessaires
	if (parse_selectors(query, field, value) == false){ perror("error has occured"); } 
	//vector permet de stocker la recherche d'étudiant (car il peut en avoir plusieurs)
	vector<student_t> list_value;
	//db_search permet de chercher les étudiants ayant un field correpsondant à la valeur
	db_search(db, value, &list_value, field);

	// la for loop enregistre tout les étudiants dans l'espace alouer à result.students
	for (auto i=list_value.begin(); i<list_value.end();i++){
		query_result_add(&result_s, *i); // enregistre l'étudiant en paramètre
		student_t student = *i;
		student_to_str(buffer, &student); // permet d'afficher les étudiants concernés
		cout << "\n";
		result_s.status = QUERY_SUCCESS;
	}
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	result_s.end_ns = now.tv_nsec + 1e9 * now.tv_sec; //fin de la requête
	log_query(&result_s); 
}


/*
 * La fonction query_update lié à la requête "update"
 * Cette fonction prend en argument l'addresse de la database, ainsi que
 * la deuxième partie de la requête. 
 * Ex: update lname=Leclerc set birthdate=11/12/1990;
 *            |_____________________________________|
 *						deuxième partie
 */

void query_update(database_t *db, char* query){
	query_result_t result_u;
	query_result_init(&result_u, query);
	char field_filter[64]; // stocke le champ qui filtre la requête (ici: lname)
	char value_filter[64]; // stocke la valeur qui filtre la requête (ici: Leclerc)
	char field_to_update[64]; // stocke le champ à modifier (ici: birthdate)
	char update_value[64]; // stocke la valeur à modifier (ici: 11/12/1990)
	char buffer[10]; 
	// stocke le unsigned int convertie en char pour pouvoir faciliter les comparaisons
	// avec les valeurs lu dans la database
	char time_b[11];
	// même principe que pour le buffer, mise à part que il stocke un struct tm
	
	if (parse_update(query,field_filter, value_filter, field_to_update, update_value) == false){ perror("error has occured"); }

	for (size_t y = 0; y<db->lsize; y++){
		// for loop pour lire les étudiants de la database

		student_t student = db->data[y];

		//s'occupe de la conversion de unsigned int -> char
		if (strcmp(field_filter,"id")==0){
		snprintf(buffer, sizeof(buffer), "%u", student.id);
		}
		//s'occupe de la conversion de unsigned int -> char
		if (strcmp(field_filter,"birthdate")==0){
			strftime(time_b, sizeof(time_b), "%d/%m/%Y", &student.birthdate);
		}

		// compare la valeur des arguments de l'étudiant lu, avec la valeur à filter
		/* Ex: value_filter = Jean
		 * student.fname = Jean
		 * strcmp(student.fname, value_filter) = 0  -> les chaines de caractères sont équivalentes
		 */

		if (strcmp(buffer,value_filter)==0 || strcmp(student.fname, value_filter)==0 || strcmp(student.lname ,value_filter)==0 || strcmp(student.section,value_filter)==0 || strcmp(time_b, value_filter) == 0){

			if (strcmp(field_to_update, "id")==0){
				unsigned int id;
				id = (unsigned)atol(update_value); //permet de convertir un unsigned int en char
				db->data[y].id = id;
				result_u.status = QUERY_SUCCESS; //permet de notifer le succès de la request
			}
			if (strcmp(field_to_update, "fname")==0){
				strcpy(db->data[y].fname, update_value); // change la valeur de student.fname par update_value
				result_u.status = QUERY_SUCCESS;
			}
			if (strcmp(field_to_update, "lname")==0){
				strcpy(db->data[y].lname, update_value);
				result_u.status = QUERY_SUCCESS;
			}
			if (strcmp(field_to_update,"section")==0){
				strcpy(db->data[y].section, update_value);
				result_u.status = QUERY_SUCCESS;
			}
			if (strcmp(field_to_update, "birthdate")==0){
				struct tm birthdate;
				strptime(update_value, "%d/%m/%Y", &birthdate); 
				db->data[y].birthdate = birthdate;
				result_u.status = QUERY_SUCCESS;
			}
			query_result_add(&result_u, student);		}
	}
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	result_u.end_ns = now.tv_nsec + 1e9 * now.tv_sec;
	log_query(&result_u);
}

/*
 * La fonction query_insert lié à la requête "insert"
 * Cette fonction prend en argument l'addresse de la database, ainsi que
 * la deuxième partie de la requête. 
 * Ex: insert Matthias Reveillard informatique 11/07/1900;
 *           |___________________________________________|
 *						deuxième partie
 */

void query_insert(database_t *db, char* query){  
	query_result_t result_i;
	query_result_init(&result_i, query);
	char fname[64]; 
	char lname[64];
	char section[64];
	struct tm birthdate;
	//unsigned int id;
	if (parse_insert(query, fname, lname, section, &birthdate) == false){ perror("Problem has occured"); }
	student_t new_student ;
	strcpy(new_student.fname, fname);
	strcpy(new_student.lname, lname);
	strcpy(new_student.section, section);
	new_student.birthdate = birthdate;
	//new_student.id = id;
	query_result_add(&result_i, new_student);

	size_t i; // iterateur de la database
	for (i=0; i < db->lsize; i++){

		// vérifie si l'id choisie ne correspond pas à un id existant
		if(db->data[i].id == new_student.id){
			printf("the insert goes wrong...");
			break;
		}
	}

	// si l'iterateur a lu toute la database, alors l'id choisi n'est pas dans la db
	if (i==db->lsize){
		cout << "adding the new student \n";
		result_i.status = QUERY_SUCCESS;
		db_add(db,new_student);
	}

	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	result_i.end_ns = now.tv_nsec + 1e9 * now.tv_sec;
	log_query(&result_i); 
}


/*
 * La fonction query_delete lié à la requête "delete""
 * Cette fonction prend en argument l'addresse de la database, ainsi que
 * la deuxième partie de la requête. 
 * Ex: delete lname=Frederic
 *            |_____________|
 *			  deuxième partie
 */

void query_delete(database_t *db,char* query){
	query_result_t result_d;
	query_result_init(&result_d, query);
	char field[64];
	char value[64];
	if (parse_selectors(query, field, value) == false) { perror("Problem has occured\n"); }

	for (size_t y = 0; y<db->lsize; y++){
		//permet de lire les étudiants de la database

		student_t student = db->data[y];

		if (strcmp(field,"id")==0){
			char buffer[10];
			snprintf(buffer, sizeof(buffer), "%u", student.id);
			if(strcmp(buffer,value)==0){
				query_result_add(&result_d, student);
				if(db_remove(db, y)){cout << "remove correctly\n" << endl;}
				result_d.status = QUERY_SUCCESS;

				/*
				 * y se decrémente car lorsque on efface l'étudiant, 
				 * sa place est prise par l'étudiant en y+1.
				 * Donc si on ne décremente rien, nous pourrions rater 
				 * des étudiants à chaque suppression.
				 */
				y--;
			}
		}
		if (strcmp(field, "fname")==0){
			if(strcmp(student.fname, value)==0){
				query_result_add(&result_d, student);
				if(db_remove(db, y)){cout << "remove correctly\n" << endl;}
				result_d.status = QUERY_SUCCESS;
				y--;
			}
		}
		if (strcmp(field,"lname")==0){
			cout << "here lname";
			if(strcmp(student.lname, value)==0){
				cout << "enter lname";
				query_result_add(&result_d, student);
				if(db_remove(db, y)){cout << "remove correctly\n" << endl;}
				result_d.status = QUERY_SUCCESS;
				y--;
			}
		}
		if (strcmp(field,"section")==0){
			if(strcmp(student.section,value)==0){
				query_result_add(&result_d, student);
				if(db_remove(db, y)){cout << "remove correctly\n" << endl;}
				result_d.status = QUERY_SUCCESS;
				y--;
			}
		}
		if (strcmp(field,"birthdate")==0){
			char time_b[11];
			strftime(time_b, sizeof(time_b), "%d/%m/%Y", &student.birthdate);
			if(strcmp(time_b,value)==0){
				query_result_add(&result_d, student);
				if(db_remove(db, y)){cout << "remove correctly\n" << endl;}
				result_d.status = QUERY_SUCCESS;
				y--;
			}
		}
	}
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	result_d.end_ns = now.tv_nsec + 1e9 * now.tv_sec;
	log_query(&result_d); 
}





void query_result_add(query_result_t *query, student_t student) {
  query_result_extend(query);	// vérifie si la mémoire physique arrive à saturation
  query->students[query->lsize] = student; // rajoute l'étudiant
  query->lsize++;
}



void query_result_extend(query_result_t *query){
	if(query->lsize >= query->psize){
		student_t* old_values = query->students; // enregistre les étudiants dans un tampon
		size_t old_size = query->psize;
		query->psize = query->psize * 2; // double l'espace alouer (peut être excessif)
        query->students = (student_t *) malloc(sizeof(student_t) * query->psize);
		memcpy(query->students, old_values, old_size* sizeof(student_t));
        free(old_values);
	}
}
