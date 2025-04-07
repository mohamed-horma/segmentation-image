#ifndef IMAGE_H
#define IMAGE_H

#include "objet.h"
#include "classe.h"

#include "point.h"
#include "move_type.h"


/* Type de fonction a utiliser pour calculer la distance entre 2 pixel */
typedef double (*PFdist)(int*,int*,bouleen);
 
CLASSE(image);

#define NULL_IMAGE ((image)NULL)

extern image     FAIRE_image();
extern void      DEFAIRE_image(image);

/* Return a negative value in case of error */
/* -1 : unable to open file */
/* -2 : Unrecognized format */
/* -3 : Wrong values for height, width, of max value */
extern int       image_charger    (image,char*);
extern int       image_sauvegarder(image,char*);
extern int       image_to_stream  (image,FILE*);

extern int*      image_lire_pixel  (image);
extern int       image_get_comp(image,point,int);
extern void      image_ecrire_pixel(image,int*);

extern bouleen   image_pixel_suivant(image);
extern void      image_debut        (image);
extern bouleen   image_move_to      (image,point);
extern bouleen   image_pixel_dessus (image);
extern bouleen   image_pixel_dessous(image);
extern bouleen   image_pixel_gauche (image);
extern bouleen   image_pixel_droite (image);

extern void      image_set_distance(image,PFdist);
/* img,dim,largeur,hauteur */
extern void      image_initialize(image,int,int,int);
extern double    image_distance(image,point,move_type);

extern int       image_give_dim    (image);
extern int       image_give_hauteur(image);
extern int       image_give_largeur(image);
#endif /* IMAGE_H */



