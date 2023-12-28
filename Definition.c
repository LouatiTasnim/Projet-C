#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


static void Date(char* dateStr[256]) {
    // Obtenir le temps actuel
    time_t currentTime;
    struct tm *localTime;

    time(&currentTime);           // Obtient le temps actuel en secondes depuis l'époque (01/01/1970)
    localTime = localtime(&currentTime);  // Convertit le temps en une structure tm

    // Formater la date dans la chaîne de caractères
    snprintf(dateStr, 256, "%02d/%02d/%04d",
             localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year + 1900);
    //MessageBox(NULL, dateStr, "Error", MB_ICONEXCLAMATION | MB_OK);
}


/////////les structures/////////////////////////////
////liste//////////////////////////////////////////

struct User {
    char username[256];
    char password[256];
    char email[256];
};

struct Node {
    struct User user;
    struct Node* next;
};



///////////////file/////////////////////////////////

typedef struct {
    char ref[256];
    char couleur[256];
    char description[256];
    int chevaux;
    double prix;
    char dispo[5];
} Car;

typedef struct NodeCar {
    Car car;
    struct NodeCar* next;
} NodeCar;

typedef struct {
    NodeCar* tete;  
    NodeCar* queue;
} FileCar;

 FileCar carFile;

////////////////////////pile/////////////////////////////

typedef struct {
    char refCar[256];
    char emailUser[256];
    char dateOp[256];
    char operation[256];

}Location;

typedef struct  {
    Location location;
    Location* next;
}NodeLocation;

typedef struct {
    NodeLocation* sommet; 
} PileLocation;

/////////////fichier////////////////////////////

static void sauvegarderHistorique(PileLocation* pile) {
    // Ouvrir le fichier en mode écriture, s'il n'existe pas, il sera créé
    FILE *fichierHistorique = fopen("historique.txt", "a");

    if (fichierHistorique == NULL) {
        printf("Erreur lors de l'ouverture/la création du fichier historique.txt.\n");
        return;
    }

    // Parcourir la pile et écrire chaque élément dans le fichier
    NodeLocation *current = pile->sommet;
    while (current != NULL) {
        fprintf(fichierHistorique, "Référence voiture : %s\n", current->location.refCar);
        fprintf(fichierHistorique, "E-mail utilisateur : %s\n", current->location.emailUser);
        fprintf(fichierHistorique, "Date  : %s\n", current->location.dateOp); 
        fprintf(fichierHistorique, "Opération : %s\n", current->location.operation);
        fprintf(fichierHistorique, "----------------------------------\n");

        current = current->next;
    }

    // Fermer le fichier
    fclose(fichierHistorique);

    printf("Historique sauvegardé dans le fichier historique.txt.\n");
}



////////////les sous programmes des cars//////////////////////

static void initializeFile(FileCar* file) {
    file->tete = NULL;
    file->queue = NULL;
}

static int isEmpty(FileCar* file) {
    return (file->tete == NULL);
}

static void Enfiler(FileCar* file, Car car) {
    // Créer un nouveau nœud
    NodeCar* newNode = (NodeCar*)malloc(sizeof(NodeCar));
    if (newNode == NULL) {
        printf("Erreur d'allocation de mémoire\n");
        exit(EXIT_FAILURE);
    }

    // Initialiser les données du nouveau nœud
    newNode->car = car;
    newNode->next = NULL;

    // Si la file est vide, le nouvel élément est à la fois le premier et le dernier
    if (isEmpty(file)) {
        file->tete = newNode;
        file->queue = newNode;
    } else {
        // Sinon, ajouter le nouveau nœud à la fin de la file
        file->queue->next = newNode;
        file->queue = newNode;
    }
}

static Car Defiler(FileCar* file) {
    if (isEmpty(file)) {
        printf("La file est vide\n");
        exit(EXIT_FAILURE);
    }

    // Extraire la voiture du premier nœud
    Car car = file->tete->car;

    // Mettre à jour le pointeur vers le premier nœud
    NodeCar* temp = file->tete;
    file->tete = file->tete->next;

    // Libérer la mémoire du nœud extrait
    free(temp);

    // Si la file est maintenant vide, mettre à jour le pointeur vers le dernier nœud
    if (file->tete == NULL) {
        file->queue = NULL;
    }

    return car;
}

static Car DefilerRef(FileCar* file, char reference[256]) {
    if (isEmpty(file)) {
        printf("La file est vide\n");
        exit(EXIT_FAILURE);
    }

    // Rechercher le nœud correspondant à la référence de la voiture
    NodeCar* current = file->tete;
    NodeCar* prev = NULL;

    while (current != NULL && strcmp(current->car.ref, reference) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("La voiture avec la référence %d n'a pas été trouvée dans la file\n", reference);
        exit(EXIT_FAILURE);
    }

    // Extraire la voiture du nœud trouvé
    Car car = current->car;

    // Mettre à jour les pointeurs vers le nœud précédent et suivant
    if (prev == NULL) {
        // Le nœud à extraire est en tête de file
        file->tete = current->next;
    } else {
        prev->next = current->next;
    }

    // Si le nœud à extraire est le dernier nœud, mettre à jour le pointeur vers le dernier nœud
    if (current == file->queue) {
        file->queue = prev;
    }

    // Libérer la mémoire du nœud extrait
    free(current);

    return car;
}


static void Louer(FileCar* file, const char ref[255],char test[5]) {
    if (isEmpty(file)) {
        printf("La file est vide\n");
        exit(EXIT_FAILURE);
    }

    NodeCar* current = file->tete;
    NodeCar* previous = NULL;

    // Parcourir la file pour trouver le nœud avec la référence donnée
    while (current != NULL) {
       if (strcmp(current->car.ref, ref) == 0) {
           strcpy(current->car.dispo, test);
       }

        // Avancer dans la file
        previous = current;
        current = current->next;
    }

    // Si la référence n'a pas été trouvée, vous pouvez gérer l'erreur ici
    printf("La voiture avec la référence %s n'a pas été trouvée dans la file\n", ref);
    //exit(EXIT_FAILURE);
}


// Fonction pour récupérer la référence de la voiture à partir de l'index
static const char* GetCarRefAtIndex(FileCar* file, int index) {
    if (isEmpty(file) || index < 0) {
        return NULL; // La file est vide ou l'index est invalide
    }

    NodeCar* current = file->tete;
    int currentIndex = 0;

    while (current != NULL && currentIndex < index) {
        current = current->next;
        currentIndex++;
    }

    if (current != NULL && currentIndex == index) {
        return current->car.ref;
    } else {
        return NULL; // L'index est hors limites
    }
}

static Car GetCarInfoByRef(FileCar* file, const char* reference) {
    NodeCar* current = file->tete;
    while (current != NULL && strcmp(current->car.ref, reference) != 0) {
        current = current->next;
    }

        return current->car;
}



//////////////////////////les sous programmes des locations///////////////////////////////

static void initialiserPile(PileLocation* pile) {
    pile->sommet = NULL;
}

static int estVide(PileLocation* pile) {
    return pile->sommet == NULL;
}

static void empiler(PileLocation* pile, Location* valeur) {
    NodeLocation* nouveauNoeud = (NodeLocation*)malloc(sizeof(NodeLocation));
    if (nouveauNoeud == NULL) {
    	printf("Erreur d'allocation mémoire.\n");
    }

    // Copie de la valeur dans le nouveau nœud
    strcpy(nouveauNoeud->location.refCar, valeur->refCar);
    strcpy(nouveauNoeud->location.emailUser, valeur->emailUser);
    strcpy(nouveauNoeud->location.operation, valeur->operation);
    strcpy(nouveauNoeud->location.dateOp, valeur->dateOp);

    // Mise à jour du lien
    nouveauNoeud->next = pile->sommet;

    // Mise à jour du sommet de la pile
    pile->sommet = nouveauNoeud;
}

static Location depiler(PileLocation* pile) {
    if (estVide(pile)) {
        printf("La pile est vide. Impossible de dépiler.\n");
    }

    // Récupération de la valeur au sommet de la pile
    Location valeurDepilee = pile->sommet->location;

    // Mise à jour du sommet de la pile
    NodeLocation* ancienSommet = pile->sommet;
    pile->sommet = pile->sommet->next;

    // Libération de la mémoire de l'ancien sommet
    free(ancienSommet);

    // Retour de la valeur dépilée
    return valeurDepilee;
}


