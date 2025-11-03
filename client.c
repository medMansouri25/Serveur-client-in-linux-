#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define LONGMAX 100
#define SERVEUR_PORT 2022

int main(int argc , char **argv[]){
    int sd, rc, i;
    struct sockaddr_in localAddr, servAddr;
    struct hostent *h;

    if(argc < 3){
        printf("usage: %s <SERVEUR>,<DATA1>............\n", argv[0]);
        exit(1);
    }

    h = gethostbyname(argv[1]);
    if(h==NULL){
        printf("%s : hote inconnu: %s\n", argv[0], argv[1]);
        exit(1);
    }

    servAddr.sin_family = h->h_addrtype;
    memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    servAddr.sin_port = htons(SERVEUR_PORT);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd<0){
        printf("socket non cree \n");
        exit(1);
    }

    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(0);

    rc = bind(sd,(struct sockaddr *) &localAddr, sizeof(localAddr));
    if(rc<0){
        printf("%s:port occupe %u\n",argv[0], SERVEUR_PORT);
        exit(1);
    }

    rc = connect(sd,(struct sockaddr *) &servAddr, sizeof(servAddr));
    if(rc<0){
        printf("erreur connect \n");
        exit(1);
    }

    // Envoi de chaque argument (data)
    for(i = 2; i < argc; i++){
        int nombre;

        if(sscanf(argv[i], "%d", &nombre) == 1){
            // L'argument est un entier
            rc = send(sd, &nombre, sizeof(int), 0);
            if(rc < 0){
                printf("cannot send data\n");
                close(sd);
                exit(1);
            }
            printf("%s: entier %d envoye\n", argv[0], nombre);
        } else {
            // L'argument est une chaîne
            rc = send(sd, argv[i], strlen(argv[i]) + 1, 0);
            if(rc < 0){
                printf("cannot send data\n");
                close(sd);
                exit(1);
            }
            printf("%s: chaine envoyee (%s)\n", argv[0], argv[i]);
        }
    }

    close(sd);
    return 0;
}
