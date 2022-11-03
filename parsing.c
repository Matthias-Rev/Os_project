#include "parsing.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "student.h"

bool parse_update(char* query, char* field_filter, char* value_filter, char* field_to_update, char* update_value) {
    char* key_val_filter = strtok_r(NULL, " ", &query);  // key=val filter
    if (key_val_filter == NULL) {
        return false;
    }
    if (strtok_r(NULL, " ", &query) == NULL) {  // discard the "set" keyword
        return false;
    }

    char* key_val_update = strtok_r(NULL, " ", &query);  // key=val updated value
    if (key_val_update == NULL) {
        return false;
    }

    if (parse_selectors(key_val_filter, field_filter, value_filter) == 0) {
        return false;
    }
    if (parse_selectors(key_val_update, field_to_update, update_value) == 0) {
        return false;
    }
    return true;
}

// test it and it goes correctly
bool parse_insert(char* query, char* fname, char* lname, unsigned int* id, char* section, struct tm* birthdate) {
    char* token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(fname, token);
	printf("the fname=%s\n", fname);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(lname, token);
	printf("the lname=%s\n", lname);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    *id = (unsigned)atol(token);
	printf("the id= %d \n", *id);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(section, token);
	printf("the section=%s\n", section);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    if (strptime(token, "%d/%m/%Y", birthdate) == NULL) {
        return false;
    }
	token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return true;
    }
    return true;
}

// test correctly, works fine
bool parse_selectors(char* query, char* field, char* value) {
    char* token = strtok_r(NULL, "=", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(field, token);
    token = strtok_r(NULL, "=", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(value, token);
	token = strtok_r(NULL, "=", &query);
    if (token == NULL) {
        return true;
    }
	return true;
}
