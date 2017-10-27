#ifndef PARSECONF_H
#define PARSECONF_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char * getsecondword(char * str);
char * getitem(char * word);
char * getportnumber();
char * getroot();
char * getdefaultpage();

#endif
