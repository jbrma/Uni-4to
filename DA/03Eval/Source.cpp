#include <iostream>
#include <limits>
#include <unordered_map>
#include "IndexPQ.h" // Incluimos la implementación de la cola con prioridad

using namespace std;

struct Tema
{
    string nombre;
    int citas;

    Tema(const string& n, int c) : nombre(n), citas(c) {}
    Tema() : nombre(""), citas(0) {}
};

int main()
{
    string tipo, tema;
    int n, citas;

    while (cin >> n)
    {
        unordered_map<string, Tema> temas;           // Mapa para almacenar los temas
        IndexPQ<int, greater<int>> maxHeap(1000000); // Montículo para mantener el top de temas
        IndexPQ<int, greater<int>> maxHeapCopy(1000000);

        for (int i = 0; i < n; ++i)
        {
            cin >> tipo;

            if (tipo == "C" || tipo == "E")
            {
                cin >> tema >> citas;

                if (tipo == "C")
                {
                    auto it = temas.find(tema);
                    if (it == temas.end())
                    {
                        temas[tema] = Tema(tema, citas);
                        maxHeap.push(citas, citas);
                    }
                    else
                    {
                        temas[tema].citas += citas;
                        maxHeap.update(temas[tema].citas, temas[tema].citas);
                    }
                }
                else
                { // tipo == "E"
                    if (temas.find(tema) != temas.end())
                    {
                        temas[tema].citas -= citas;
                        maxHeap.update(temas[tema].citas, temas[tema].citas);
                    }
                }
            }

            else if (tipo == "TC")
            {
                int count = 0;
                maxHeapCopy = maxHeap;
                while (!maxHeapCopy.empty() && count < 3)
                {
                    int maxCitas = maxHeapCopy.top().prioridad;

                    // Buscar el tema con el número de citas máximo
                    for (const auto& tema : temas)
                    {
                        if (tema.second.citas == maxCitas)
                        {
                            cout << count + 1 << " " << tema.second.nombre << endl;
                            count++;
                        }

                        if (count >= 3)
                            break;
                    }

                    // Eliminar el tema con el número de citas máximo del montículo
                    maxHeapCopy.update(maxCitas, numeric_limits<int>::max());
                }
            }
        }
        cout << "---" << endl;
    }

    return 0;
}