#ifndef _TYPE_OBJ_
#define _TYPE_OBJ_

#include <limits.h>
/*
------------------------------------------------------
#define malloc(s)        MALLOC_((s))
#define free(s)          FREE_((s))
#define calloc(t,nb)     CALLOC_((t),(nb))
#define realloc(pt,t)    REALLOC_((pt),(t))
------------------------------------------------------
*/

/*#ifndef OC_MAKEM
#include <sys/types.h>
#include <sys/uio.h>
#endif*/


typedef enum{faux,vrai} bouleen;

typedef char any;
typedef void* any_ptr;

typedef struct f_cel_ {
                        int taille;
                        struct f_cel_ *suiv;
                        any *mem;
                      } *FREECEL,freecel;

typedef int dart;
typedef char byte;
typedef  int angle;


#define FINITE_FACE -1

#define CHAR_SIZE     1
#define INT_SIZE      4
#define LONG_SIZE     4
#define OBJDESC_SIZE  4

#define INFINITE  INT_MIN

#ifndef NULL
#define NULL  0x0
#endif

#define NIL   0

# define NUL_B(b_)     (b_ == 0)


#define MAXMAP 500

#define DEFAULT_DESC_LENGTH   10240     /* 10 Kbytes */
#define DEFAULT_REQ_LENGTH    1024

#define DIM_MAX 3

#endif





