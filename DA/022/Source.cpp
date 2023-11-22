
/*@ <answer>
 *
 * Nombre y Apellidos:
 *
 *@ </answer> */

#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

// #include "..."  // propios o los de las estructuras de datos de clase

struct registro {
    int momento;
    int id;
    int periodo;
};

bool operator<(registro const& a, registro const& b) {
    return b.momento < a.momento || (a.momento == b.momento && b.id < a.id);
}

/*@ <answer>

 Escribe aqu� un comentario general sobre la soluci�n, explicando c�mo
 se resuelve el problema y cu�l es el coste de la soluci�n, en funci�n
 del tama�o del problema.

 @ </answer> */


 // ================================================================
 // Escribe el c�digo completo de tu soluci�n aqu� debajo
 // ================================================================
 //@ <answer>

bool resuelveCaso() {

    // leer los datos de la entrada
    int num;
    cin >> num;

    if (num == 0) return false;

    priority_queue<registro> cola;

    // resolver el caso posiblemente llamando a otras funciones

    for (int i = 0; i < num; ++i) { //O(NlogN)
        int idd, per;
        cin >> idd >> per;
        cola.push({ per, idd, per }); //O(logN)
    }

    // escribir la soluci�n

    int envios;
    cin >> envios;

    while (envios--) {//O(KlogN)
        auto e = cola.top(); /*O(1)*/ cola.pop(); //O(logN)
        cout << e.id << '\n';
        e.momento += e.periodo;
        cola.push(e);
    }
    cout << "---\n";
    return true;

    // ...>   O(NlogN)  +  O(KlogN)
}

//@ </answer>
//  Lo que se escriba dejado de esta l�nea ya no forma parte de la soluci�n.

int main() {
    // ajustes para que cin extraiga directamente de un fichero
#ifndef DOMJUDGE
    std::ifstream in("casos.txt");
    auto cinbuf = std::cin.rdbuf(in.rdbuf());
#endif

    resuelveCaso();

    // para dejar todo como estaba al principio
#ifndef DOMJUDGE
    std::cin.rdbuf(cinbuf);
    system("PAUSE");
#endif
    return 0;
}
