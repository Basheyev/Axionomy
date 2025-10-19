// Axionomy.cpp: определяет точку входа для приложения.
//

#include "Axionomy.h"
#include "engine/MarketEngine.h"

using namespace std;
using namespace Axionomy;


void productLoaderTest() {

	ProductsPricer marketPricer("data\\products.json");
	const ProductsList& products = marketPricer.getProductsList();

	if (products.size() == 0) {
		cerr << "Product list load failed for some reason \n";
		return;
	}

	size_t id = products[2].productID;

	for (int64_t i = 0; i < 40; i++) {
		Product product;			

		marketPricer.setDemandAndSupply(id, 110 - i, 100);			
		marketPricer.update(0);

		size_t index = marketPricer.getIndexByProductID(1);
		product = products[index];

		cout << "Product:" << product.name
			<< " Demand: " << product.demand
			<< " Supply: " << product.supply
			<< " Price: " << product.price
			<< " Cost: " << product.cost
			<< " Margin: " << (product.price - product.cost)
			<< endl;
	}

}


int main()
{	
	//ProductsPricer marketPricer("data\\products.json");
	productLoaderTest();
	return 0;
}
