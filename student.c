#include "student.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// permet d'écrire les arguments d'un étudiant de manière lisible
void student_to_str(char* buffer, student_t* s) {
	char birth[11];
	strftime(birth, sizeof(birth), "%d/%m/%Y", &s->birthdate);
	printf("ID:%d  First name: %s Last name: %s section: %s née le %s \n",s->id, s->fname,s->lname,s->section, birth);
}

// compare deux étudiants
int student_equals(student_t* s1, student_t* s2) {
	if (s1->id == s2->id){
		return 1;
	}
	return 0;
}
