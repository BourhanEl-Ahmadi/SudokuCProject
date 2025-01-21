#include <stdio.h>
#include <stdlib.h>


#include <string.h>
#include "partie.h"

void afficherSudoku(){
    printf( "\n\n\t\t\t  ||    /******  /**   /** /*******   /******  /**   /** /**   /** || \n");
    printf( "\t\t\t  ||   /**__  **| **  | **| **__  ** /**__  **| **  /**/| **  | ** ||  \n");
    printf("\t\t\t  ||  | **  \\__/| **  | **| **  \\ **| **  \\ **| ** /**/ | **  | ** ||  \n");
    printf("\t\t\t  ||  |  ****** | **  | **| **  | **| **  | **| *****/  | **  | ** ||  \n");
    printf("\t\t\t  ||   \\____  **| **  | **| **  | **| **  | **| **  **  | **  | ** ||   \n");
    printf("\t\t\t  ||   /**  \\ **| **  | **| **  | **| **  | **| **\\  ** | **  | ** ||  \n");
    printf("\t\t\t  ||  |  ******/|  ******/| *******/|  ******/| ** \\  **|  ******/ ||  \n");
    printf("\t\t\t  ||   \\______/  \\______/ |_______/  \\______/ |__/  \\__/ \\______/  ||   \n\n" );
}

int verifierPseudo(char * pseudo){
    size_t i;
    if(strlen(pseudo) < 3) return 0;
    if(strlen(pseudo) > 15) return 0;
    for(i = 0; i < strlen(pseudo); i++) {
        if(pseudo[i] == ' ') return 0;
    }
    return 1;
}

int ajouterStatistique(Partie *partie, char *filename) {
    FILE *fich = fopen(filename, "a");
    if (fich == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return 0;
    }
    
    fprintf(fich, "%d %s %s %d %d\n", partie->id, partie->joeur, partie->niveau, partie->temps, partie->progression);
    fclose(fich);
    return 1;
}

int modifierStatistique(Partie * partie){
    Partie part;
    FILE * fich = fopen("statistiques", "r");
    int cmpt = 0, ligne;
    if(fich == NULL) return 0;

    ligne = chercherJoueur(partie->joeur);
    if(ligne == 0) return 0;

    while(!feof(fich)){
       cmpt++;
       fscanf(fich, "%d %s %s %d %d\n", &part.id, part.joeur, part.niveau, &part.temps, &part.progression);
       if(strcmp(part.joeur, partie->joeur) == 0 && cmpt == ligne) {
            strcpy(partie->niveau, part.niveau);
            partie->temps += part.temps;
            partie->id = part.id;
            ajouterStatistique(partie, "copieStatistiques");
       } else {
            ajouterStatistique(&part, "copieStatistiques");
       }
    }
    fclose(fich);
    remove("statistiques");
    rename("copieStatistiques", "statistiques");
    return 1;
}

void afficherStatistiques(char *pseudo) {
    Partie partie;
    FILE *fich = fopen("statistiques", "r");
    int heures, minutes, secondes;
    char temps[20], progression[10];
    int found = 0;

    if (fich == NULL) {
        printf("\n\t\t\t Alerte : aucune statistique n'a ete enregistree jusqu'à present !\n");
        return;
    }

    printf("\n\n\t\t--- Statistiques pour le joueur : %s ---\n", pseudo);
    printf("\n\t|----------------------|----------------------|----------------------|----------------------|\n");
    printf("\t| %-20s | %-20s | %-20s | %-20s |\n", "ID Partie", "Niveau", "Temps", "Progression");
    printf("\t|----------------------|----------------------|----------------------|----------------------|\n");

    // Parcours du fichier des statistiques
    while (fscanf(fich, "%d %s %s %d %d\n", &partie.id, partie.joeur, partie.niveau, &partie.temps, &partie.progression) == 5) {
        if (strcmp(partie.joeur, pseudo) == 0) {
            formatTemps(partie.temps, &heures, &minutes, &secondes);
            snprintf(temps, sizeof(temps), "%d h %d m %d s", heures, minutes, secondes);
            snprintf(progression, sizeof(progression), "%d/81", partie.progression);
            printf("\t| %-20d | %-20s | %-20s | %-20s|\n", partie.id, partie.niveau, temps, progression);
            found = 1;
        }
    }
    printf("\n\t|----------------------|----------------------|----------------------|----------------------|\n");

    if (!found) {
        printf("\n\t\t--- Aucune partie trouvee pour le joueur : %s ---\n", pseudo);
    }
    fclose(fich);
}


int chercherStatistique(char *pseudo) {
    Partie partie;
    int found = 0;
    FILE *fich = fopen("statistiques", "r");
    if (!fich) {
        printf("\n\n\t\t\t Alerte : aucune statistique n'a été établie pour vous !\n");
        return 0;
    }

    printf("\n\t|----------------------|----------------------|----------------------|----------------------|\n");
    printf("\t| %-20s | %-20s | %-20s | %-20s |\n", "Joueur", "Niveau", "Temps", "Progression");
    printf("\t|----------------------|----------------------|----------------------|----------------------|\n");

    while (fscanf(fich, "%d %49s %49s %d %d", &partie.id, partie.joeur, partie.niveau, &partie.temps, &partie.progression) == 5) {
        if (strcmp(partie.joeur, pseudo) == 0) {
            int heures = partie.temps / 3600;
            int minutes = (partie.temps % 3600) / 60;
            int secondes = partie.temps % 60;
            printf("\t| %-20s | %-20s | %2d h %2d min %2d s     | %-20d/81  |\n",
                   partie.joeur, partie.niveau, heures, minutes, secondes, partie.progression);
            found = 1;
        }
    }

    fclose(fich);

    if (!found) {
        printf("\n\t\t\t --- Aucune statistique trouvée pour le joueur : %s ---\n", pseudo);
    }

    return found;
}


int chercherJoueur(char * pseudo){
    Partie part;
    FILE * fich = fopen("statistiques", "r");
    int i = 0, j = 0;
    if(fich == NULL) return 0;
    while(!feof(fich)){
       fscanf(fich, "%d %s %s %d %d\n", &part.id, part.joeur, part.niveau, &part.temps, &part.progression);
       j++;
       if(strcmp(part.joeur, pseudo) == 0){
            i += j;   
            j = 0;
       }
    }
    fclose(fich);
    return i;
}

void formatTemps(int duree, int * heures, int * minutes, int * secondes){
    *heures = duree / 3600; 
    *minutes = (duree % 3600) / 60; 
    *secondes = duree % 60;
}

int totalParties(char * pseudo){
    Partie part;
    FILE * fich = fopen("statistiques", "r");
    int i = 0;
    if(fich == NULL) return 0;
    while(!feof(fich)){
       fscanf(fich, "%d %s %s %d %d\n", &part.id, part.joeur, part.niveau, &part.temps, &part.progression);
       if(strcmp(part.joeur, pseudo) == 0){
            i += 1;
       }
    }
    fclose(fich);
    return i;
}

#include <dirent.h> 

int choisirPartie(char *filename, char *pseudo) {
    DIR *dir;
    struct dirent *entry;
    int count = 0;
    char pattern[50];
    
    snprintf(pattern, sizeof(pattern), "%s_", pseudo); 
    
    // Ouvre le répertoire courant
    if ((dir = opendir(".")) == NULL) {
        perror("Erreur : Impossible d'ouvrir le repertoire");
        return 0;
    }

    printf("\n\t|----------------------|--------------------------------|\n");
    printf("\t| %-20s | %-20s |\n", "Numero", "Nom de Fichier de Partie");
    printf("\t|----------------------|--------------------------------|\n");

    
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, pattern) == entry->d_name) {
            printf("\t| %-20d | %-20s |\n", count + 1, entry->d_name);
            count++;
        }
    }
    closedir(dir);

    if (count == 0) {
        printf("\n\tAucune partie trouve pour le pseudo '%s'.\n", pseudo);
        return 0; 
    }

    printf("\n\t|----------------------|----------------------|\n");
    printf("\n\t Entrez le numero de la partie que vous voulez charger (ou 0 pour annuler) > ");
    
    int choice;
    scanf("%d", &choice);

    if (choice == 0) {
        return 2; 
    } else if (choice > 0 && choice <= count) {
        if ((dir = opendir(".")) == NULL) {
            perror("Erreur : Impossible de rouvrir le repertoire");
            return 0;
        }

        count = 0;
        while ((entry = readdir(dir)) != NULL) {
            if (strstr(entry->d_name, pattern) == entry->d_name) {
                count++;
                if (count == choice) {
                    snprintf(filename, 260, "%s", entry->d_name); 
                    closedir(dir);
                    return 1;
                }
            }
        }
        closedir(dir);
    }

    printf("\n\tChoix invalide.\n");
    return 0;
}


int verifierIdPartie(int * tab, int taille, char * saisie){
    size_t i;
    if(strlen(saisie) == 1 && saisie[0] == '0') return 1; 
    for(i = 0; i < strlen(saisie); i++){
        if(saisie[i] > '9' || saisie[i] < '0') return 0;
    }
    int j;
    for(j = 0;  j < taille; j++){
        if(tab[j] == atoi(saisie))  return 1;
    }
    return 0;
}

