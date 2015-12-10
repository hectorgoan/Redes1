/*
** Fichero: mylib.h
** Autores: 
** Héctor Gonzalo Andrés DNI 71038384D
** Néstor 
*/
#ifndef HEADER_SERVIDOR
#define HEADER_SERVIDOR

//servidor's functions
void INThandler(int);
void ObtainUsersFromString (char*);
char* ObtainStringFromTextFile (void);
char *replaceStringAforStringB(char *str, char *a, char *b);


#endif