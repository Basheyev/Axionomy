// Axionomy.cpp: определяет точку входа для приложения.
//

#include "Axionomy.h"
#include "engine/CoreEngine.h"

using namespace std;
using namespace Axionomy;

int main()
{
	MarketPricer marketPricer(1);

	uint64_t id = marketPricer.addItem(1000,100,100,0.5);


	for (int64_t i = 0; i < 40; i++) {
		marketPricer.setDemand(id, 80 + i);
		marketPricer.setSupply(id, 100);

		cout << "Demand: " << marketPricer.getDemand(id)
			<< " Supply: " << marketPricer.getSupply(id)
			<< " Price: " << marketPricer.evaluatePrice(id) << endl;
	}

	return 0;
}
