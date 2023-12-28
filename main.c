#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Definition.c"


char g_connectedUserEmail[256];

HWND g_usernameTextBox;
HWND g_passwordTextBox;
HWND g_loginButton;
HWND g_inscriButton;
HWND g_usernameLabel;
HWND g_passwordLabel;

HWND g_inscriptionWindow;
HWND g_inscriptionUsernameTextBox;
HWND g_inscriptionPasswordTextBox;
HWND g_confirmPasswordTextBox;
HWND g_emailTextBox;
HWND g_createAccountButton;

HWND g_louerButton;
HWND g_afficherButton;
HWND g_supprimerButton;
HWND g_modifierButton;
HWND g_historiqueButton;
HWND g_retourButton;

HWND g_voituresDispoWindow;

HWND g_historiqueWindow;
HWND g_ArchiveButton;
HWND g_FiltreJourButton;
HWND g_FiltreMoisButton;
HWND g_FiltreJourInput;
HWND g_FiltreMoisInput;
HWND g_historiqueFiltreJourWindow;
g_historiqueFiltreMoisWindow;

HWND g_voituresLouerWindow;
HWND g_voituresWindow;
HWND g_voituresSupprimerWindow;
HWND g_voituresModifierWindow;

HWND g_modifWindow;
HWND g_ModifButton;
HWND g_refTextBox;
HWND g_couleurTextBox;
HWND g_descTextBox;
HWND g_prixTextBox;
HWND g_chevauxTextBox;


struct Node* userList = NULL;

FileCar carFile;

PileLocation LocationPile;

////////////////////les sous programmes des users ///////////////////////////////////////

void AddUser(const char* username, const char* password,const char* email) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->user.username, username);
    strcpy(newNode->user.password, password);
    strcpy(newNode->user.email, email);
    newNode->next = userList;
    userList = newNode;
}

int AuthenticateUser(const char* username, const char* password) {
    struct Node* current = userList;
    while (current != NULL) {
        if (strcmp(current->user.username, username) == 0 && strcmp(current->user.password, password) == 0) {
            return 1;  // User found
        }
        current = current->next;
    }
    return 0;  // User not found
}

const char* GetUserEmail(const char* username) {
    struct Node* current = userList;
    while (current != NULL) {
        if (strcmp(current->user.username, username) == 0) {
            return current->user.email;
        }
        current = current->next;
    }
    return NULL; // Username not found
}



////////////////////modifier window////////////////////////////////////

LRESULT CALLBACK ModifierWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    	
    	case WM_COMMAND:
    		if (HIWORD(wParam) == EN_SETFOCUS) {
    		// R�cup�rez l'indice de la voiture � partir de la propri�t� de fen�tre
                int carIndex = (int)GetWindowLongPtr((HWND)lParam, GWLP_USERDATA);
                 Car selectedCar = GetCarInfoByRef(&carFile, GetCarRefAtIndex(&carFile,carIndex));
                CreateModifWindow(selectedCar);
               DestroyWindow(hwnd);
                
			}
			break;

    	
        case WM_DESTROY:
            // Fermez la fen�tre des voitures disponibles
            //PostQuitMessage(0);
            break;

        // Ajoutez d'autres messages ou �v�nements n�cessaires selon vos besoins

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}


LRESULT CALLBACK modifWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_COMMAND:
        // G�rer les �v�nements de la fen�tre de modification ici
        if (lParam == (LPARAM)g_ModifButton) {
            // R�cup�rez l'indice de la voiture � partir de la propri�t� de fen�tre
            int carIndex = (int)GetWindowLongPtr(hwnd, GWLP_USERDATA);
             Car selectedCar = GetCarInfoByRef(&carFile, GetCarRefAtIndex(&carFile,carIndex));
            // Supprimer l'ancienne voiture de la file
            DefilerRef(&carFile, GetCarRefAtIndex(&carFile, carIndex));
            // R�cup�rez les nouvelles informations de la voiture depuis les zones de texte
            char newRef[256], newCouleur[256], newDesc[256], newPrixStr[100], newChevauxStr[100];
            GetWindowTextA(g_refTextBox, newRef, sizeof(newRef));
            GetWindowTextA(g_couleurTextBox, newCouleur, sizeof(newCouleur));
            GetWindowTextA(g_descTextBox, newDesc, sizeof(newDesc));
            GetWindowTextA(g_prixTextBox, newPrixStr, sizeof(newPrixStr));
            GetWindowTextA(g_chevauxTextBox, newChevauxStr, sizeof(newChevauxStr));

            // Convertir les cha�nes en valeurs appropri�es
            double newPrix = atof(newPrixStr);
            int newChevaux = atoi(newChevauxStr);

            // Cr�er une nouvelle voiture avec les informations mises � jour
            Car updatedCar;
            strcpy(updatedCar.ref, newRef);
            strcpy(updatedCar.couleur, newCouleur);
            strcpy(updatedCar.description, newDesc);
            updatedCar.prix = newPrix;
            updatedCar.chevaux = newChevaux;
            strcpy(updatedCar.dispo, selectedCar.dispo); // Mettez � jour selon vos besoins

            // Enfiler la nouvelle voiture
            Enfiler(&carFile, updatedCar);

            // Ajouter une nouvelle op�ration � la pile
            Location newLocation;
            strcpy(newLocation.operation, "Modification");
            strcpy(newLocation.emailUser, g_connectedUserEmail);
            strcpy(newLocation.refCar, newRef);
            Date(newLocation.dateOp);
            empiler(&LocationPile, &newLocation);

            MessageBox(hwnd, "Modification avec succ�s!", "Succ�s", MB_OK | MB_ICONINFORMATION);
            DestroyWindow(g_modifWindow);
            g_modifWindow = NULL;
        }
        break;

    case WM_DESTROY:
        // Terminer la boucle de message de la fen�tre d'inscription
       // PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}



void CreateModifWindow(Car selectedCar){
	static int windowClassCounter1 = 0;
	char classNom[255];
	sprintf(classNom, "ModifClass%d", windowClassCounter1++);
    WNDCLASSEX wcModif;
    memset(&wcModif, 0, sizeof(wcModif));
    wcModif.cbSize = sizeof(WNDCLASSEX);
    wcModif.lpfnWndProc = modifWndProc;
    wcModif.hInstance = GetModuleHandle(NULL);
    wcModif.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcModif.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcModif.lpszClassName = classNom;
    wcModif.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcModif.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcModif)) {
        MessageBox(NULL, "�chec de l'enregistrement de la classe de la fen�tre des modifications disponibles!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
	 g_modifWindow = CreateWindowEx(WS_EX_CLIENTEDGE, classNom, "Modification",
                WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
                NULL, NULL, GetModuleHandle(NULL), NULL);

    if (g_modifWindow == NULL) {
                MessageBox(NULL, "�chec de la cr�ation de la fen�tre de modification!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
            }
    CreateWindow("STATIC", "R�f�rence:", WS_VISIBLE | WS_CHILD,
    		10, 10, 120, 20, g_modifWindow, NULL, NULL, NULL);

	g_refTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
    		140, 10, 200, 20, g_modifWindow, NULL, NULL, NULL);

	CreateWindow("STATIC", "Couleur:", WS_VISIBLE | WS_CHILD,
            10, 50, 120, 20, g_modifWindow, NULL, NULL, NULL);

    g_couleurTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
            140, 50, 200, 20, g_modifWindow, NULL, NULL, NULL);

    CreateWindow("STATIC", "Description:", WS_VISIBLE | WS_CHILD,
            10, 90, 120, 20, g_modifWindow, NULL, NULL, NULL);

    g_descTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER ,
            140, 90, 200, 20, g_modifWindow, NULL, NULL, NULL);

    CreateWindow("STATIC", "Chevaux:", WS_VISIBLE | WS_CHILD,
            10, 130, 120, 20, g_modifWindow, NULL, NULL, NULL);

    g_chevauxTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER ,
            140, 130, 200, 20, g_modifWindow, NULL, NULL, NULL);
    
    CreateWindow("STATIC", "Prix:", WS_VISIBLE | WS_CHILD,
            10, 170, 120, 20, g_modifWindow, NULL, NULL, NULL);

    g_prixTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER ,
            140, 170, 200, 20, g_modifWindow, NULL, NULL, NULL);
    

    g_ModifButton = CreateWindow("BUTTON", "Modifier", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            140, 190, 100, 30, g_modifWindow, NULL, NULL, NULL);
            
    // Convert prix to string using snprintf
    char p[100];
    snprintf(p, sizeof(p), "%f", selectedCar.prix);
    SetWindowTextA(g_refTextBox, selectedCar.ref);
    SetWindowTextA(g_couleurTextBox, selectedCar.couleur);
    SetWindowTextA(g_prixTextBox, p);
    SetWindowTextA(g_descTextBox, selectedCar.description);

    // Convert chevaux to string
    char chevauxStr[100];
    snprintf(chevauxStr, sizeof(chevauxStr), "%d", selectedCar.chevaux);
    SetWindowTextA(g_chevauxTextBox, chevauxStr);
}

////////////////Fin modifier window///////////////////////////////////

////////////////////supprimer window////////////////////////////////////

LRESULT CALLBACK SupprimerWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    	
    	case WM_COMMAND:
            if (HIWORD(wParam) == EN_SETFOCUS) {
                // R�cup�rez l'indice de la voiture � partir de la propri�t� de fen�tre
                int carIndex = (int)GetWindowLongPtr((HWND)lParam, GWLP_USERDATA);
                 // R�cup�rez l'e-mail de l'utilisateur connect� et la r�f�rence de la voiture
                char emailBuffer[256], refCarBuffer[256];
                strcpy(emailBuffer, g_connectedUserEmail);

                // Ajoutez un nouveau n�ud � la pile
                Location newLocation;
                strcpy(newLocation.operation, "Suppression");
                strcpy(newLocation.emailUser, emailBuffer);
                strcpy(newLocation.refCar, GetCarRefAtIndex(&carFile,carIndex));
                Date(newLocation.dateOp);
                empiler(&LocationPile, &newLocation);
                
                MessageBox(hwnd, "Suppression avec success!", "Success", MB_OK | MB_ICONINFORMATION);
                DestroyWindow(hwnd);
                g_voituresSupprimerWindow = NULL;
                DefilerRef(&carFile,GetCarRefAtIndex(&carFile,carIndex));

            }
            break;
    	
        case WM_DESTROY:
            // Fermez la fen�tre des voitures disponibles
            //PostQuitMessage(0);
            break;

        // Ajoutez d'autres messages ou �v�nements n�cessaires selon vos besoins

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

////////////////Fin supprimer window///////////////////////////////////

////////////////////retour window////////////////////////////////////

LRESULT CALLBACK RetourWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    	
    	case WM_COMMAND:
            if (HIWORD(wParam) == EN_SETFOCUS) {
                // R�cup�rez l'indice de la voiture � partir de la propri�t� de fen�tre
                int carIndex = (int)GetWindowLongPtr((HWND)lParam, GWLP_USERDATA);
                 // R�cup�rez l'e-mail de l'utilisateur connect� et la r�f�rence de la voiture
                char emailBuffer[256], refCarBuffer[256];
                strcpy(emailBuffer, g_connectedUserEmail);

                // Ajoutez un nouveau n�ud � la pile
                Location newLocation;
                strcpy(newLocation.operation, "Retour");
                strcpy(newLocation.emailUser, emailBuffer);
                strcpy(newLocation.refCar, GetCarRefAtIndex(&carFile,carIndex));
                Date(newLocation.dateOp);
                empiler(&LocationPile, &newLocation);
                
                MessageBox(hwnd, "Retour avec success!", "Success", MB_OK | MB_ICONINFORMATION);
                DestroyWindow(hwnd);
                g_voituresLouerWindow = NULL;
                Louer(&carFile,GetCarRefAtIndex(&carFile,carIndex),"Oui");

            }
            break;
    	
        case WM_DESTROY:
            // Fermez la fen�tre des voitures disponibles
            //PostQuitMessage(0);
            break;

        // Ajoutez d'autres messages ou �v�nements n�cessaires selon vos besoins

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

////////////////Fin supprimer window///////////////////////////////////

////////////////Afficher window///////////////////////////////////

LRESULT CALLBACK CarWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    	
    	case WM_COMMAND:
            if (HIWORD(wParam) == EN_SETFOCUS) {
                // R�cup�rez l'indice de la voiture � partir de la propri�t� de fen�tre
                int carIndex = (int)GetWindowLongPtr((HWND)lParam, GWLP_USERDATA);

            }
            break;
    	
        case WM_DESTROY:
            // Fermez la fen�tre des voitures disponibles
            //PostQuitMessage(0);
            break;

        // Ajoutez d'autres messages ou �v�nements n�cessaires selon vos besoins

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

///////////////fin afficher window/////////////////////////////////


////////////////location window///////////////////////////////////
LRESULT CALLBACK VoituresDispoWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    	
    	case WM_COMMAND:
            if (HIWORD(wParam) == EN_SETFOCUS) {
                // R�cup�rez l'indice de la voiture � partir de la propri�t� de fen�tre
                int carIndex = (int)GetWindowLongPtr((HWND)lParam, GWLP_USERDATA);

                // R�cup�rez l'e-mail de l'utilisateur connect� et la r�f�rence de la voiture
                char emailBuffer[256], refCarBuffer[256];
                strcpy(emailBuffer, g_connectedUserEmail);

                // Ajoutez un nouveau n�ud � la pile
                Location newLocation;
                strcpy(newLocation.dateOp, "test");
                strcpy(newLocation.operation,"Location");
                strcpy(newLocation.emailUser, emailBuffer);
                strcpy(newLocation.refCar, GetCarRefAtIndex(&carFile,carIndex));
                Date(&newLocation.dateOp);
                empiler(&LocationPile, &newLocation);
                 MessageBox(hwnd, "Location avec success!", "Success", MB_OK | MB_ICONINFORMATION);
                 DestroyWindow(hwnd);
                g_voituresDispoWindow = NULL;
                Louer(&carFile,GetCarRefAtIndex(&carFile,carIndex),"Non");
                //ShowWindow(g_voituresDispoWindow, SW_SHOWNORMAL);
               //AfficherVoituresDispo();
            }
            break;
    	
        case WM_DESTROY:
            // Fermez la fen�tre des voitures disponibles
            //PostQuitMessage(0);
            break;

        // Ajoutez d'autres messages ou �v�nements n�cessaires selon vos besoins

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}
void AfficherVoituresDispo(HWND g_Window, char test[5],LRESULT VoituresDispoWndProc) {    
	static int windowClassCounter = 0;
	char classNom[255];
	sprintf(classNom, "VoituresDispoWindowClass%d", windowClassCounter++);
    WNDCLASSEX wcVoituresDispo;
    memset(&wcVoituresDispo, 0, sizeof(wcVoituresDispo));
    wcVoituresDispo.cbSize = sizeof(WNDCLASSEX);
    wcVoituresDispo.lpfnWndProc = VoituresDispoWndProc;
    wcVoituresDispo.hInstance = GetModuleHandle(NULL);
    wcVoituresDispo.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcVoituresDispo.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcVoituresDispo.lpszClassName = classNom;
    wcVoituresDispo.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcVoituresDispo.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcVoituresDispo)) {
        MessageBox(NULL, "�chec de l'enregistrement de la classe de la fen�tre des voitures !", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
	
	// Cr�ez une nouvelle fen�tre pour afficher les voitures disponibles
    g_Window = CreateWindowEx(WS_EX_CLIENTEDGE, classNom, "Voitures ",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, NULL, GetModuleHandle(NULL), NULL);
        
    if (g_Window == NULL) {
        MessageBox(NULL, "�chec de la cr�ation de la fen�tre des voitures !", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
  if (isEmpty(&carFile)) {
        MessageBox(NULL, "La file est vide", "Information", MB_OK | MB_ICONINFORMATION);
        return;
    }

    // Parcourir la file et afficher chaque voiture dans une bo�te de texte s�par�e
    NodeCar* current = carFile.tete;
    int yOffset = 10;  // Ajustez la valeur de d�part en fonction de vos besoins
	
	 int currentIndex = 0;
	 
    while (current != NULL ) {
    if(test){
		if(strcmp(current->car.dispo, test) == 0){
        // Cr�ez une bo�te de texte pour chaque voiture
        HWND hEditCarInfo = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY,
            10, yOffset, 480, 80, g_Window, (HMENU)(intptr_t)currentIndex, GetModuleHandle(NULL), NULL);

        if (hEditCarInfo == NULL) {
            MessageBox(NULL, "Failed to create edit control!", "Error", MB_ICONEXCLAMATION | MB_OK);
            return;
        }

        // Affichez les informations de la voiture dans la bo�te de texte
        char carText[256];
        sprintf(carText, "R�f�rence: %s, Couleur: %s, Description: %s, Chevaux: %d, Prix: %.2f, Disponible: %s \n",
               current->car.ref, current->car.couleur, current->car.description,
               current->car.chevaux, current->car.prix,current->car.dispo);
		
		// Associez l'indice de la voiture � la propri�t� de fen�tre
        SetWindowLongPtr(hEditCarInfo, GWLP_USERDATA, (LONG_PTR)currentIndex);
        SetWindowText(hEditCarInfo, carText);
         yOffset += 90;  // Ajustez la valeur pour l'espacement entre les bo�tes de texte
	}
	}
	else {
        HWND hEditCarInfo = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY,
            10, yOffset, 480, 80, g_Window, (HMENU)(intptr_t)currentIndex, GetModuleHandle(NULL), NULL);

        if (hEditCarInfo == NULL) {
            MessageBox(NULL, "Failed to create edit control!", "Error", MB_ICONEXCLAMATION | MB_OK);
            return;
        }
        char carText[256];
        sprintf(carText, "R�f�rence: %s, Couleur: %s, Description: %s, Chevaux: %d, Prix: %.2f, Disponible: %s \n",
               current->car.ref, current->car.couleur, current->car.description,
               current->car.chevaux, current->car.prix,current->car.dispo);

        SetWindowLongPtr(hEditCarInfo, GWLP_USERDATA, (LONG_PTR)currentIndex);
        SetWindowText(hEditCarInfo, carText);
         yOffset += 90;  
	}
        current = current->next;
        currentIndex++;
    }
    // Affichez la fen�tre des voitures disponibles
    ShowWindow(g_Window, SW_SHOWNORMAL);
    UpdateWindow(g_Window);
}

//////////////fin location window///////////////////

////////////////Historique window//////////////////////////////////

LRESULT CALLBACK HistoriqueWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    	
    	case WM_COMMAND:
           if (lParam == (LPARAM)g_ArchiveButton) {
           	sauvegarderHistorique(&LocationPile);
		   }
		   else if (lParam == (LPARAM)g_FiltreJourButton) {
                // Filtrer par jour
                FiltrerHistoriqueParJour();
            }
           else if (lParam == (LPARAM)g_FiltreMoisButton) {
            FiltrerHistoriqueParMois();
        }
            break;
    	
        case WM_DESTROY:
            // Fermez la fen�tre des voitures disponibles
            //PostQuitMessage(0);
            break;

        // Ajoutez d'autres messages ou �v�nements n�cessaires selon vos besoins

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK HistoriqueJWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    	
    	case WM_COMMAND:
           
            break;
    	
        case WM_DESTROY:
            // Fermez la fen�tre des voitures disponibles
            //PostQuitMessage(0);
            break;

        // Ajoutez d'autres messages ou �v�nements n�cessaires selon vos besoins

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK HistoriqueMWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    	
    	case WM_COMMAND:
           
            break;
    	
        case WM_DESTROY:
            // Fermez la fen�tre des voitures disponibles
            //PostQuitMessage(0);
            break;

        // Ajoutez d'autres messages ou �v�nements n�cessaires selon vos besoins

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

void AfficherHistoriqueLocation(){
	static int windowClassCounter1 = 0;
	char classNom[255];
	sprintf(classNom, "HistoriqueClass%d", windowClassCounter1++);
    WNDCLASSEX wcHistorique;
    memset(&wcHistorique, 0, sizeof(wcHistorique));
    wcHistorique.cbSize = sizeof(WNDCLASSEX);
    wcHistorique.lpfnWndProc = HistoriqueWndProc;
    wcHistorique.hInstance = GetModuleHandle(NULL);
    wcHistorique.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcHistorique.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcHistorique.lpszClassName = classNom;
    wcHistorique.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcHistorique.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcHistorique)) {
        MessageBox(NULL, "�chec de l'enregistrement de la classe de la fen�tre des historiques disponibles!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
	
	// Cr�ez une nouvelle fen�tre pour afficher l'historique
    g_historiqueWindow = CreateWindowEx(WS_EX_CLIENTEDGE, classNom, "Historique",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, NULL, GetModuleHandle(NULL), NULL);
        
    if (g_historiqueWindow == NULL) {
        MessageBox(NULL, "�chec de la cr�ation de la fen�tre historique!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
  if (isEmpty(&LocationPile)) {
        MessageBox(NULL, "La pile est vide", "Information", MB_OK | MB_ICONINFORMATION);
        return;
    }
    g_FiltreJourInput = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
        10,5,200,30, g_historiqueWindow, NULL, NULL, NULL);
        
     g_FiltreJourButton = CreateWindow("BUTTON", "Filtrer l'historique par jour", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        210,5,300,30,g_historiqueWindow, NULL, NULL, NULL);
    
     g_FiltreMoisInput = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
        10,30,200,30, g_historiqueWindow, NULL, NULL, NULL);
            
    g_FiltreMoisButton = CreateWindow("BUTTON", "Filtrer l'historique par mois", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        210,30,300,30,g_historiqueWindow, NULL, NULL, NULL);
            
     g_ArchiveButton = CreateWindow("BUTTON", "Vider l'historique dans un fichier", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,70,500, 30, g_historiqueWindow, NULL, NULL, NULL);

    // Parcourir la pile et afficher chaque voiture dans une bo�te de texte s�par�e
    NodeLocation* current = LocationPile.sommet;
    int yOffset = 150;  // Ajustez la valeur de d�part en fonction de vos besoins
	
	 int currentIndex = 0;
	 
    while (current != NULL) {
        // Cr�ez une bo�te de texte pour chaque voiture
        HWND hEditHistoriqueInfo = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY,
            10, yOffset, 480, 80, g_historiqueWindow, (HMENU)(intptr_t)currentIndex, GetModuleHandle(NULL), NULL);

        if (hEditHistoriqueInfo == NULL) {
            MessageBox(NULL, "Failed to create edit control!", "Error", MB_ICONEXCLAMATION | MB_OK);
            return;
        }

        // Affichez les informations de la location dans la bo�te de texte
        char carText[256];
        sprintf(carText, "R�f�rence: %s, Email: %s, Operation: %s, Date: %s \n",
               current->location.refCar, current->location.emailUser,current->location.operation,current->location.dateOp);
		
		// Associez l'indice de la voiture � la propri�t� de fen�tre
        SetWindowLongPtr(hEditHistoriqueInfo, GWLP_USERDATA, (LONG_PTR)currentIndex);
        SetWindowText(hEditHistoriqueInfo, carText);

        yOffset += 90;  // Ajustez la valeur pour l'espacement entre les bo�tes de texte
        current = current->next;
        currentIndex++;
    }
    // Affichez la fen�tre des historiques
    ShowWindow(g_historiqueWindow, SW_SHOWNORMAL);
    UpdateWindow(g_historiqueWindow);
	
}

void FiltrerHistoriqueParJour() {
	static int windowClassCounter1 = 100;
	char classNom[255];
	sprintf(classNom, "HistoriqueClass%d", windowClassCounter1++);
    WNDCLASSEX wcHistoriqueJ;
    memset(&wcHistoriqueJ, 0, sizeof(wcHistoriqueJ));
    wcHistoriqueJ.cbSize = sizeof(WNDCLASSEX);
    wcHistoriqueJ.lpfnWndProc = HistoriqueJWndProc;
    wcHistoriqueJ.hInstance = GetModuleHandle(NULL);
    wcHistoriqueJ.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcHistoriqueJ.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcHistoriqueJ.lpszClassName = classNom;
    wcHistoriqueJ.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcHistoriqueJ.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcHistoriqueJ)) {
        MessageBox(NULL, "�chec de l'enregistrement de la classe de la fen�tre des historiques disponibles!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
	
	// Cr�ez une nouvelle fen�tre pour afficher l'historique
    g_historiqueFiltreJourWindow = CreateWindowEx(WS_EX_CLIENTEDGE, classNom, "Historique",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, NULL, GetModuleHandle(NULL), NULL);
        
    if (g_historiqueFiltreJourWindow == NULL) {
        MessageBox(NULL, "�chec de la cr�ation de la fen�tre historique!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
    // Obtenez la date saisie dans l'�l�ment d'�dition
    char dateFilter[20];
    GetWindowText(g_FiltreJourInput, dateFilter, sizeof(dateFilter));

    // Parcourez la pile et affichez uniquement les �l�ments correspondant � la date
    NodeLocation* current = LocationPile.sommet;
    int yOffset = 10;  // Ajustez la valeur de d�part en fonction de vos besoins
    int currentIndex = 0;

    while (current != NULL) {
        // V�rifiez si la date correspond au filtre
        if (strstr(current->location.dateOp, dateFilter) != NULL) {
            // Cr�ez une bo�te de texte pour chaque voiture
            HWND hEditHistoriqueInfo = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY,
                10, yOffset, 480, 80, g_historiqueFiltreJourWindow, (HMENU)(intptr_t)currentIndex, GetModuleHandle(NULL), NULL);

            if (hEditHistoriqueInfo == NULL) {
                MessageBox(NULL, "Failed to create edit control!", "Error", MB_ICONEXCLAMATION | MB_OK);
                return;
            }

            // Affichez les informations de la location dans la bo�te de texte
            char carText[256];
            sprintf(carText, "R�f�rence: %s, Email: %s, Operation: %s, Date: %s \n",
                   current->location.refCar, current->location.emailUser, current->location.operation, current->location.dateOp);

            // Associez l'indice de la voiture � la propri�t� de fen�tre
            SetWindowLongPtr(hEditHistoriqueInfo, GWLP_USERDATA, (LONG_PTR)currentIndex);
            SetWindowText(hEditHistoriqueInfo, carText);

            yOffset += 90;  // Ajustez la valeur pour l'espacement entre les bo�tes de texte
        }

        current = current->next;
        currentIndex++;
    }
    ShowWindow(g_historiqueFiltreJourWindow, SW_SHOWNORMAL);
    UpdateWindow(g_historiqueFiltreJourWindow);
}

void FiltrerHistoriqueParMois() {
	static int windowClassCounter1 = 200;
	char classNom[255];
	sprintf(classNom, "HistoriqueClass%d", windowClassCounter1++);
    WNDCLASSEX wcHistoriqueM;
    memset(&wcHistoriqueM, 0, sizeof(wcHistoriqueM));
    wcHistoriqueM.cbSize = sizeof(WNDCLASSEX);
    wcHistoriqueM.lpfnWndProc = HistoriqueMWndProc;
    wcHistoriqueM.hInstance = GetModuleHandle(NULL);
    wcHistoriqueM.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcHistoriqueM.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcHistoriqueM.lpszClassName = classNom;
    wcHistoriqueM.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcHistoriqueM.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcHistoriqueM)) {
        MessageBox(NULL, "�chec de l'enregistrement de la classe de la fen�tre des historiques disponibles!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
	
	// Cr�ez une nouvelle fen�tre pour afficher l'historique
    g_historiqueFiltreMoisWindow = CreateWindowEx(WS_EX_CLIENTEDGE, classNom, "Historique",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, NULL, GetModuleHandle(NULL), NULL);
        
    if (g_historiqueFiltreMoisWindow == NULL) {
        MessageBox(NULL, "�chec de la cr�ation de la fen�tre historique!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
    // Obtenez la date saisie dans l'�l�ment d'�dition
    char dateFilter[20];
    GetWindowText(g_FiltreMoisInput, dateFilter, sizeof(dateFilter));

    // Parcourez la pile et affichez uniquement les �l�ments correspondant � la date
    NodeLocation* current = LocationPile.sommet;
    int yOffset = 10;  // Ajustez la valeur de d�part en fonction de vos besoins
    int currentIndex = 0;

    while (current != NULL) {
        // V�rifiez si la date correspond au filtre
        if (strstr(strchr(current->location.dateOp, '/'), dateFilter) != NULL) {
            // Cr�ez une bo�te de texte pour chaque voiture
            HWND hEditHistoriqueInfo = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY,
                10, yOffset, 480, 80, g_historiqueFiltreMoisWindow, (HMENU)(intptr_t)currentIndex, GetModuleHandle(NULL), NULL);

            if (hEditHistoriqueInfo == NULL) {
                MessageBox(NULL, "Failed to create edit control!", "Error", MB_ICONEXCLAMATION | MB_OK);
                return;
            }

            // Affichez les informations de la location dans la bo�te de texte
            char carText[256];
            sprintf(carText, "R�f�rence: %s, Email: %s, Operation: %s, Date: %s \n",
                   current->location.refCar, current->location.emailUser, current->location.operation, current->location.dateOp);

            // Associez l'indice de la voiture � la propri�t� de fen�tre
            SetWindowLongPtr(hEditHistoriqueInfo, GWLP_USERDATA, (LONG_PTR)currentIndex);
            SetWindowText(hEditHistoriqueInfo, carText);

            yOffset += 90;  // Ajustez la valeur pour l'espacement entre les bo�tes de texte
        }

        current = current->next;
        currentIndex++;
    }
    ShowWindow(g_historiqueFiltreMoisWindow, SW_SHOWNORMAL);
    UpdateWindow(g_historiqueFiltreMoisWindow);
}
///////////////////fin historique window///////////////////////////

////////// Inscription window//////////////////////
LRESULT CALLBACK InscriptionWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_COMMAND:
        // G�rer les �v�nements de la fen�tre d'inscription ici
        if (lParam == (LPARAM)g_createAccountButton) {
            // R�cup�rer les informations de l'utilisateur pour cr�er le compte
            char usernameBuffer[256], passwordBuffer[256], confirmPasswordBuffer[256], emailBuffer[256];
            GetWindowTextA(g_inscriptionUsernameTextBox, usernameBuffer, sizeof(usernameBuffer));
            GetWindowTextA(g_inscriptionPasswordTextBox, passwordBuffer, sizeof(passwordBuffer));
            GetWindowTextA(g_confirmPasswordTextBox, confirmPasswordBuffer, sizeof(confirmPasswordBuffer));
            GetWindowTextA(g_emailTextBox, emailBuffer, sizeof(emailBuffer));

            // V�rifier si les mots de passe correspondent
            if (strcmp(passwordBuffer, confirmPasswordBuffer) == 0) {
                // Ajouter l'utilisateur � la liste
                AddUser(usernameBuffer, passwordBuffer, emailBuffer);
                MessageBox(hwnd, "Compte cr�� avec succ�s!", "Succ�s", MB_OK | MB_ICONINFORMATION);
                // Fermer la fen�tre d'inscription
                ShowWindow(hwnd, SW_HIDE);
            } else {
                MessageBox(hwnd, "Les mots de passe ne correspondent pas!", "Erreur", MB_OK | MB_ICONERROR);
            }
        }
        break;

    case WM_DESTROY:
        // Terminer la boucle de message de la fen�tre d'inscription
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}


void CreateInscriWindow(){
	 g_inscriptionWindow = CreateWindowEx(WS_EX_CLIENTEDGE, "InscriptionWindowClass", "Inscription",
                WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
                NULL, NULL, GetModuleHandle(NULL), NULL);

    if (g_inscriptionWindow == NULL) {
                MessageBox(NULL, "�chec de la cr�ation de la fen�tre d'inscription!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
            }
    CreateWindow("STATIC", "Email:", WS_VISIBLE | WS_CHILD,
    		10, 50, 120, 20, g_inscriptionWindow, NULL, NULL, NULL);

	g_emailTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
    		140, 50, 200, 20, g_inscriptionWindow, NULL, NULL, NULL);

	 CreateWindow("STATIC", "Nom d'utilisateur:", WS_VISIBLE | WS_CHILD,
            10, 10, 120, 20, g_inscriptionWindow, NULL, NULL, NULL);

        g_inscriptionUsernameTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
            140, 10, 200, 20, g_inscriptionWindow, NULL, NULL, NULL);

        CreateWindow("STATIC", "Mot de passe:", WS_VISIBLE | WS_CHILD,
            10, 90, 120, 20, g_inscriptionWindow, NULL, NULL, NULL);

        g_inscriptionPasswordTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
            140, 90, 200, 20, g_inscriptionWindow, NULL, NULL, NULL);

        CreateWindow("STATIC", "Confirmer mot de passe:", WS_VISIBLE | WS_CHILD,
            10, 130, 120, 20, g_inscriptionWindow, NULL, NULL, NULL);

        g_confirmPasswordTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
            140, 130, 200, 20, g_inscriptionWindow, NULL, NULL, NULL);

        g_createAccountButton = CreateWindow("BUTTON", "Confirmer", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            140, 170, 100, 30, g_inscriptionWindow, NULL, NULL, NULL);
}
//////////fin inscription////////////

////Menu window///////////////////////

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
        // G�rer les messages de la nouvelle fen�tre ici
        case WM_COMMAND:
            if (lParam == (LPARAM)g_louerButton) {
                AfficherVoituresDispo(g_voituresDispoWindow,"Oui",VoituresDispoWndProc);
                 MessageBox(NULL, "Veiullez selectionner la voiture � louer ", "Information", MB_OK | MB_ICONINFORMATION);
				}
			if (lParam == (LPARAM)g_historiqueButton) {
                AfficherHistoriqueLocation();
				}
			if (lParam == (LPARAM)g_retourButton) {
                AfficherVoituresDispo(g_voituresLouerWindow,"Non",RetourWndProc);
                MessageBox(NULL, "Veiullez selectionner la voiture � retourner ", "Information", MB_OK | MB_ICONINFORMATION);
				}
			if (lParam == (LPARAM)g_afficherButton) {
                AfficherVoituresDispo(g_voituresWindow,NULL,CarWndProc);
				}
			if (lParam == (LPARAM)g_supprimerButton) {
                AfficherVoituresDispo(g_voituresSupprimerWindow,NULL,SupprimerWndProc);
                MessageBox(NULL, "Veiullez selectionner la voiture � supprimer ", "Information", MB_OK | MB_ICONINFORMATION);
				}
			if (lParam == (LPARAM)g_modifierButton) {
                AfficherVoituresDispo(g_voituresModifierWindow,NULL,ModifierWndProc);
                MessageBox(NULL, "Veiullez selectionner la voiture � modifier ", "Information", MB_OK | MB_ICONINFORMATION);
				}
            break;

        case WM_DESTROY:
            // Terminer la boucle de message de la nouvelle fen�tre
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}


void CreateMainWindow() {

    WNDCLASSEX wcMain;
    memset(&wcMain, 0, sizeof(wcMain));
    wcMain.cbSize = sizeof(WNDCLASSEX);
    wcMain.lpfnWndProc = MainWndProc;
    wcMain.hInstance = GetModuleHandle(NULL);
    wcMain.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcMain.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcMain.lpszClassName = "MainWindowClass";
    wcMain.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcMain.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcMain)) {
        MessageBox(NULL, "�chec de l'enregistrement de la classe de la fen�tre principale!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    HWND hwndMain = CreateWindowEx(WS_EX_CLIENTEDGE, "MainWindowClass", "Nouvelle Fen�tre",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (hwndMain == NULL) {
        MessageBox(NULL, "�chec de la cr�ation de la fen�tre principale!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    g_louerButton = CreateWindow("BUTTON", "Louer voiture ", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        20, 10, 200, 50, hwndMain, (HMENU)1, GetModuleHandle(NULL), NULL);
    
    g_afficherButton = CreateWindow("BUTTON", "Afficher voiture", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        20, 100, 200, 50, hwndMain, (HMENU)1, GetModuleHandle(NULL), NULL);
    
    g_supprimerButton = CreateWindow("BUTTON", "Supprimer voiture", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        20, 200, 200, 50, hwndMain, (HMENU)1, GetModuleHandle(NULL), NULL);
        
    g_modifierButton = CreateWindow("BUTTON", "Modifier voiture", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        500, 10, 200, 50, hwndMain, (HMENU)1, GetModuleHandle(NULL), NULL);
    
    g_historiqueButton = CreateWindow("BUTTON", "Hostorique location", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        500, 100, 200, 50, hwndMain, (HMENU)1, GetModuleHandle(NULL), NULL);
    
    g_retourButton = CreateWindow("BUTTON", "Retour voiture", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        500, 200, 200, 50, hwndMain, (HMENU)1, GetModuleHandle(NULL), NULL);
    
    
    // Affichez la fen�tre principale
    ShowWindow(hwndMain, SW_SHOWNORMAL);
    UpdateWindow(hwndMain);
}

//////////fin menu///////////////////////////////////////////////

///////////Login Window/////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
    case WM_COMMAND:
        if (lParam == (LPARAM)g_loginButton) {
            char usernameBuffer[256], passwordBuffer[256];
            GetWindowTextA(g_usernameTextBox, usernameBuffer, sizeof(usernameBuffer));
            GetWindowTextA(g_passwordTextBox, passwordBuffer, sizeof(passwordBuffer));

            if (AuthenticateUser(usernameBuffer, passwordBuffer)) {
            	strcpy(g_connectedUserEmail, GetUserEmail(usernameBuffer));
                MessageBox(hwnd, "Login successful!", "Success", MB_OK | MB_ICONINFORMATION);
                // Cr�ez et affichez la nouvelle fen�tre principale
                    CreateMainWindow();
                    // Cachez la fen�tre d'authentification
                    ShowWindow(hwnd, SW_HIDE);
            } else {
                MessageBox(hwnd, "Login failed! Invalid username or password.", "Error", MB_OK | MB_ICONERROR);
            }
        } else if (lParam == (LPARAM)g_inscriButton) {
            // Cr�er et afficher la fen�tre d'inscription
            CreateInscriWindow();
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "WindowClass";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    
     // Enregistrez la classe pour la fen�tre d'inscription
    WNDCLASSEX wcInscription;
    memset(&wcInscription, 0, sizeof(wcInscription));
    wcInscription.cbSize = sizeof(WNDCLASSEX);
    wcInscription.lpfnWndProc = InscriptionWndProc;
    wcInscription.hInstance = hInstance;
    wcInscription.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcInscription.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcInscription.lpszClassName = "InscriptionWindowClass";
    wcInscription.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcInscription.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcInscription)) {
        MessageBox(NULL, "�chec de l'enregistrement de la classe de la fen�tre d'inscription!", "Erreur", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", "Login", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create labels
    g_usernameLabel = CreateWindow("STATIC", "Username:", WS_VISIBLE | WS_CHILD,
        10, 10, 80, 20, hwnd, NULL, hInstance, NULL);

    g_passwordLabel = CreateWindow("STATIC", "Password:", WS_VISIBLE | WS_CHILD,
        10, 40, 80, 20, hwnd, NULL, hInstance, NULL);

    // Create username text box
    g_usernameTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
        100, 10, 200, 20, hwnd, NULL, hInstance, NULL);

    // Create password text box
    g_passwordTextBox = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
        100, 40, 200, 20, hwnd, NULL, hInstance, NULL);

    // Create login button
    g_loginButton = CreateWindow("BUTTON", "Se connecter", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 70, 130, 30, hwnd, (HMENU)1, hInstance, NULL);
    
    g_inscriButton = CreateWindow("BUTTON", "Cr�er un compte", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        150, 70, 130, 30, hwnd, (HMENU)1, hInstance, NULL);

    if (g_usernameLabel == NULL || g_passwordLabel == NULL ||
        g_usernameTextBox == NULL || g_passwordTextBox == NULL || g_loginButton == NULL || g_inscriButton == NULL) {
        MessageBox(NULL, "Failed to create login interface elements!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    AddUser("tasnim", "test","tasnim@gmail.com");
    
    Car car1 = {"123ABC", "Red", "Sport car", 300, 45000.00,"Oui"}; 
    Car car2 = {"456DEF", "Blue", "Sedan", 150, 25000.00,"Oui"};  
	initializeFile(&carFile); 
    Enfiler(&carFile, car1);
    Enfiler(&carFile, car2);
    
    Location newLocation1 = {"Cupra", "tasnim@gmail.com", "26/12/2023","Location"}; 
    Location newLocation2 = {"BMW", "tasnim@gmail.com", "05/11/2023","Retour"};
    empiler(&LocationPile, &newLocation2);
    empiler(&LocationPile, &newLocation1);
    
    
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Cleanup: free the linked list memory
    struct Node* current = userList;
    while (current != NULL) {
        struct Node* next = current->next;
        free(current);
        current = next;
    }
    
     // Cleanup: free the file memory
   while (!isEmpty(&carFile)) {
        Car car = Defiler(&carFile);
    }
    
    return msg.wParam;
}

////////////fin login window/////////////////////////
