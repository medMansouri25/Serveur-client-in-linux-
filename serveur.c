#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LONGMAX 100
#define SERVEUR_PORT 2022

int main(int argc, char *argv[]) {
    int sd, aSd, n;
    struct sockaddr_in servAddr, cliAddr;
    socklen_t cliLen;
    char buffer[LONGMAX];
    int entier;

    // Création de la socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        printf("socket non cree\n");
        return -1;
    }

    // Configuration de l'adresse du serveur
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERVEUR_PORT);

    // Attachement
    if (bind(sd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        printf("port occupe\n");
        return -1;
    }

    // Ecoute
    listen(sd, 5);
    printf("Attente sur port %d...\n", SERVEUR_PORT);

    while (1) {
        cliLen = sizeof(cliAddr);
        aSd = accept(sd, (struct sockaddr *) &cliAddr, &cliLen);
        if (aSd < 0) {
            printf("Connexion non acceptee\n");
            continue;
        }

        printf("Client connecte\n");

        while (1) {
            // Essayer de lire 4 octets pour voir si c'est un entier
            int test;
            int peek = recv(aSd, &test, sizeof(int), MSG_PEEK);

            if (peek <= 0) {
                printf("Fin communication par le client\n");
                close(aSd);
                break;
            }

            if (peek == sizeof(int) && test >= -1000000 && test <= 1000000) {
                // C'est probablement un entier
                recv(aSd, &entier, sizeof(int), 0);
                printf("✅ Nombre reçu : %d\n", entier);
            } else {
                // C'est une chaîne
                memset(buffer, 0, LONGMAX);
                n = recv(aSd, buffer, LONGMAX, 0);
                if (n > 0) {
                    printf("✅ Chaîne reçue : %s\n", buffer);
                } else {
                    printf("Erreur ou fin communication\n");
                    close(aSd);
                    break;
                }
            }
        }
    }

    close(sd);
    return 0;
}
