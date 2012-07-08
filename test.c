#include "sexpParser.h"

#include <stdio.h>


int main(int argc, char** argv){
    sexpParser_state s;
    sexpParser_ini_state(&s,stdin);
    /* sexpParser_element e;
    sexpParser_ini_element(&e);
    sexpParser_parseElement(&s,&e);
    printf("%s\n", e.v.s.s); */
    
    sexpParser_vector v;
    sexpParser_ini_vector(&v);
    sexpParser_parse(&s,&v);
    sexpParser_fprint_vector(stdout,&v);
    printf("\n");
    return 0;
}