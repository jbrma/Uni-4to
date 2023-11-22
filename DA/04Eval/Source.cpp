#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include "Grafo.h"

using namespace std;
const int INF = numeric_limits<int>::max();

// El coste del algoritmo es O(3V * log V) = O(V * log V)

vector<int> dijkstra(int source, Grafo& grafo, int V) // Usando el algoritmo dijkstra
{
    vector<int> distance(V, INF);
    distance[source] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({ 0, source });

    while (!pq.empty())
    {
        auto [dist, u] = pq.top();
        pq.pop();

        if (dist > distance[u])
            continue;

        for (int v : grafo.ady(u))
        {
            if (dist + 1 < distance[v])
            {
                distance[v] = dist + 1;
                pq.push({ distance[v], v });
            }
        }
    }

    return distance;
}

int findMinimumDistanceBetweenVertices(Grafo g, int V, int vertex1, int vertex2, int vertex3)
{
    // calculamos los tres caminos minimos desde cada vertice
    vector<int> dist1 = dijkstra(vertex1, g, V);
    vector<int> dist2 = dijkstra(vertex2, g, V);
    vector<int> dist3 = dijkstra(vertex3, g, V);

    int minDistance = INF;

    for (int i = 0; i < V; ++i)
    {
        minDistance = min(minDistance, dist1[i] + dist2[i] + dist3[i]);
    }

    return minDistance;
}

int main()
{
    int N, C, Alex, Lucas, Trabajo;
    while (cin >> N >> C >> Alex >> Lucas >> Trabajo)
    {
        Grafo g(N);
        for (int i = 0; i < C; ++i)
        {
            int u, v;
            cin >> u >> v;
            g.ponArista(u - 1, v - 1);
        }

        cout << findMinimumDistanceBetweenVertices(g, C, Alex - 1, Lucas - 1, Trabajo - 1) << endl;
    }

    return 0;
}