#include <iostream>
#include <vector>
using namespace std;

int abadias(vector<int> const& v) {
	int maxAbadias = 1, max = v[v.size() - 1]; //al inicio el maximo es la ultima montaña

	for (int i = v.size() - 2; i >= 0; --i) { //recorremos desde atras hacia delante
		if (v[i] > max) {
			maxAbadias++;
			max = v[i];
		}
	}

	return maxAbadias;
}

bool sol() {

	int num;
	cin >> num;
	if (num == 0)
		return false;

	vector<int> mont;

	for (int i = 0; i < num; i++){
		int ent;
		cin >> ent;
		mont.push_back(ent);
	}

	int total = abadias(mont);
	cout << total << '\n';

	return true;
}

int main() {

	while (sol());

	return 0;
}
