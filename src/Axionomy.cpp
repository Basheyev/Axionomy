// Axionomy.cpp: определяет точку входа для приложения.
//

#include "Axionomy.h"
#include "engine/CoreEngine.h"

using namespace std;
using namespace Axionomy;


void productLoaderTest() {

	ProductsPricer marketPricer("data\\products.json");

	size_t id = marketPricer.getProductsList()[0].productID;

	for (int64_t i = 0; i < 40; i++) {
		Product product;		
		marketPricer.setDemandAndSupply(1, 80 + i, 100);
		marketPricer.getProductData(1, product);

		cout << "Demand: " << product.demand
			<< " Supply: " << product.supply
			<< " Price: " << marketPricer.evaluatePrice(id) << endl;
	}
}


int main()
{	
	//ProductsPricer marketPricer("data\\products.json");
	productLoaderTest();
	return 0;
}
