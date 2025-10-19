// Axionomy.cpp: определяет точку входа для приложения.
//

#include "Axionomy.h"
#include "engine/CoreEngine.h"

using namespace std;
using namespace Axionomy;


void producTest() {

	ProductsPricer marketPricer("data\\products.json");

	size_t id = marketPricer.getProductID("Bolt");

	for (int64_t i = 0; i < 40; i++) {
		marketPricer.setDemand(1, 80 + i);
		marketPricer.setSupply(1, 100);

		cout << "Demand: " << marketPricer.getDemand(id)
			<< " Supply: " << marketPricer.getSupply(id)
			<< " Price: " << marketPricer.evaluatePrice(id) << endl;
	}
}


int main()
{	
	ProductsPricer marketPricer("data\\products.json");
	
	return 0;
}
