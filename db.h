#ifndef _DB_H
#define _DB_H

#include <stdbool.h> 
#include "student.h"
#include <iostream>
#include <vector>
using namespace std;
/**
 * Database structure type.
 */
typedef struct {
  student_t *data; /** The list of students **/
  size_t lsize;    /** The logical size of the list **/
  size_t psize;    /** The physical size of the list **/
} database_t;


/**
 *  Add a student to the database.
 **/
void db_add(database_t *db, student_t s);

/**
 * Save the content of a database_t in the specified file.
 **/
void db_save(database_t *db, const char *path);

/**
 * Load the content of a database of students.
 **/
void db_load(database_t *db, const char *path);

/**
 * Initialise a database_t structure.
 * Typical use:
 * ```
 * database_t db;
 * db_init(&db);
 * ```
 **/
void db_init(database_t *db);
void db_extend(database_t *db);
bool db_remove(database_t *db, size_t index);
void db_search(database_t *db, char* value, vector<student_t> *list_value, char* field);
#endif
