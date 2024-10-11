#include <stddef.h>

struct nodo {
    char *nombre;
    float valor;
    struct nodo *siguiente;
};

struct nodo *lista = NULL;

void insertarTS(char *id, float v) {
    for (struct nodo *nod = lista; nod != NULL; nod = nod->siguiente) {
        if (strcmp(nod->nombre, id) == 0) {
            nod->valor = v;
            return;
        }
    }
    struct nodo *nod = malloc(sizeof(struct nodo));
    nod->nombre = id;
    nod->valor = v;
    nod->siguiente = lista;
    lista = nod;
}

float buscarTS(char *id) {
    for (struct nodo *nod = lista; nod != NULL; nod = nod->siguiente) {
        if (strcmp(nod->nombre, id) == 0) {
            return nod->valor;
        }
    }
    printf("(El identificador '%s' no esta definido !!!)", id);
    return 0;
}