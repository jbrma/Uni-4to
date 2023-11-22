#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "PriorityQueue.h"

using namespace std;

struct Candidatura {
	int coeficiente;
	int votos;
	int escanos;
	int indice;

	bool operator<(const Candidatura& otra) const {
		if (votos * (1 + otra.escanos) == otra.votos * (1 + escanos)) {
			if (votos == otra.votos) {
				return indice < otra.indice;
			}
			return votos > otra.votos;
		}
		return votos * (1 + otra.escanos) > otra.votos * (1 + escanos);
	}
};

bool resuelveCaso() {

	// leer los datos de la entrada
	int c, n;

	cin >> c >> n;

	if (c == 0 && n == 0)  // fin de la entrada
		return false;

    
	vector<Candidatura> candidaturas(c);
	// Crear una cola de prioridad para almacenar las candidaturas
	PriorityQueue<Candidatura> cola(candidaturas);

	for (int i = 0; i < c; i++) {
		cin >> candidaturas[i].votos;
		candidaturas[i].escanos = 0;
		candidaturas[i].indice = i;
        candidaturas[i].coeficiente = candidaturas[i].votos;
		cola.push(candidaturas[i]);
	}

	// Asignar escaños a las candidaturas
	while (n > 0) {
		// Obtener la candidatura con mayor coeficiente
		Candidatura c = cola.top();
		cola.pop();


		// Actualizar el coeficiente de la candidatura
		c.escanos++;
		c.coeficiente = c.votos / (1 + c.escanos);

        candidaturas[c.indice].escanos = c.escanos;

		// Asignar el escaño a la candidatura
		cola.push(c);
		n--;
	}

    // Imprimir los resultados
	for (int i = 0; i < c; ++i) {
		cout << candidaturas[i].escanos << (i == c - 1 ? "\n" : " ");
	}    

	return true;
}

int main() {
	while(resuelveCaso()){}
	return 0;
}
