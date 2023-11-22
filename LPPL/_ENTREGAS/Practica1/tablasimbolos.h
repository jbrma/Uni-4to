struct nodo {
    char *nombre;
    float valor;
    char *tipo;
    struct nodo *siguiente;
};

struct nodo *lista = NULL;

void insertarTS(char *id, float v, char *tipo) {
    for (struct nodo *nod = lista; nod != NULL; nod = nod->siguiente) {
        if (strcmp(nod->nombre, id) == 0) {
            nod->valor = v;
            nod->tipo = tipo;
            return;
        }
    }
    struct nodo *nod = malloc(sizeof(struct nodo));
    nod->nombre = id;
    nod->valor = v;
    nod->tipo = tipo;
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

void mostrarTS(){
    
    struct nodo *nod = lista;
    printf("Tabla de símbolos:\n");
    
    while (nod != NULL) {
        printf("Identificador: %s, Tipo de datos: %s, Valor: %.2f\n", nod->nombre, nod->tipo, nod->valor);
        nod = nod->siguiente;
    }
}