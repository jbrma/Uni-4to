//Alicia Castro y Jorge Bravo
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// { Precondicion: v es un vector de enteros de tamaño n, y n >= 1
    // y k es un umbral entre 0 y 2×10^9.}
// { Cota:  O(n)}
// { Complejidad: O(n log n)}
long long numParejas(int v[], int n, int k){

    sort(v , v + n);
    long long cont = 0;
    int i = 0, j = n - 1;

    while (i < j) {
        if (v[i] + v[j] <= k) {
            cont += (j - i);
            i++;
        }
        else {
            j--;
        }
    }

    return cont;
}


int main() {
    int n, k;

    while (true) {
        cin >> n >> k;
        if (n == 0 && k == 0) break;

        int v[200000];
        for (int i = 0; i < n; i++) {
            cin >> v[i];
        }

        long long result = numParejas(v, n, k);
        cout << result << endl;
    }

    return 0;
}