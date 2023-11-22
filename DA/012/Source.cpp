#include <iostream>
#include "TreeSet_AVL.h"

using namespace std;

int main() {

    int N, M;
    while (cin >> N && N != 0) {
        Set<int> tree;
        for (int i = 0; i < N; ++i) {
            int val;
            cin >> val;
            tree.insert(val);
        }

        cin >> M;
        for (int i = 0; i < M; ++i) {
            int k;
            cin >> k;
            try {
                int result = tree.kesimo(k);
                cout << result << endl;
            }
            catch (runtime_error& e) {
                cout << "??\n";
            }
        }

        cout << "---\n";
    }

    return 0;
}
