// Axionomy.cpp: определяет точку входа для приложения.
//

#include "Axionomy.h"
#include "engine/CoreEngine.h"

using namespace std;
using namespace Axionomy;


void producTest() {
	MarketPricer marketPricer(1);

	uint64_t id = marketPricer.addProduct({ 0, ProductType::Good, ProductUnit::Piece, 1000, 1000, 100, 100, 0.5 });


	for (int64_t i = 0; i < 40; i++) {
		marketPricer.setDemand(id, 80 + i);
		marketPricer.setSupply(id, 100);

		cout << "Demand: " << marketPricer.getDemand(id)
			<< " Supply: " << marketPricer.getSupply(id)
			<< " Price: " << marketPricer.evaluatePrice(id) << endl;
	}
}

json loadJson(const std::string& path) {
	json j;
	cout << filesystem::current_path() << std::endl;
	std::ifstream f(path);
	if (!f.is_open()) {
		cout << "failed to open" << std::endl;
	} else {		
		f >> j;
	}
	cout << j;
	return j;
}

int main()
{	
	
	loadJson("data\\products.json");

	return 0;
}
