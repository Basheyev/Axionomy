// Axionomy.cpp: определяет точку входа для приложения.
//

#include "Axionomy.h"
#include "engine/CoreEngine.h"

using namespace std;
using namespace Axionomy;

int main()
{
	MarketPricer marketPricer(1);

	uint32_t id = marketPricer.addItem(1000,100,100,0.5);


	for (int i = 0; i < 10; i++) {
		marketPricer.setDemand(id, 100 + i);
		marketPricer.setSupply(id, 100 - i);

		cout << "Demand: " << marketPricer.getDemand(id)
			<< " Supply: " << marketPricer.getSupply(id)
			<< " Price: " << marketPricer.getPrice(id) << endl;
	}

	return 0;
}
