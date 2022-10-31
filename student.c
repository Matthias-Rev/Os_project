#include "student.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void student_to_str(char* buffer, student_t* s) {
	//buffer = "ID:%d  First name: %s Last name: %s section: %s",s->id, s->fname,s->lname,s->section;
	printf("ID:%d  First name: %s Last name: %s section: %s \n",s->id, s->fname,s->lname,s->section);
}

int student_equals(student_t* s1, student_t* s2) {
	if (s1->id == s2->id){
		return 1;
	}
	return 0;
}
