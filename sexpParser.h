#ifndef SEXPPARSER_H
#define SEXPPARSER_H

#include <stdlib.h>     /* for malloc and friends */
#include <stdio.h>      /* for FILE*, size_t and friends */
#include <string.h>     /* for strcpy and friends */
#include <ctype.h>      /* for isspace */

/* struct prototypes/typedefs */
typedef struct  sexpParser_vector_s     sexpParser_vector;
typedef struct  sexpParser_string_s     sexpParser_string;
typedef union   sexpParser_value_u      sexpParser_value;
typedef enum    sexpParser_type_e       sexpParser_type;
typedef struct  sexpParser_element_s    sexpParser_element;
typedef struct  sexpParser_state_s      sexpParser_state;
struct  sexpParser_vector_s;
struct  sexpParser_string_s;
union   sexpParser_value_u;
enum    sexpParser_type_e;
struct  sexpParser_element_s;
struct  sexpParser_state_s;

/* a replacement for C++ vector */
struct sexpParser_vector_s {
    size_t                  size;
    size_t                  capacity;
    sexpParser_element**    head;
};

/* a replacement for C++ string */
struct sexpParser_string_s {
    size_t                  size;
    size_t                  capacity;
    char*                   s;
};

/* the value an element is allowed to take */
union sexpParser_value_u {
    sexpParser_string   s;
    sexpParser_vector   v;
};

/* parsed element types */
enum sexpParser_type_e { sexpParser_atom_t, sexpParser_string_t, sexpParser_list_t, sexpParser_none_t };

/* a parsed element. */
struct sexpParser_element_s {
    sexpParser_type     t;
    sexpParser_value    v;
};

/* the current parser state */
struct sexpParser_state_s {
    FILE*   is; /* input stream */
    int     c;  /* next input character */
};



/* function prototypes */
void sexpParser_ini_element (sexpParser_element*    e);
void sexpParser_ini_element_from_string (sexpParser_element*    e, sexpParser_string* s);
void sexpParser_ini_element_from_atom   (sexpParser_element*    e, sexpParser_string* s);
void sexpParser_ini_element_from_vector (sexpParser_element*    e, sexpParser_vector* v);
void sexpParser_ini_vector  (sexpParser_vector*     v);
void sexpParser_ini_string  (sexpParser_string*     s);
void sexpParser_ini_string_from_string  (sexpParser_string*     s,  char* c,    size_t size);
void sexpParser_del_element (sexpParser_element*    e);
void sexpParser_del_vector  (sexpParser_vector*     v);
void sexpParser_del_string  (sexpParser_string*     s);

void sexpParser_app_vector  (sexpParser_vector*     v,  sexpParser_element* e);
void sexpParser_pop_vector  (sexpParser_vector*     v);
void sexpParser_clean_vector(sexpParser_vector*     v); /* delete all elements inside the vector */

void sexpParser_app_string  (sexpParser_string*     s,  char c);
void sexpParser_pop_string  (sexpParser_string*     s);

void sexpParser_ini_state   (sexpParser_state*      s,  FILE* is);
void sexpParser_read        (sexpParser_state*      s);
void sexpParser_whitespace  (sexpParser_state*      s);
void sexpParser_parse       (sexpParser_state*      s, sexpParser_vector*   v);
void sexpParser_parseElement(sexpParser_state*      s, sexpParser_element*  e);
void sexpParser_parseString (sexpParser_state*      s, sexpParser_element*  e);
void sexpParser_parseAtom   (sexpParser_state*      s, sexpParser_element*  e);
void sexpParser_parseList   (sexpParser_state*      s, sexpParser_element*  e);

void sexpParser_fprint_element  (FILE* fout,    sexpParser_element*    e);
void sexpParser_fprint_vector   (FILE* fout,    sexpParser_vector*     v);





/* function implementations */
void sexpParser_ini_element (sexpParser_element*    e) {
    e->t = sexpParser_none_t;
}
void sexpParser_ini_element_from_string (sexpParser_element*    e, sexpParser_string* s){
    sexpParser_del_element(e);
    e->t = sexpParser_string_t;
    e->v.s = *s;
}
void sexpParser_ini_element_from_atom   (sexpParser_element*    e, sexpParser_string* s){
    sexpParser_del_element(e);
    e->t = sexpParser_atom_t;
    e->v.s = *s;
}
void sexpParser_ini_element_from_vector (sexpParser_element*    e, sexpParser_vector* v){
    sexpParser_del_element(e);
    e->t = sexpParser_list_t;
    e->v.v = *v;
}


void sexpParser_ini_vector  (sexpParser_vector*     v) {
    v->size = 0;
    v->capacity = 10;
    v->head = malloc(sizeof(sexpParser_element*) * (v->capacity));
}
void sexpParser_ini_string  (sexpParser_string*     s){
    s->size = 0;
    s->capacity = 10;
    s->s = malloc(sizeof(char)*(s->capacity));
    s->s[s->size] = '\0';
}
void sexpParser_ini_string_from_string  (sexpParser_string*     s,  char* c,    size_t size) {
    s->size = size;
    s->capacity = size + 1;
    s->s = malloc(sizeof(char)*(s->capacity));
    strcpy(s->s, c);
    /* regardless of whether c ended with '\0', we want to make sure s->s ends with '\0' */
    s->s[size] = '\0';
}
void sexpParser_del_element (sexpParser_element*    e) {
    switch (e->t){
    case sexpParser_atom_t:
    case sexpParser_string_t:
        sexpParser_del_string(&(e->v.s));
        break;
    case sexpParser_list_t:
        sexpParser_del_vector(&(e->v.v));
        break;
    case sexpParser_none_t:
        break;
    }
    e->t = sexpParser_none_t;
}
void sexpParser_del_vector  (sexpParser_vector*     v) {
    free(v->head);
}
void sexpParser_del_string  (sexpParser_string*     s) {
    free(s->s);
}



void sexpParser_app_vector  (sexpParser_vector*     v,  sexpParser_element* e){
    if (v->capacity < v->size + 1){
        v->head = realloc(v->head, sizeof(sexpParser_element*) * (v->capacity * 2) );
        v->capacity *= 2;
    }
    v->head[v->size] = e;
    v->size++;
}
void sexpParser_pop_vector  (sexpParser_vector*     v){v->size--;}
void sexpParser_clean_vector(sexpParser_vector*     v){
    int i;
    for (i = 0; i < v->size; v++){
        sexpParser_del_element  (v->head[i]);
        free                    (v->head[i]);
        v->head[i]              = NULL;
    }
}




void sexpParser_app_string  (sexpParser_string*     s,  char c){
    if (s->capacity < s->size + 2){ /* we need extra room for '\0' */
        s->s = realloc(s->s, sizeof(char) * (s->capacity * 2));
        s->capacity *= 2;
    }
    s->s[s->size] = c;
    s->size++;
    s->s[s->size] = '\0';
}
void sexpParser_pop_string  (sexpParser_string*     s){s->size--;}



void sexpParser_ini_state   (sexpParser_state*      s,  FILE* is){
    s->is   = is;
    s->c    = getc(is);
}
void sexpParser_read        (sexpParser_state*      s){ s->c = getc(s->is); }
void sexpParser_whitespace  (sexpParser_state*      s){
    while (isspace(s->c) && s->c != EOF) sexpParser_read(s);
}
void sexpParser_parse       (sexpParser_state*      s, sexpParser_vector*   v){
    sexpParser_element* e;
    
    sexpParser_whitespace(s);
    while (s->c != EOF){
        e = malloc(sizeof(sexpParser_element));
        sexpParser_ini_element(e);
        
        sexpParser_parseElement(s,e);
        
        sexpParser_app_vector(v,e);
        
        sexpParser_whitespace(s);
    }
}
void sexpParser_parseElement(sexpParser_state*      s, sexpParser_element*  e) {
    sexpParser_whitespace(s);
    
    switch (s->c){
    case '\'':
    case '\"':
        sexpParser_parseString(s,e);    break;
    case '(' :
        sexpParser_parseList(s,e);      break;
    default:
        sexpParser_parseAtom(s,e);      break;
    }
}
void sexpParser_parseList   (sexpParser_state*      s, sexpParser_element*   e){
    sexpParser_vector* v = malloc(sizeof(sexpParser_vector));
    sexpParser_ini_vector(v);
    
    sexpParser_read(s); /* consume '(' */
    
    sexpParser_element* ee;
    
    sexpParser_whitespace(s);
    
    while (s->c != ')' && s->c != EOF ){
        ee = malloc(sizeof(sexpParser_element));
        sexpParser_ini_element(ee);
        
        sexpParser_parseElement(s,ee);
        sexpParser_app_vector(v,ee);
        
        sexpParser_whitespace(s);
    }
    
    sexpParser_read(s); /* consume ')' */
    
    sexpParser_ini_element_from_vector(e,v);
}
void sexpParser_parseString (sexpParser_state*      s, sexpParser_element*   e) {
    sexpParser_string* v = malloc(sizeof(sexpParser_string));
    sexpParser_ini_string(v);
    
    int quote = s->c;
    
    sexpParser_read(s); /* consume begin quote */
    while (s->c != quote && s->c != EOF){
        if (s->c != '\\'){
            sexpParser_app_string(v,s->c);
        }
        else { /* handle escape */
            sexpParser_read(s); /* consume '\\' */
            switch (s->c){
            case '\\': sexpParser_app_string(v,'\\'); break;
            case 'n' : sexpParser_app_string(v,'\n'); break;
            case 't' : sexpParser_app_string(v,'\t'); break;
            case '\'': sexpParser_app_string(v,'\''); break;
            case '"' : sexpParser_app_string(v,'\"'); break;
            }
        }
        sexpParser_read(s);
    }
    sexpParser_read(s); /* consume end quote */
    
    sexpParser_ini_element_from_string(e,v);
}
void sexpParser_parseAtom   (sexpParser_state*      s, sexpParser_element*  e) {
    sexpParser_string* v = malloc(sizeof(sexpParser_string));
    sexpParser_ini_string(v);
    
    while (s->c != '(' && s->c != ')' && s->c != '"' && s->c != '\'' && !isspace(s->c) && s->c != EOF){
        sexpParser_app_string(v,s->c);
        sexpParser_read(s);
    }
    sexpParser_ini_element_from_atom(e,v);
}



void sexpParser_fprint_element  (FILE* fout,    sexpParser_element*    e){
    switch (e->t){
    case sexpParser_atom_t:
        fprintf(fout,"%s",e->v.s.s);
        break;
    case sexpParser_string_t:
        fprintf(fout,"\"%s\"",e->v.s.s);
        break;
    case sexpParser_none_t:
        fprintf(fout,"none");
        break;
    case sexpParser_list_t:
        sexpParser_fprint_vector(fout,&(e->v.v));
        break;
    }
}
void sexpParser_fprint_vector   (FILE* fout,    sexpParser_vector*     v){
    fprintf(fout,"[");
    int first = (1 == 1);
    int i;
    for (i = 0; i < v->size; i++){
        if (!first) fprintf(fout,",");
        sexpParser_fprint_element(fout,v->head[i]);
        first = (1 != 1);
    }
    fprintf(fout,"]");
}


#endif /* SEXPPARSER_H */