# Projet-C
Demo(https://clipchamp.com/watch/UGt9H83KSTR)
*E-Cras est une application de type Windows application développée en langage C à l'aide de l'éditeur DevC++.

-Les utilisateurs sont stockés dans une liste simplement chaînée.
-Les voitures sont stockées dans une file.
-L'historique des opérations (suppression, modification, retour et location) est stocké dans une pile.
-L'archivage de l'historique s'effectue dans un fichier.

*Chaque utilisateur est caractérisé par un nom d'utilisateur, un mot de passe et une adresse e-mail (récupérée lors de l'authentification et utilisée pour l'historique dans chaque opération).

*Chaque voiture est caractérisée par une référence, une couleur, une description, le nombre de chevaux, un prix et son état (disponible ou non).

*Chaque opération est caractérisée par une date (automatiquement récupérée à partir de la date système lors de l'opération), l'adresse e-mail de l'utilisateur ayant effectué l'opération, la référence de la voiture ayant subi l'opération et le type d'opération (location, modification, suppression et retour).

-Le code source est composé de deux fichiers :
- Fichier Definition.c : contient la définition des structures de données et les sous-programmes nécessaires pour la manipulation des structures de données.
- Fichier main.c : contient la fonction main ainsi que l'ensemble des fenêtres de l'application.

*Initialisation:
    user1={"tasnim", "test","tasnim@gmail.com"}
    car1 = {"Ferari", "Red", "Sport car", 300, 45000.00,"Oui"}
    car2 = {"Golf 7", "Blue", "Phase2", 6, 8000.00,"Oui"}
    car3 = {"Ford", "Green", "4*4", 8, 5000.00,"Non"}
    operation1 = {"Cupra", "tasnim@gmail.com", "26/11/2023","Location"}; 
    operation2 = {"BMW", "tasnim@gmail.com", "05/10/2023","Retour"};

Diagramme de cas d'utilisation:
![UseCase(E-Cars)](https://github.com/LouatiTasnim/Projet-C/assets/120528076/7de17645-a833-493b-9886-73024e9d03b1)


