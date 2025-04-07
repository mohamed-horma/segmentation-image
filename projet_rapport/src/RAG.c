#include "RAG.h"
#include <stdlib.h>
#include "image.h"
#include <stdio.h>
#include <math.h>



typedef struct cellule* cellule;
typedef struct moments* moments;

struct moments {
    double M0;
    double M1[3];
    double M2[3];
};

struct cellule {
    int block;
    cellule next;
};

struct RAG {
    int size;
    image img;
    moments m;
    int* father;
    cellule neighbors;
};

/*Fonction give_moments*/
extern void give_moments(image img, int block_id, int n, int m, double *m0, double* m1, double* m2)
{    
    int i,j,R,G,B;
    int* pix;
    Point P;
    int width, height, block_width, block_height, x_block, y_block;
    
    width = image_give_largeur(img);
    height = image_give_hauteur(img);
    
    /*les dimensions du block */
    block_width = width/n;
    block_height = height/m;

    /*Les coordonnées du block*/
    x_block = (block_id/n) * block_height;
    y_block = (block_id % n) * block_width;


    /*Parcourir les pixels du block*/
    for ( i = x_block ; i < x_block + block_height; i++)
    {
        for ( j = y_block; j < y_block + block_width ; j++)
        {   
            COORDX(P) = i;
            COORDY(P) = j;
            image_move_to(img,&P);
            
            /* Lire le pixel à la position (i,j) */
            pix = image_lire_pixel(img);
            R = pix[0];  
            G = pix[1];  
            B = pix[2];  

            /*Moment d'ordre 0*/
            (*m0)++;

             /*Moment d'ordre 1 (somme des intensités des couleurs)*/ 
            m1[0] += R;
            m1[1] += G;
            m1[2] += B;

            /*Moment d'ordre 2 (somme des carrés des intensités des couleurs)*/
            m2[0] += R * R;
            m2[1] += G * G;
            m2[2] += B * B;
        }    
    }     
}

void calculate_moments_for_all_blocks(Rag rag, image img, int n, int m) {
    int i, j, size;
    size = n * m;
    for (i = 0; i < size ; i++) {
        
        double m0, m1[3], m2[3];
        rag->father[i] = i;
        give_moments(img, i, n, m, &m0, m1, m2);
        rag->m[i].M0 = m0;
        for (j = 0; j < 3; j++) {
            rag->m[i].M1[j] = m1[j];
            rag->m[i].M2[j] = m2[j];
        }
    }
}

void add_neighbors(Rag rag, int n, int m) {
    int size, block_id;  
    int right_id, down_id;
    cellule new_cell;
    size = n * m;

    for (block_id = 0; block_id < size; block_id++) {
        

        if ( (block_id / n) < m - 1) {
            right_id = block_id + 1;
            
            new_cell = (cellule)malloc(sizeof(struct cellule));
            new_cell->block = right_id;
            new_cell->next = rag->neighbors;
            rag->neighbors = new_cell;
        } 
 

        if ( (block_id % n) < n - 1) {
            down_id = block_id + n;
       
            new_cell = (cellule)malloc(sizeof(struct cellule));
            new_cell->block = down_id;
            new_cell->next = rag->neighbors;
            rag->neighbors = new_cell;
            
        }
    }
}

/*Fonction create_RAG*/
Rag create_RAG(image img, int n, int m) {
    Rag rag = (Rag)malloc(sizeof(struct RAG));
    if (rag == NULL) return NULL;

    rag->size = n * m;
    rag->img = img;

    rag->m = (moments)malloc(rag->size * sizeof(struct moments));
    if (rag->m == NULL) return NULL;

    rag->father = (int *)malloc(rag->size * sizeof(int));
    if (rag->father == NULL) return NULL;

    rag->neighbors = NULL;

   
    calculate_moments_for_all_blocks(rag, img, n, m);
    add_neighbors(rag, n, m);

    return rag;
}

double min_mat(int size, double* mat, int* B1, int* B2)
{   
    int i, j, index_i, index_j; 
    double min;
    
    min = mat[0*size + 1];
    index_j = 1;
    index_i = 0;
    for(i = 0; i < size; i++ )
    {   
        for ( j = i+1; j < size; j++)
        {
            if ( min < mat[i*size + j] )
            {
                min = mat[i*size +j];
                index_i = i;
                index_j = j;
            }    
        }    
    }
    *B1 = index_i;
    *B2 = index_j;
    return min;
}

int get_father(Rag rag, int block_id)
{
    if (rag->father[block_id]==block_id)
    {
        return block_id;
    }

    rag->father[block_id] = get_father(rag, rag->father[block_id]);
    return rag->father[block_id] ;

}

/*Fonction RAG_give_closest_region */
extern double RAG_give_closest_region(Rag rag, int* B1_id, int* B2_id)
{   
    
    int i, j, k, size, father_i, father_j;   
    double* mat;

    size = rag->size;
    mat = malloc(size * size * sizeof(double));
  

    for (i = 0; i < size; i++)
    {   
        double temp, m01, m02, m11, m12;
        m01 = rag->m[i].M0;

        m11=0; 
        for ( k = 0; k < 3; k++)
        {   
            m11 += rag->m[i].M1[k] ;          
        }
        
        father_i = get_father(rag, i);

        if( father_i == i )
        {
            for ( j = i+1; j < size; j++)
            {   
                
                father_j = get_father(rag, j);
                if (father_j == j)
                {
                    m02 = rag->m[j].M0;
                    m12 = 0;
                    for ( k = 0; k < 3; k++)
                    {   
                          m12 += rag->m[j].M1[k] ;          
                    }
                }
            
                temp = (m01 * m02) / (m01 + m02) * ((m11 / m01 - m12 / m02) * (m11 / m01 - m12 / m02));
                mat[i*size + j] = temp;
            }   
        }
    } 

    return min_mat(size, mat, B1_id, B2_id); 
    

}


/*Fonction fusion_region*/
void update_adjacent_neighbors(Rag rag, int min, int max) {
    cellule current;
    int i;
    for (i = 0; i < rag->size; i++) {
        if (i != min && i != max) {
            current = rag->neighbors;

            while (current != NULL) {
                if (current->block == min) {
                    /*Remplacer `min` par `max`*/
                    current->block = max;
                }
                current = current->next;
            }
        }
    }
}
int is_in_neighbors(cellule neighbors, int block) {
    
    cellule current;
    current = neighbors;
    while (current != NULL) {
        if (current->block == block) {
            return 1; /* Le bloc est déjà présent*/
        }
        current = current->next;
    }
    return 0; /* Le bloc n'est pas présent*/
}

void merge_neighbors(Rag rag, int min, int max) {
    cellule current = rag->neighbors; /*Pointer to the global neighbor list*/ 
    cellule last = NULL; /*Pointer to track the last valid neighbor*/ 

    /*Traverse the global neighbor list*/

    while (current != NULL) {
        if (current->block == min) {
            /* Found a neighbor belonging to block "min"*/
            cellule temp = current; /*Save the current neighbor*/
            current = current->next; /*Save the current neighbor*/

            /* Add this neighbor to the "max" list (except if it is already "max")*/
            if (temp->block != max && !is_in_neighbors(rag->neighbors, temp->block)) {
                /*Add to the head of "max" neighbors*/
                temp->next = rag->neighbors; 
                rag->neighbors = temp; 
            } else {
                free(temp); /*Free memory for invalid neighbors*/
            }
        } else {
            last = current; /* Update the last valid neighbor*/
            current = current->next; /*Move to the next neighbor*/
        }
    }
    /*Final cleanup: remove all neighbors of "min"*/
    if (last != NULL) {
        last->next = NULL; /*Properly terminate the list*/
    }
}

void insert_sorted(cellule *neighbors, int block) {
    cellule new_cell, current;
    new_cell = malloc(sizeof(struct cellule));
    new_cell->block = block;
    new_cell->next = NULL;

    if (*neighbors == NULL || (*neighbors)->block > block) {
        /* Insérer au début*/
        new_cell->next = *neighbors;
        *neighbors = new_cell;
    } else {
        /*Insérer au bon endroit*/
        current = *neighbors;
        while (current->next != NULL && current->next->block < block) {
            current = current->next;
        }
        new_cell->next = current->next;
        current->next = new_cell;
    }
}


void RAG_merge_region(Rag rag, int p1, int p2)
{
    int max, min, i;
    max = (p1<p2) ? p2 : p1;
    min = (p1<p2) ? p1 : p2;

    rag->father[min] = max ;

    rag->m[max].M0 = rag->m[max].M0 + rag->m[min].M0; 

    for ( i = 0; i < 3; i++)
    {
        rag->m[max].M1[i] += rag->m[min].M1[i];
        rag->m[max].M2[i] += rag->m[min].M2[i];

    }

    merge_neighbors(rag, min, max);
    update_adjacent_neighbors(rag, min, max);   

}


/*Fonction perform_merge */

void perform_merge(Rag rag, double seuil)
{
   int B1, B2;
   double cost ;

   while(1)
   {
    cost =  RAG_give_closest_region(rag, &B1, &B2);
    if (cost > seuil)
    {
    break;
    }

    RAG_merge_region(rag, B1, B2);
    }

}


/*Fonction RAG_give_mean_color*/
extern void RAG_normalize_parents(Rag rag) {
    int i; 
    for (i = rag->size - 1; i >= 0; i--) {
        if (rag->father[i] != i) {
            rag->father[i] = rag->father[rag->father[i]];
        }
    }
}

int RAG_find_final_parent(Rag rag, int block) {
    int current, parent;
    if (rag->father[block] == block) {
        return block;
    }
    
    current = block;
    parent = rag->father[current];
    
    while (parent != rag->father[parent]) {
        current = parent;
        parent = rag->father[parent];
    }
    
    return parent;
}

extern void RAG_give_mean_color(Rag rag, int block, int* color) {
    int i;
    /*Trouver le père final du bloc*/
    int parent = RAG_find_final_parent(rag, block);
    
    /* Récupérer les moments du bloc parent*/
    struct moments* m = &(rag->m[parent]);
    
    /* Calculer la couleur moyenne*/
    if (m->M0 > 0) {
        for (i = 0; i < 3; i++) {
            /*Convertir la moyenne en valeur entière (0-255)*/
            color[i] = (int)(m->M1[i] / m->M0);
            
            /* S'assurer que la valeur est dans les limites valides*/
            if (color[i] < 0) color[i] = 0;
            if (color[i] > 255) color[i] = 255;
        }
    } else {
        /*Cas d'erreur : bloc vide*/
        for (i = 0; i < 3; i++) {
            color[i] = 0;
        }
    }
}

image create_output_image(Rag rag) {
    int block_id, y, x, n, m, block_x, block_y, block_width, block_height;
    
    /* Récupérer les dimensions de l'image originale */
    int width = image_give_largeur(rag->img);
    int height = image_give_hauteur(rag->img);
    
    /* Créer une nouvelle image de mêmes dimensions */
    image output = FAIRE_image();
    if (output == NULL) {
        return NULL;
    }
    
    /* Calculer les dimensions des blocs */
    n = sqrt(rag->size * width / height);  /* nombre de blocs en largeur */
    m = rag->size / n;                      /* nombre de blocs en hauteur */
    block_width = width / n;
    block_height = height / m;
    
    /* Parcourir chaque bloc */
    for (block_id = 0; block_id < rag->size; block_id++) {
        int color[3];
        RAG_give_mean_color(rag, block_id, color);
        
        /* Calculer les coordonnées du bloc dans l'image */
        block_x = (block_id % n) * block_width;
        block_y = (block_id / n) * block_height;
        
        /* Remplir tous les pixels du bloc avec la couleur moyenne */
        for (y = block_y; y < block_y + block_height && y < height; y++) {
            for (x = block_x; x < block_x + block_width && x < width; x++) {
                /* Remplir le tableau avec les valeurs RGB de la couleur moyenne */
                int pixel_color[3]; 
                pixel_color[0] = color[0]; 
                pixel_color[1] = color[1];
                pixel_color[2] = color[2];
                
                /* Appeler la fonction pour écrire le pixel */
                image_ecrire_pixel(output, pixel_color);  /* Passer le tableau de couleur */
            }
        }
    }
    
    return output;
}




/* Fonction utilitaire pour sauvegarder l'image*/


int save_output_image(image img, const char* filename) {
    int c, x, y;
    Point P;
    FILE* f = fopen(filename, "wb");
    if (f == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier %s en écriture\n", filename);
        return 0;
    }
    
    /* Écrire l'image au format PPM */
    fprintf(f, "P6\n%d %d\n255\n",  image_give_largeur(img),  image_give_hauteur(img));
    
    /* Écrire les données de l'image */
    for (y = 0; y < image_give_hauteur(img); y++) {
        for (x = 0; x < image_give_largeur(img); x++) {
            unsigned char pixel[3];
            for (c = 0; c < 3; c++) {
                COORDX(P) = x;
                COORDY(P) = y;
                image_move_to(img,&P);
                /* Remplacer get_pixel_component par la bonne fonction pour accéder aux composants de pixel */
                pixel[c] = (unsigned char)image_get_comp(img, &P, c);  /* Supposons que image_get_comp est la fonction à utiliser */
            }
            fwrite(pixel, 1, 3, f);
        }
    }
    
    fclose(f);
    return 1;
}
