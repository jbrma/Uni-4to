#include <iostream>
using namespace std;


int factor(int a) {

	if (a <= 1) {

		return 1;
	}

	return a * factor(a - 1);
	
}

int main() {

	int num;
	cin >> num;

	for (int i = 0; i < num; i++) {
		int res;

		int fact;
		cin >> fact;

		if (fact <= 1)
			res = 1;
		else if (fact > 4)
			res = 0;
		else
			res = factor(fact);


		cout << res % 10 << '\n';

	}

	return 0;
}

