#include <iostream>
using namespace std;

int main() {

	int num;
	cin >> num;

	int year;
	for (int i = 0; i < num; i++) {
		cin >> year;
		if (year > 0)
			cout << year - 1 << '\n';
		else
			cout << year << '\n';
	}

	return 0;
}