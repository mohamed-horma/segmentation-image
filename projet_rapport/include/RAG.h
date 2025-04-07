#ifndef RAG_H
#define RAG_H

#include "image.h"

typedef struct RAG* Rag ;

extern Rag create_RAG(image img, int n, int m);
extern void RAG_give_mean_color(Rag rag, int block, int* color);
extern image create_output_image(Rag rag);
void perform_merge(Rag rag, double threshold);



#endif


