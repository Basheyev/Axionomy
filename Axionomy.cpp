// Axionomy.cpp: определяет точку входа для приложения.
//

#include "Axionomy.h"
#include "src/CoreEngine.h"

using namespace std;
using namespace Axionomy;

int main()
{

	Quantity demand = 100, supply = 99;
	OfferingItem bread(1000, demand, supply, 0.2);

	cout << "Get demand: " << demand << endl;
	cout << "Get supply: " << supply << endl;
	cout << "Current price: " << bread.getPrice() << endl;

	return 0;
}
