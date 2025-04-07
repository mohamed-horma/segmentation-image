#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "RAG.h"

int main(int argc, char* argv[]) {
    /* Déclaration des variables */
    Rag rag;
    FILE *file;
    image input_image, output_image;
    int nb_blocks_height, nb_blocks_width;
    double threshold;

    /* Vérification du nombre d'arguments */
    if (argc < 5) {
        printf("Usage: %s <image_path> <nb_blocks_width> <nb_blocks_height> <threshold>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Conversion des arguments et gestion des erreurs */
    nb_blocks_width = atoi(argv[2]);
    if (nb_blocks_width <= 0) {
        printf("Erreur : le nombre de blocs en largeur doit être supérieur à 0.\n");
        return EXIT_FAILURE;
    }

    nb_blocks_height = atoi(argv[3]);
    if (nb_blocks_height <= 0) {
        printf("Erreur : le nombre de blocs en hauteur doit être supérieur à 0.\n");
        return EXIT_FAILURE;
    }

    threshold = atof(argv[4]);
    if (threshold <= 0) {
        printf("Erreur : le seuil doit être supérieur à 0.\n");
        return EXIT_FAILURE;
    }

    /* Initialisation de l'image d'entrée */
    input_image = FAIRE_image();
    if (image_charger(input_image, argv[1]) < 0) {
        printf("Erreur : Impossible de charger l'image d'entrée.\n");
        return EXIT_FAILURE;
    }

    /* Création du RAG */
    rag = create_RAG(input_image, nb_blocks_width, nb_blocks_height);
    if (rag == NULL) {
        printf("Erreur : Impossible de créer le RAG.\n");
        DEFAIRE_image(input_image);
        return EXIT_FAILURE;
    }

    /* Appel de la fonction de fusion */
    perform_merge(rag, threshold);

    /* Création de l'image résultat */
    output_image = create_output_image(rag);
    if (output_image == NULL) {
        printf("Erreur : Impossible de créer l'image de sortie.\n");
        DEFAIRE_image(input_image);
      
        return EXIT_FAILURE;
    }

    /* Ouverture du fichier en mode binaire pour écrire l'image */
    file = fopen("./fichier/output.ppm", "wb");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier output.ppm pour l'écriture.\n");
        DEFAIRE_image(input_image);
        DEFAIRE_image(output_image);
        return EXIT_FAILURE;
    }

    /* Envoi de l'image de sortie vers le fichier */
    if (image_to_stream(output_image, file) < 0) {
        printf("Erreur : Impossible d'écrire l'image dans le fichier.\n");
        fclose(file); 
        DEFAIRE_image(input_image);
        DEFAIRE_image(output_image);
        return EXIT_FAILURE;
    }

    /* Fermeture du fichier après l'écriture */
    fclose(file);

    /* Libération des ressources */
    DEFAIRE_image(input_image);
    DEFAIRE_image(output_image);
   

    return EXIT_SUCCESS;
}
