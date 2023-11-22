// Arturo Navarro y Jorge Bravo
// Coste = O(V + A)
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include "Digrafo.h"

using namespace std;

using Camino = std::deque<int>; // para representar caminos

// Utilizando el dfs de la funcion de busqueda de ciclos modificado para que lo haga solo con el 0
class CicloDirigido
{
private:
    std::vector<bool> visit;   // visit[v] = ¿se ha alcanzado a v en el dfs?
    std::vector<int> ant;      // ant[v] = vértice anterior en el camino a v
    std::vector<bool> apilado; // apilado[v] = ¿está el vértice v en la pila?
    Camino _ciclo;             // ciclo dirigido (vacío si no existe)
    bool hayciclo;             // hay ciclo dirigido

public:
    CicloDirigido(Digrafo const &g) : visit(g.V(), false), ant(g.V()),
                                      apilado(g.V(), false), hayciclo(false)
    {
        dfs(g, 0);
    }
    bool hayCiclo() const { return hayciclo; }
    Camino const &ciclo() const { return _ciclo; }
    vector<bool> visitas() const { return visit; }

    void dfs(Digrafo const &g, int v)
    {
        apilado[v] = true;
        visit[v] = true;
        for (int w : g.ady(v))
        {
            if (!visit[w])
            { // encontrado un nuevo vértice, seguimos
                ant[w] = v;
                dfs(g, w);
            }
            else if (apilado[w])
            { // hemos detectado un ciclo
                // se recupera retrocediendo
                hayciclo = true;
                for (int x = v; x != w; x = ant[x])
                    _ciclo.push_front(x);
                _ciclo.push_front(w);
                _ciclo.push_front(v);
            }
        }
        apilado[v] = false;
    }
};

// función que devuelve un valor dependiendo de si el recorrido
// es siempre, a veces o nunca posible
int tipoRecorrido(Digrafo &dg)
{
    CicloDirigido cd(dg);
    vector<bool> visit = cd.visitas(); // vector con vertices visitados
    int toReturn = 2;
    if (visit[dg.V() - 1] == true) // si el ultimo vertice esta visitado lo que marca el final
    {
        if (!cd.hayCiclo())
            toReturn = 0;
        else if (cd.hayCiclo())
            toReturn = 1;
    }
    return toReturn;
}

int main()
{
    int n; //Parseamos la entrada
    while (cin >> n)
    {
        Digrafo dg(n + 1);
        for (int i = 0; i < n; ++i)
        {
            char c;
            cin >> c;
            if (c == 'A')
                dg.ponArista(i, i + 1);
            else if (c == 'J' || c == 'C')
            {
                int position;
                cin >> position;
                if (c == 'J')
                    dg.ponArista(i, position - 1);
                else if (c == 'C')
                {
                    dg.ponArista(i, position - 1);
                    dg.ponArista(i, i + 1);
                }
            }
        }
        // escribir la solución llamando a tipoRecorrido()
        switch (tipoRecorrido(dg))
        {
        case 0:
            cout << "SIEMPRE";
            break;
        case 1:
            cout << "A VECES";
            break;
        case 2:
            cout << "NUNCA";
            break;
        }
        cout << "\n";
    }
    return 0;
}