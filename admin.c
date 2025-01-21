#include "admin.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <direct.h> 
    #define MAKE_DIR(dir) _mkdir(dir) 
#else
    #include <sys/stat.h> 
    #define MAKE_DIR(dir) mkdir(dir, 0777) 
#endif


Grid * creerGrille(){
    Grid * grille = (Grid *)malloc(sizeof(Grid));
    int i, j;

    for(i = 0; i < 9; i++){
        for(j = 0; j < 9; j++){
            grille->cell[i][j].valeur = 0;
            grille->cell[i][j].estEditable = 1;
        }
    }
    return grille;
}


Grid* modifierGrille(Grid* grille) {
    char lin, col, val;
    int ligne, colonne, valeur;

    while (1) {
        system("cls");
        afficherGrille(grille);  

        
        do {
            printf("\t\t - Enter row number (1-9) or 'S' to stop: ");
            lin = getchar();
            clearBuffer();
            if (lin == 'S' || lin == 's') {
                goodbyeMessage();
                return grille;
            }
        } while (lin < '1' || lin > '9');

        ligne = lin - '0';

        do {
            printf("\t\t - Enter column number (1-9): ");
            col = getchar();
            clearBuffer();
        } while (col < '1' || col > '9');

        colonne = col - '0';

        
        do {
            printf("\t\t - Enter value (0-9) for cell or '0' to clear: ");
            val = getchar();
            clearBuffer();
        } while (val < '0' || val > '9');

        valeur = val - '0';

        int previousValue = grille->cell[ligne - 1][colonne - 1].valeur;
        grille->cell[ligne - 1][colonne - 1].valeur = valeur;
        
        if (valeur != 0 && (!verifierLigne(grille, ligne - 1) || !verifierColonne(grille, colonne - 1) || !verifierRegion(grille, ligne - 1, colonne - 1))) {
            printf("\n\t\t - ERROR: Value %d conflicts with existing values. Re-enter.\n", valeur);
            grille->cell[ligne - 1][colonne - 1].valeur = previousValue;  
            getchar();
        } else {
            grille->cell[ligne - 1][colonne - 1].estEditable = (valeur == 0) ? 1 : 0;  
        }
    }

    return grille;
}





int chercherGrille(char *filename, int *type) {
    char c;
    int num;
    do {
        system("cls");
        fflush(stdin);
        printf("\n\n\t\t\t\t - cliquez sur '1' pour un niveau Facile \n\n");
        printf("\t\t\t\t - cliquez sur '2' pour un niveau Intermediaire \n\n");
        printf("\t\t\t\t - cliquez sur '3' pour un niveau Difficile \n\n");
        printf("\t\t\t\t - cliquez sur '4' pour retourner au menu \n\n");
        printf("\t\t\t --------------------------------------------------------------\n");
        printf("\t\t\t\t Veuillez choisir le niveau du jeu :       ");
        c = getchar();
    } while(c < '1' || c > '4');
    
    if(c != '4'){
        *type = c - '0';
        num = numeroGrille(*type);
    }
    
    switch(c) {
        case '1': sprintf(filename, "sudoku_grids/Grille F %d", num); break;
        case '2': sprintf(filename, "sudoku_grids/Grille I %d", num); break;
        case '3': sprintf(filename, "sudoku_grids/Grille D %d", num); break;
        case '4': return 0;
    }
    
    return 1;
}

int numeroGrille(int niveau){
    FILE * fichier = fopen("tablebord", "r");
    int c, num = 0;
    int i;
    
    if(fichier != NULL){
        for(i = 1; i <= 3; i++){
            fscanf(fichier, "%d %d\n", &c, &num);
            if(c == niveau) break;
        }
        fclose(fichier);
        return num + 1;
    } else {
        return 1;
    }
}

void nombreGrilles(int niveau){
    FILE * fichier = fopen("tablebord", "r");
    FILE * copie;
    int niv, num;
    int i;

    if(fichier != NULL){
        copie = fopen("fichierTMP", "w");
        for(i = 1; i <= 3; i++){
            fscanf(fichier, "%d %d\n", &niv, &num);
            if(niv == niveau){
                fprintf(copie, "%d %d\n", niv, num + 1);
            } else {
                fprintf(copie, "%d %d\n", niv, num);
            }
        }
        fclose(fichier);
        fclose(copie);
        remove("tablebord");
        rename("fichierTMP", "tablebord");
    } else {
        fclose(fichier);
        fichier = fopen("tablebord", "w");
        for(i = 1; i <= 3; i++){
            if(i == niveau){
                fprintf(fichier, "%d %d\n", i, 1);
            } else {
                fprintf(fichier, "%d %d\n", i, 0);
            }
        }
        fclose(fichier);
    }
}



Grid * genererGrilleRemplie(){
    Grid * grille = (Grid *)malloc(sizeof(Grid));
    int i, j;

    for(i = 0; i < 9; i++){
        for(j = 0; j < 9; j++){
            grille->cell[i][j].valeur = (j + i * 3 + i / 3) % 9 + 1;
            grille->cell[i][j].estEditable = 1;
        }
    }
    return grille;
}




void goodbyeMessage() {
    system("cls");  

    printf("\n\n\n");
    printf("\t\t\t  ||   *********  ********  ********  *******        ********    ***   ***   ********  || \n");
    printf("\t\t\t  ||   **         **    **  **    **  **     **      **      **  ***   ***   ***       || \n");
    printf("\t\t\t  ||   **         **    **  **    **  **     **      **      **  ***   ***   ***       || \n");
    printf("\t\t\t  ||   **         **    **  **    **  **     **      **********  ***   ***   ********  || \n");
    printf("\t\t\t  ||   **   ****  **    **  **    **  **     **      **      **  **    ***   ********  || \n");
    printf("\t\t\t  ||   **     **  **    **  **    **  **     **      **      **    *** ***   ***       || \n");
    printf("\t\t\t  ||   **     **  **    **  **    **  **     **      **      **      ** **   ***       || \n");
    printf("\t\t\t  ||   *********  ********  ********  *******        ********       ******   ********  || \n\n");
    printf("\n\n\n");

    printf("\t\t\t Press any key to exit...\n");
    getchar(); 
}



Grid * importerGrille(){
    Grid * grille = (Grid *)malloc(sizeof(Grid));
    char chemin[128];
    FILE * fich;
    int i, j;

    printf("\n\t\t\t  - entrer le chemin vers le fichier contenant la grille > ");
    clearBuffer();
    fgets(chemin, sizeof(chemin), stdin);

    if(!validerFichier(chemin)){
        printf("\n\t\t\t  - ERREUR : Le fichier est invalide , il ne correspond pas a une grille Sudoku !\n");
        free(grille);
        return NULL;
    }

    fich = fopen(chemin, "r");
    if(fich != NULL){
        for(i = 0; i < 9; i++){
            for(j = 0; j < 9; j++){
                fscanf(fich, "%d ", &grille->cell[i][j].valeur);
                if(grille->cell[i][j].valeur != 0) grille->cell[i][j].estEditable = 0;
                else grille->cell[i][j].estEditable = 1;
            }
            fscanf(fich, "\n");
        }
    } else {
        printf("\n\t\t\t  - ERREUR : Le fichier est introuvable !\n");
        free(grille);
        return NULL;
    }
    fclose(fich);
    return grille;
}

int validerFichier(char * filename){
    FILE * fich = fopen(filename, "r");
    char c;
    int cmpt = 0;

    if(fich == NULL) {
        printf("\n\t\t\t Remarque : un probleme est survenu lors de l'ouverture du fichier \n");
        return 0;
    }

    while ((c = fgetc(fich)) != EOF) {
        cmpt += 1;
        if(c == ' ' || (c <= '9' && c >= '0') || c == '\n');
        else return 0;
    }
    fclose(fich);
    if(cmpt != 171) return 0;

    return 1;
}

void choixMenuAdmin(char choix) {
    char filename[62], c;
    Grid* grille = NULL;
    int type;

    switch (choix) {
        case '1':
            printf("\n\n\n\t\t\t --------------------- CREER UNE GRILLE SUDOKU ---------------------\n\n");
            if (chercherGrille(filename, &type)) {
                do {
                    system("cls");
                    printf("\n\n\n\t\t\t\t - cliquez sur .1. pour creer une nouvelle grille pour votre jeu\n\n");
                    printf("\t\t\t\t ----------------------------------------------------------------- \n\n\t\t\t\t >");
                    clearBuffer();
                    c = getchar();
                } while (c < '1' || c > '2');

                if (c == '1') {
                    grille = creerGrille();
                    grille = modifierGrille(grille);  
                    if (grille != NULL) {
                        enregistrerGrille(filename, grille);
                        nombreGrilles(type);
                        printf("\n\t\t\t - REMARQUE : La grille (%s) a ete cree avec succes !\n", filename);
                    } else {
                        printf("\n\t\t\t - ERREUR : Une erreur est survenue lors de la modification de la grille !\n");
                    }
                } else if (c == '2') {
                    grille = importerGrille();
                    if (grille != NULL) {
                        if (verifierGrille(grille)) {
                            enregistrerGrille(filename, grille);
                            nombreGrilles(type);
                            printf("\n\t\t\t - REMARQUE : La grille (%s) a ete importee avec succes !\n", filename);
                        } else {
                            printf("\n\n\t\t\t - ERREUR : Cette grille est incorrecte !\n");
                            free(grille);
                        }
                    }
                }
                clearBuffer();
                printf("\n\t\t\t Tapez (Entrer) pour terminer > ");
                getchar();
            }
            break;
        
        case '2':
            grille = genererGrilleRemplie();
            afficherGrille(grille);
            clearBuffer();
            printf("\t\t\t Sauvegarder cette grille de test sur votre Bureau ?: (y ou n) ? ");
            c = getchar();
            if (c == 'y') {
                enregistrerGrille("C:\\Etude\\Eilco\\S5\\Algo C\\projet jeu\\annepr\\BourhanVersion\\TestSudoku", grille);
            }
            break;

        case '3':
            goodbyeMessage();
            break;

        case '4':
            consulterToutesStatistiques();
            break;
    }
    system("cls");
}


void menuAdmin() {
    _mkdir("sudoku_grids"); 
    char c;
    do {
        clearBuffer();
        printf("\n\n\n\t\t\t ---------------------------- MENU ADMIN SUDOKU ----------------------------\n\n");
        printf("\t\t\t\t - cliquez sur .1. pour creer une grille \n\n");
        printf("\t\t\t\t - cliquez sur .2. pour generer une grille de test (pre-remplie) \n\n");
        printf("\t\t\t\t - cliquez sur .4. pour consulter les statistiques \n\n");
        printf("\t\t\t\t - cliquez sur .3. pour quitter \n\n");
        printf("\t\t\t ----------------------------------------------------------------------------\n");
        printf("\t\t\t\t Faites votre choix : ");
        scanf("%c", &c);
        system("cls");
        choixMenuAdmin(c);
    } while(c != '3');
}


int consulterToutesStatistiques() {
    Partie partie;
    int found = 0;
    FILE *fich = fopen("statistiques", "r");
    if (!fich) {
        printf("\n\n\t\t\t Alerte : aucune statistique n'a été établie pour vous !\n");
        printf("\n\t\t\t Tapez (Entrer) pour retourner au menu > ");
        clearBuffer();
        getchar();
        return 0;
    }

    printf("\n\t|----------------------|----------------------|----------------------|----------------------|\n");
    printf("\t| %-20s | %-20s | %-20s | %-20s |\n", "Joueur", "Niveau", "Temps", "Progression");
    printf("\t|----------------------|----------------------|----------------------|----------------------|\n");

    while (fscanf(fich, "%d %49s %49s %d %d", &partie.id, partie.joeur, partie.niveau, &partie.temps, &partie.progression) == 5) {
        int heures = partie.temps / 3600;
        int minutes = (partie.temps % 3600) / 60;
        int secondes = partie.temps % 60;
        printf("\t| %-20s | %-20s | %2d h %2d min %2d s     | %-20d/81  |\n",
               partie.joeur, partie.niveau, heures, minutes, secondes, partie.progression);
        found = 1;
    }

    fclose(fich);

    if (!found) {
        printf("\n\t\t\t --- Aucune statistique trouvée pour le joueur ---\n");
    }

    printf("\n\t\t\t Tapez (Entrer) pour retourner au menu > ");
    clearBuffer();
    getchar();

    return found;
}