// Axionomy.cpp: определяет точку входа для приложения.
//

#include "Axionomy.h"
#include "engine/CoreEngine.h"

using namespace std;
using namespace Axionomy;


void productLoaderTest() {

	ProductsPricer marketPricer("data\\products.json");
	const std::vector<Product>& products = marketPricer.getProductsList();

	if (products.size() == 0) {
		cerr << "Product list load failed for some reason \n";
		return;
	}

	size_t id = products[2].productID;

	for (int64_t i = 0; i < 40; i++) {
		Product product;		
	//	marketPricer.setDemandAndSupply(0, 5, 10);
	//	marketPricer.evaluateProductPrice(0);
		marketPricer.setDemandAndSupply(id, 80 + i, 100);
		marketPricer.getProductData(id, product);

		cout << "Demand: " << product.demand
			<< " Supply: " << product.supply
			<< " Price: " << marketPricer.evaluateProductPrice(id) 
			<< " Cost: " << marketPricer.evaluateProductCost(id)
			<< endl;
	}


}


int main()
{	
	//ProductsPricer marketPricer("data\\products.json");
	productLoaderTest();
	return 0;
}
