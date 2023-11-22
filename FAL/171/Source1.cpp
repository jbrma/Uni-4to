#include <iostream>
using namespace std;

int main() {

	int casos;
	cin >> casos;

	for (int i = 0; i < casos; i++) {

		int max;
		int cont = 1;

		cin >> max;
		
		int next;
		cin >> next;

		while (next != 0) {
			if (max < next) {
				max = next;
				cont = 1;
			}
			else if (max == next) {
				cont++;
			}

			cin >> next;
		}
		
		cout << max << " " << cont << "\n";
	}
}