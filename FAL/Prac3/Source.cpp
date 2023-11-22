//Alicia Castro y Jorge Bravo
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// { Precondicion: }
// { Cota:  }
// { Complejidad: }
 int entretenido(long long int n) {

    if (n < 10) {
        return n;
    }
    long long int aux = n;

    long long int ultimo = n % 10;
    n /= 10;
    long long int penultimo = n % 10;

    long long int consec = entretenido(aux-1);

    if (ultimo != penultimo) {
        return consec;
    }
    else {
        return consec + 1;
    }

}


int main() {
    long long int n;

    cin >> n;

    while (n != -1) {
        if (n > 0 && n < 10) {
            cout << n << endl;
        }
        else {
            cout << n - entretenido(n) << endl;
        }
        cin >> n;
    }
    return 0;
}