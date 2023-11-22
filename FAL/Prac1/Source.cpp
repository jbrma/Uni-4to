//Alicia Castro y Jorge Bravo
#include <iostream>
#include <vector>

using namespace std;
const int MAXIMO = 200000;

// { Precondicion: El vector v[] tiene numElem elementos, donde 1 <= n <= MAXIMO. }
// { Cota: O(n) }
// { Complejidad: El algoritmo recorre el vector v[] una vez, por lo que O(numElem) }
int misterio(int v[], int n) {

    int maxValue1 = -100001;
    int maxValue2 = -100001;

    for (int i = 0; i < n; ++i) {
        if (v[i] > maxValue1) {
            maxValue2 = maxValue1;
            maxValue1 = v[i];
        }
        else if (v[i] > maxValue2) {
            maxValue2 = v[i];
        }
    }

    return maxValue1 + maxValue2;
}


int main() {
    int numCasos;
    cin >> numCasos;

    int aux[MAXIMO];

    while (numCasos--) {
        int numElem;
        cin >> numElem;

        for (int i = 0; i < numElem; i++) {
            cin >> aux[i];
        }

        cout << misterio(aux, numElem) << endl;
    }

    return 0;
}