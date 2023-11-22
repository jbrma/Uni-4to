
// Jorge Bravo Mateos y Arturo Navarro

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

struct Candidatura {
	int votos;
	int escanyos;
	int indice;

	bool operator<(const Candidatura& otra) const {
		if (votos == otra.votos) {
			if (escanyos == otra.escanyos) {
				return indice < otra.indice;
			}
			return escanyos > otra.escanyos;
		}
		return votos > otra.votos;
	}
};

bool resuelveCaso() {

	// leer los datos de la entrada
	int c;
	int n;

	cin >> c;
	cin >> n;

	if (!cin)  // fin de la entrada
		return false;

	vector<Candidatura> candidaturas(c);

	for (int i = 0; i < c; i++) {
		cin >> candidaturas[i].votos;
		candidaturas[i].escanyos = 0;
		candidaturas[i].indice = i;
	}

	while (n > 0) {
		sort(candidaturas.begin(), candidaturas.end());

		candidaturas[0].escanyos++;
		n--;

		// Actualiza el coeficiente solo para la candidatura seleccionada
		candidaturas[0].votos /= (1 + candidaturas[0].escanyos);
	}

	sort(candidaturas.begin(), candidaturas.end(), [](const Candidatura& a, const Candidatura& b) {
		return a.indice < b.indice;
		});

	for (int i = 0; i < c; ++i) {
		cout << candidaturas[i].escanyos << (i == c - 1 ? "\n" : " ");
	}

	return true;
}

int main() {
#ifndef DOMJUDGE
	std::ifstream in("casos.txt");
	auto cinbuf = std::cin.rdbuf(in.rdbuf());
#endif

	while (resuelveCaso());

#ifndef DOMJUDGE
	std::cin.rdbuf(cinbuf);
	system("PAUSE");
#endif
	return 0;
}