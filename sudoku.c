#include "sudoku.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#ifdef _WIN32
    #include <direct.h> 
    #define MAKE_DIR(dir) _mkdir(dir)
#else
    #include <sys/stat.h> 
    #define MAKE_DIR(dir) mkdir(dir, 0777) 
#endif

#include "partie.h"

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define ORANGE "\033[0;33m"  
#define RESET "\033[0m"


// Charger une grille de Sudoku depuis un fichier
// filename : nom du fichier contenant les données de la grille
// Retourne un pointeur vers la grille allouée dynamiquement ou NULL en cas d'erreur
Grid * chargerGrille(char * filename) {
    int i, j;
    FILE * fich = fopen(filename, "r");
    
    if (fich == NULL) {
        printf("Error: Could not open file %s\n", filename); 
        return NULL;
    }
    
    printf("Debug: File %s opened successfully.\n", filename); 
    
    Grid * grille = (Grid*) malloc(sizeof(Grid));
    if (grille == NULL) {
        printf("Error: Memory allocation failed for grid structure.\n");
        fclose(fich); 
        return NULL;
    }
    
    printf("Debug: Memory allocated for grid structure. Loading grid data...\n");

    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            if (fscanf(fich, "%d %d", &grille->cell[i][j].valeur, &grille->cell[i][j].estEditable) != 2) {
                printf("Error: Incorrect file format in %s. Expected 2 values per cell.\n", filename);
                free(grille); 
                fclose(fich);
                return NULL;
            }
        }
    }
    
    fclose(fich);
    printf("Debug: Grid data loaded successfully from %s\n", filename); 
    return grille;
}

// Vérifie si une ligne respecte les règles du Sudoku (pas de doublons)
// grille : pointeur vers la grille à vérifier
// ligne : indice de la ligne (0-8) à vérifier
// Retourne 1 si la ligne est valide, 0 sinon
int verifierLigne(Grid * grille, int ligne) {
    int j, k;
    for(j = 0; j < 9; j++) {
        if(grille->cell[ligne][j].valeur != 0) {
            for(k = 0; k < 9; k++) {
                if(grille->cell[ligne][j].valeur == grille->cell[ligne][k].valeur && k != j) return 0;
            }
        }
    }
    return 1;
}

// Vérifie si une colonne respecte les règles du Sudoku (pas de doublons)
// grille : pointeur vers la grille à vérifier
// colonne : indice de la colonne (0-8) à vérifier
// Retourne 1 si la colonne est valide, 0 sinon
int verifierColonne(Grid * grille, int colonne) {
    int i, k;
    for(i = 0; i < 9; i++) {
        if(grille->cell[i][colonne].valeur != 0) {
            for(k = 0; k < 9; k++) {
                if(grille->cell[i][colonne].valeur == grille->cell[k][colonne].valeur && k != i) return 0;
            }
        }
    }
    return 1;
}

// Vérifie si une région 3x3 respecte les règles du Sudoku (pas de doublons)
// grille : pointeur vers la grille à vérifier
// ligne : indice de la ligne de départ de la région (0-8)
// colonne : indice de la colonne de départ de la région (0-8)
// Retourne 1 si la région est valide, 0 sinon
int verifierRegion(Grid * grille, int ligne, int colonne) {
    int i, j, k, l;
    ligne = (ligne / 3) * 3;
    colonne = (colonne / 3) * 3;
    for(i = ligne; i < ligne + 3; i++) {
        for(j = colonne; j < colonne + 3; j++) {
            if(grille->cell[i][j].valeur != 0) {
                for(k = ligne; k < ligne + 3; k++) {
                    for(l = colonne; l < colonne + 3; l++) {
                        if(grille->cell[i][j].valeur == grille->cell[k][l].valeur && i != k && j != l) return 0;
                    }
                }
            }
        }
    }
    return 1;
}

// Vérifie si toute la grille respecte les règles du Sudoku
// grille : pointeur vers la grille à vérifier
// Retourne 1 si toute la grille est valide, 0 sinon
int verifierGrille(Grid* grille) {
    int i, j;
    for(i = 0; i < 9; i++) {
        if(!verifierLigne(grille, i)) return 0;
    }
    for(j = 0; j < 9; j++) {
        if(!verifierColonne(grille, j)) return 0;
    }
    for(i = 0; i < 9; i += 3) {
        for(j = 0; j < 9; j += 3) {
            if(!verifierRegion(grille, i, j)) return 0;
        }
    }
    return 1;
}






void afficherGrille(Grid * grille) {
    int i, j;
    printf(ORANGE "\n\n\t\t\t\t                           JEU SUDOKU\n\n" RESET);
    printf(ORANGE "\t\t\t\t    1     2     3         4     5     6         7     8     9   \n" RESET);
    for(i = 0; i < 9; i++) {
        printf("\t\t\t\t +-----+-----+-----+   +-----+-----+-----+   +-----+-----+-----+\n");
        printf(ORANGE "\t\t\t\t%d|" RESET , i + 1);
        for(j = 0; j < 9; j++) {
            if(grille->cell[i][j].valeur != 0) {
                if(grille->cell[i][j].estEditable == 1) printf("  %d  ", grille->cell[i][j].valeur);
                if(grille->cell[i][j].estEditable == 0) printf("  %d* ", grille->cell[i][j].valeur);
            } else printf("     ");
            printf("|");
            if(j == 2 || j == 5) printf("   |");
        }
        printf("\n");
        if(i == 2 || i == 5) printf("\t\t\t\t +-----+-----+-----+   +-----+-----+-----+   +-----+-----+-----+\n\n");
    }
    printf("\t\t\t\t +-----+-----+-----+   +-----+-----+-----+   +-----+-----+-----+\n\n");
}



int enregistrerGrille(const char* filename, Grid* grille) {
    FILE* fich = fopen(filename, "w");
    if (fich == NULL) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return 0;
    }

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            fprintf(fich, "%d %d ", grille->cell[i][j].valeur, grille->cell[i][j].estEditable);

        }
        fprintf(fich, "\n");
    }
    fclose(fich);

    printf("Debug: Grid saved successfully to %s\n", filename);
    return 1;
}









int progressionJeu(Grid * grid) {
    int i, j, progress = 0;
    for(i = 0; i < 9; i++) {
        for(j = 0; j < 9; j++) {
            if(grid->cell[i][j].valeur != 0) progress += 1;
        }
    }
    return progress;
}


int totalGrille(int niveau) {
    int i, c, num = 0;
    FILE * fichier = fopen("tablebord", "r");
    
    if(fichier == NULL) {
        printf("Debug: Could not open 'tablebord' file.\n");
        return 0;
    }

    int found = 0;
    for(i = 1; i <= 3; i++) {
        if (fscanf(fichier, "%d %d\n", &c, &num) == 2) { 
            printf("Debug: Read level %d with %d grids.\n", c, num);  
            if(c == niveau) {
                found = 1;
                break;
            }
        } else {
            printf("Debug: Unexpected file format in 'tablebord'.\n");
            num = 0;
            break;
        }
    }
    fclose(fichier);

    if (!found) {
        printf("Debug: Level %d not found in 'tablebord'. Returning 0.\n", niveau);
        num = 0;
    }

    return num;
}


void initialisation(char * pseudo) {
    int valid, i;
    const char chars[] = {219, 219, 219, 219, 219, 219, 219, 219, 219, 219};
    do {
        system("cls");
        afficherSudoku();
        printf("\n\n");
        printf(RED "\n\t\t\t      Veuiller entrer votre Nom (Doit pas depasser 15 caracteres) > " RESET);
        fflush(stdin);
        fgets(pseudo, sizeof(pseudo), stdin);
        if(!verifierPseudo(pseudo)) {
            printf("\n\t\t\t ERREUR : veuillez saisir au moins 3 caracteres pour votre pseudo sans espaces ! ");
            printf("\n\t\t\t ERREUR : un pseudo ne doit pas depasser les 15 caracteres ! ");
            sleep(3);
            valid = 0;
        } else valid = 1;
    } while(!valid);

    printf("\n\n\t\tVeuillez patienter ........... \n\t\t");
    for(i = 0; i < 50; ++i) {
        printf("%c", chars[i % sizeof(chars)]);
        fflush(stdout);
        usleep(100 * 1000);
    }
    system("cls");
}


int fileExists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

int choisirGrille(char *filename, char *niveau) {
    char c;
    int num;

    do {
        system("cls");
        printf(RED"\n\n\t\t\t -------------------- Niveaux de difficulte ---------------------\n\n"RESET);
        printf("\t\t\t|\t - Entrez '1' pour un niveau Facile \t\t\t|\n\n");
        printf("\t\t\t|\t - Entrez '2' pour un niveau Intermediaire \t\t|\n\n");
        printf("\t\t\t|\t - Entrez '3' pour un niveau Difficile \t\t\t|\n\n");
        printf("\t\t\t|\t - Entrez '4' pour retourner au menu principal \t\t|\n\n");
        printf("\n\t\t\t ----------------------------------------------------------------\n");
        printf("\t\t\t Veuillez choisir le niveau de difficulte du jeu : ");
        
        clearBuffer();
        c = getchar();
        printf("Debug: Chosen level character: %c\n", c); 
    } while(c < '1' || c > '4');

    if (c == '4') {
        printf("Debug: Returning to main menu as '4' was chosen.\n");
        return 0;
    }

    srand(time(NULL));
    int total = totalGrille(c - '0');
    printf("Debug: Total grids available for level %d: %d\n", c - '0', total);

    if (total == 0) {
        printf("Debug: No grids available for the chosen level.\n");
        return 0;
    }

    int attempts = total;
    do {
        num = rand() % total + 1;
        switch (c) {
            case '1':
                sprintf(filename, "sudoku_grids/Grille F %d", num);
                strcpy(niveau, "Facile");
                break;
            case '2':
                sprintf(filename, "sudoku_grids/Grille I %d", num);
                strcpy(niveau, "Intermediaire");
                break;
            case '3':
                sprintf(filename, "sudoku_grids/Grille D %d", num);
                strcpy(niveau, "Difficile");
                break;
        }

        printf("Debug: Trying to load grid file: %s\n", filename); 

        if (fileExists(filename)) {
            printf("Debug: File found: %s\n", filename); 
            return 1;
        }

        printf("Debug: File %s does not exist. Retrying...\n", filename);
        attempts--;

    } while (attempts > 0);

    
    printf("Error: No valid grid files found for the selected level.\n");
    return 0;
}




void choisirCellule(Grid * grille, int * ligne, int * colonne) {
    char lin, col;
    fflush(stdin);
    do {
        do {
            printf("\t\t - entrer le numero de la ligne :");
            scanf("%c", &lin);
            fflush(stdin);
            if(lin > '9' || lin < '1') {
                system("cls");
                afficherGrille(grille);
                printf("\n\t\t - ERREUR : le numero de la ligne est invalide !\n\n");
            }
        } while(lin > '9' || lin < '1');
        *ligne = lin - '0';

        do {
            printf("\t\t - entrer le numero de la colonne :");
            scanf("%c", &col);
            fflush(stdin);
            if(col > '9' || col < '1') {
                system("cls");
                afficherGrille(grille);
                printf("\n\t\t - ERREUR : le numero de la colonne est invalide !\n\n");
                printf("\t\t - entrer le numero de la ligne : %c \n", lin);
            }
        } while(col > '9' || col < '1');
        *colonne = col - '0';

        if(grille->cell[*ligne - 1][*colonne - 1].estEditable == 0) {
            system("cls");
            afficherGrille(grille);
            printf("\n\t\t - ERREUR : Les indices initials de la grille ne sont pas modifiable\n\n");
        }
    } while(!grille->cell[*ligne - 1][*colonne - 1].estEditable);
}

void remplirCellule(Grid * grille) {
    int ligne, colonne, valeur, v;
    char val;
    int conflict = 0; // Tracks if there's a conflict

    fflush(stdin);
    choisirCellule(grille, &ligne, &colonne);
    v = grille->cell[ligne - 1][colonne - 1].valeur;

    do {
        conflict = 0; 
        printf("\t\t - entrer la valeur de la case (saisir 0 pour supprimer) :");
        scanf(" %c", &val);
        fflush(stdin);

        
        if (val > '9' || val < '0') {
            system("cls");
            afficherGrille(grille);
            printf("\n\t\t - ERREUR : la valeur saisie est invalide !\n\n");
            printf("\t\t - entrer le numero de la ligne : %d \n", ligne);
            printf("\t\t - entrer le numero de la colonne : %d \n", colonne);
        } else {
           
            valeur = val - '0';
            grille->cell[ligne - 1][colonne - 1].valeur = valeur;

            
            if (!verifierLigne(grille, ligne - 1)) {
                conflict = 1;
                grille->cell[ligne - 1][colonne - 1].valeur = v; 
                system("cls");
                afficherGrille(grille);
                printf("\n\t\t - ERREUR : cet indice existe deja dans la ligne %d !\n\n", ligne);
                printf("\t\t - numero de la ligne : %d \n", ligne);
                printf("\t\t - numero de la colonne : %d \n", colonne);
                printf("\t\t - valeur invalide : %c \n", val);
            } else if (!verifierColonne(grille, colonne - 1)) {
                conflict = 1;
                grille->cell[ligne - 1][colonne - 1].valeur = v; 
                system("cls");
                afficherGrille(grille);
                printf("\n\t\t - ERREUR : cet indice existe deja dans la colonne %d !\n\n", colonne);
                printf("\t\t - numero de la ligne : %d \n", ligne);
                printf("\t\t - numero de la colonne : %d \n", colonne);
                printf("\t\t - valeur invalide : %c \n", val);
            } else if (!verifierRegion(grille, ligne - 1, colonne - 1)) {
                conflict = 1;
                grille->cell[ligne - 1][colonne - 1].valeur = v; 
                system("cls");
                afficherGrille(grille);
                printf("\n\t\t - ERREUR : cet indice existe deja dans cette region !\n\n");
                printf("\t\t - numero de la ligne : %d \n", ligne);
                printf("\t\t - numero de la colonne : %d \n", colonne);
                printf("\t\t - valeur invalide : %c \n", val);
            }
        }
    } while (val < '0' || val > '9' || conflict); 

    system("cls");
    afficherGrille(grille);
}


double calculerTemps(time_t * debut, double * temps) {
    time_t start = *debut;
    time_t fin;
    time(&fin);
    *debut = fin;
    *temps += difftime(fin, start);
    return difftime(fin, start);
}

Grid * remplirGrille(Grid * grille, int * progression, int * duree, Partie *partie) {
    char c;
    time_t debut;
    double temps = 0.0;
    int progress;
    char filename[50];

    
    int len = snprintf(filename, sizeof(filename), "%.20s_%.20s_save.txt", partie->joeur, partie->niveau);
    if (len >= (int)sizeof(filename)) {  
        fprintf(stderr, "Warning: Truncated filename\n");
    }

    time(&debut);

    do {
       
        system("cls");
        afficherGrille(grille);
        progress = progressionJeu(grille);
        printf("\t\t\t\t progression %d/81 - (%.0f%%)   ", progress, (float)(progress * 100) / 81);
        calculerTemps(&debut, &temps);
        printf("|   \t temps :: %.0f sec. \n\n", temps);

       
        printf(GREEN "\n\t --------- Options: (Entree) Pour entrer une valeur - (A) Help - (S) Sauvegarder et Sortir ---------\n\t >" RESET);

        
        clearBuffer();
        c = getchar();

        
        if (c == 'S' || c == 's') {
            
            printf("Enregistrement de la partie en cours...\n");
            enregistrerGrille(filename, grille); 
            *progression = progressionJeu(grille);
            *duree = (int)temps;


            partie->progression = *progression;
            partie->temps = *duree;
            if (ajouterStatistiques(partie, "statistiques")) {
                printf(GREEN "Statistiques enregistrées avec succès.\n" RESET);
            } else {
                printf(RED "Erreur : Échec de l'enregistrement des statistiques.\n" RESET);
            }
            return NULL;  
            
        } else if (c == 'A' || c == 'a') {
            int ligne, colonne;
            choisirCellule(grille, &ligne, &colonne);
            grille->cell[ligne - 1][colonne - 1].valeur = proposerAide(grille, ligne, colonne);
            continue; 
        } else if (c == '\n') {
            remplirCellule(grille);
        }

        if (progressionJeu(grille) == 81) {
            if (verifierGrille(grille)) {
                printf("\n\n\t\t\t ------------- :) Felicitation vous avez complete cette partie! (: -------------\n\n");
                getchar(); 
                return grille; 
            } else {
                printf("\n\n\t\t\t ---------------- :( Grille incorrecte  ): ----------------\n\n");
            }
        }

    } while (c != 'S' && c != 's'); 

    *progression = progressionJeu(grille);
    *duree = (int)temps;
    return grille;
}






void reglesSudoku() {
    afficherSudoku();
    printf(RED "\n\n\t\t ------------------------------------ Regles du Sudoku ------------------------------------- \n\t\t|");
    printf(GREEN" 1. Le Sudoku se joue sur une grille 9x9.                                                   |\n\t\t|");
    printf(GREEN" 2. Chaque ligne doit contenir les chiffres de 1 a 9, sans repetition.                      |\n\t\t|");
    printf(GREEN" 3. Chaque colonne doit egalement contenir les chiffres de 1 a 9, sans repetition.          |\n\t\t|" );
    printf(GREEN" 4. Chaque sous-grille 3x3 (appelee region) doit contenir les chiffres [1-9]                |\n\t\t|");
    printf(GREEN" 5. Une grille de Sudoku initiale est partiellement remplie avec des chiffres,              |\n\t\t|");
    printf(GREEN"    l'objectif est de completer la grille.                                                  |\n\t\t|");
    printf(GREEN" 6. Aucun chiffre ne doit se repeter dans une meme ligne, colonne ou region.                |\n\t\t|");
    printf(GREEN" 7. La solution d'une grille de Sudoku est unique.                                          |\n\t\t"RESET);
    printf(RED" ------------------------------------------------------------------------------------------- \n" RESET);
}

int proposerAide(Grid * grille, int ligne, int colonne) {
    int i, j;
    Grid * grilleCopy = (Grid *)malloc(sizeof(Grid));
    for(i = 0; i < 9; i++) {
        for(j = 0; j < 9; j++) {
            grilleCopy->cell[i][j].valeur = grille->cell[i][j].valeur;
        }
    }
    resoudreSudoku(grilleCopy);
    return grilleCopy->cell[ligne - 1][colonne - 1].valeur;
}

int verifierCellule(Grid * grille, int num, int lin, int col) {
    int i, j;
    for(i = 0; i < 9; i++) {
        if(grille->cell[lin][i].valeur == num || grille->cell[i][col].valeur == num) {
            return 0;
        }
    }
    int ligne = lin - lin % 3;
    int colonne = col - col % 3;
    for(i = ligne; i < ligne + 3; i++) {
        for(j = colonne; j < colonne + 3; j++) {
            if(grille->cell[i][j].valeur == num) {
                return 0;
            }
        }
    }
    return 1;
}

int resoudreSudoku(Grid * grille) {
    int lin, col, num;
    int plein = 1;
    for(lin = 0; lin < 9; lin++) {
        for(col = 0; col < 9; col++) {
            if(grille->cell[lin][col].valeur == 0) {
                plein = 0;
                for(num = 1; num <= 9; num++) {
                    if(verifierCellule(grille, num, lin, col)) {
                        grille->cell[lin][col].valeur = num;
                        if(resoudreSudoku(grille)) {
                            return 1;
                        }
                        grille->cell[lin][col].valeur = 0;
                    }
                }
                return 0;
            }
        }
    }
    return plein;
}

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void choix(char choix, char *pseudo) {
    char c, filename[150];
    Grid *grille = NULL;
    Partie partie;
    strcpy(partie.joeur, pseudo);
    int statut;

    switch (choix) {
        case '1': {
            if (!choisirGrille(filename, partie.niveau)) {
                break;
            }

            grille = chargerGrille(filename);
            if (grille == NULL) {
                printf("Error: Unable to load the selected grid. Returning to the menu...\n");
                break;
            }

            grille = remplirGrille(grille, &partie.progression, &partie.temps, &partie);

            if (grille != NULL) {
                partie.id = totalParties(pseudo) + 1;

                // Increase buffer size and truncate inputs
                char filename[150];
                snprintf(filename, sizeof(filename), "%.20s_%.20s_%d.txt", partie.joeur, partie.niveau, partie.id);
                ajouterStatistiques(&partie, "statistiques");
                enregistrerGrille(filename, grille);
                
                printf(GREEN "Partie enregistrée sous le nom : %s\n" RESET, filename);
            } else {
                printf("Game progress saved for next time.\n");
            }
            break;
        }
        case '2': { // Continue Game
            statut = choisirPartie(filename, pseudo);
            if (statut == 1) {
                grille = chargerGrille(filename);
            } else if (statut == 2) {
                break;
            }

            if (grille != NULL) {
                char niveau[50] = "unknown";
                char *underscore1 = strchr(filename, '_');
                char *underscore2 = strchr(underscore1 + 1, '_');
                if (underscore1 != NULL && underscore2 != NULL) {
                    size_t niveauLength = underscore2 - (underscore1 + 1);
                    strncpy(niveau, underscore1 + 1, niveauLength);
                    niveau[niveauLength] = '\0';
                }

                char newFilename[150];
                int saveCounter = 1;

                do {
                    snprintf(newFilename, sizeof(newFilename), "%s_%s_save%d.txt", pseudo, niveau, saveCounter);
                    saveCounter++;
                } while (fileExists(newFilename));

                grille = remplirGrille2(grille, &partie.progression, &partie.temps, newFilename);
            } else {
                printf("\n\n\t\t\t --------- Vous n'avez commencé aucune partie! ---------\n\t");
                clearBuffer();
                printf("\n\t\t\t Tapez (Entrer) pour retourner au menu principal > ");
                getchar();
            }
            break;
        }

        case '3':
            reglesSudoku();
            clearBuffer();
            printf("\n\t\t\t (Entrer) pour retourner au menu principal >");
            getchar();
            break;

        case '4':
            do {
                system("cls");
                printf(RED"\n\n\n\t\t\t ---------------------------- LES STATISTIQUES ----------------------------\n\n"RESET);
                printf("\t\t\t\t - Cliquez sur '1' pour afficher vos statistiques. \n\n");
                printf("\t\t\t\t - Cliquez sur '3' pour retourner au menu \n\n");
                printf("\t\t\t ----------------------------------------------------------------------------\n");
                printf("\t\t\t\t Faites votre choix > ");
                clearBuffer();
                scanf(" %c", &c);
            } while (c > '3' || c < '1');
            system("cls");
            if (c == '1') {
                chercherStatistique(pseudo);
                clearBuffer();
                printf("\n\t\t\t (Entrer) pour retourner au menu principal >");
                getchar();
            } else if (c == '2') {
                afficherStatistiques();
                clearBuffer();
                printf("\n\t\t\t (Entrer) pour retourner au menu principal >");
                getchar();
            }
            break;

        case '5':
            printf("Quitter\n");
            break;
    }
    system("cls");
}


void menuSudoku() {
    char pseudo[50];
    initialisation(pseudo);
    char c;
    do {
        fflush(stdin);
        printf(YELLOW"\n\n\t\t\t ----------------------------------------------------------------------------------------\n"RESET);
        printf(BLUE"\t\t\t| \t\t\t Bonjour %s, Bienvenue dans le jeu du SUDOKU.  \t\t|\n"RESET , pseudo);    
        printf(YELLOW"\n\t\t\t ------------------------------------ MENU DU JEU  --------------------------------\n\n"RESET);
        printf("\t\t\t|\t - Appuyez sur .1. Commencer une nouvelle partie. \t\t\t\t|\n\n");
        printf("\t\t\t|\t - Appuyez sur .2. Continuer votre derniere partie\t\t\t\t|\n\n");
        printf("\t\t\t|\t - Appuyez sur .3. Voirs les regles du SUDOKU.\t\t\t\t\t|\n\n");
        printf("\t\t\t|\t - Appuyez sur .4. Consulter votres statistiques.\t\t\t\t|\n\n");
        printf("\t\t\t|\t - Appuyez sur .5. Connectez comme Admin. \t\t\t\t\t|\n");
        printf(YELLOW"\n\t\t\t -----------------------------------------------------------------------------------------\n"RESET);
        printf("\t\t\t Entrez votre choix > ");
        scanf("%c", &c);
        system("cls");
        choix(c, pseudo);
    } while(c != '5');
}

void sauvegarderPartie(Grid *grille, Partie *partie, const char *filename) {
    if (enregistrerGrille(filename, grille)) {
        printf(GREEN "\nLa partie a été sauvegardée avec succès dans le fichier: %s\n" RESET, filename);

        // Optionally update metadata/statistics
        ajouterStatistique(partie, "statistiques");
    } else {
        printf(RED "\nErreur : Impossible de sauvegarder la partie.\n" RESET);
    }
}


Grid * remplirGrille2(Grid * grille, int * progression, int * duree, char *filename) {
    char c;
    time_t debut;
    double temps = 0.0;
    int progress;

    time(&debut);

    do {
        system("cls");
        afficherGrille(grille);
        progress = progressionJeu(grille);
        printf("\t\t\t\t progression %d/81 - (%.0f%%)   ", progress, (float)(progress * 100) / 81);
        calculerTemps(&debut, &temps);
        printf("|   \t temps :: %.0f sec. (en calcul...)\n\n", temps);

        printf(GREEN "\n\t --------- Options: (Entree) Continuer - (A) Aide - (S) Sauvegarder et Sortir ---------\n\t >" RESET);

        clearBuffer();
        c = getchar();

        if (c == 'S' || c == 's') {
            
            printf("Enregistrement de la partie en cours...\n");
            enregistrerGrille(filename, grille);
            *progression = progressionJeu(grille);
            *duree = (int)temps;
            return NULL; 
        } else if (c == 'A' || c == 'a') {
            int ligne, colonne;
            choisirCellule(grille, &ligne, &colonne);
            grille->cell[ligne - 1][colonne - 1].valeur = proposerAide(grille, ligne, colonne);
            continue;
        } else if (c == '\n') {
            remplirCellule(grille);
        }

        if (progressionJeu(grille) == 81) {
            if (verifierGrille(grille)) {
                printf("\n\n\t\t\t ------------- :) Félicitation vous avez complété cette partie! (: -------------\n\n");
                getchar();
                return grille; 
            } else {
                printf("\n\n\t\t\t ---------------- :( Grille incorrecte ): ----------------\n\n");
            }
        }
    } while (c != 'S' && c != 's');

    *progression = progressionJeu(grille);
    *duree = (int)temps;
    return grille;
}

int ajouterStatistiques(Partie *partie, char *filename) {
    FILE *fich = fopen(filename, "a");
    if (fich == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return 0;
    }

    // Affichez les données qui seront écrites
    printf("Debug: Écriture des statistiques - ID: %d, Joueur: %s, Niveau: %s, Temps: %d, Progression: %d\n",
           partie->id, partie->joeur, partie->niveau, partie->temps, partie->progression);

    if (fprintf(fich, "%d %s %s %d %d\n", partie->id, partie->joeur, partie->niveau, partie->temps, partie->progression) < 0) {
        perror("Erreur lors de l'écriture dans le fichier");
        fclose(fich);
        return 0;
    }

    fclose(fich);
    return 1;

}

