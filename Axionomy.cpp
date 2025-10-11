// Axionomy.cpp: определяет точку входа для приложения.
//

#include "Axionomy.h"
#include "src/CoreEngine.h"

using namespace std;
using namespace Axionomy;

int main()
{

	for (int i = 0; i < 10; i++) {
		Quantity demand = 100 + i, supply = 100 - i;
		OfferingItem bread(1000, demand, supply, 0.2);

		cout << "Demand: " << demand
			<< " Supply: " << supply
			<< " Price: " << bread.getPrice() << endl;
	}

	return 0;
}
