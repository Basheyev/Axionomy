
#include "engine/pricer/ProductsPricer.h"

#include <algorithm>


using namespace Axionomy;


ProductsPricer::ProductsPricer(const std::string& path) {    
    size_t productsCount = ProductsLoader::loadProductList(path, products);
    if (productsCount == 0) {

        // TODO: do something about absenñe of file

    }
}


const std::vector<Product>& ProductsPricer::getProductsList() const {
    return products;
}


size_t ProductsPricer::findProductIndexByID(uint64_t id) {
    for (size_t index = 0; index < products.size(); index++) {
        if (products[index].productID == id) return index;
    }
    return NOT_FOUND;
}


Money ProductsPricer::getProductPrice(uint64_t productID) {
    size_t index = findProductIndexByID(productID);
    if (index == NOT_FOUND) return 0ULL;
    return products[index].price;
}


bool ProductsPricer::getProductData(uint64_t productID, Product& product) {
    size_t index = findProductIndexByID(productID);
    if (index == NOT_FOUND) return false;
    product = products[index];
    return true;
}


bool ProductsPricer::setDemandAndSupply(uint64_t productID, Quantity demand, Quantity supply) {
    size_t index = findProductIndexByID(productID);
    if (index == NOT_FOUND) return false;
    Product& productData = products[index];
    productData.demand = demand;
    productData.supply = supply;
    return true;
}



Money ProductsPricer::evaluatePrice(uint64_t id) {
    // fetch values and convert to double
    double demand = double(products[id].demand);
    double supply = double(products[id].supply);
    double basePrice = products[id].cost;

    // disbalance sensitivity (maximum 1% deficit adds 10% to price)
    constexpr double maxElasticity = 12.305019857643899;
    double k = products[id].importance * maxElasticity;

    // sigmoid asymmetry parameters
    constexpr double minY = 0.4; // minimal price multiplier
    constexpr double maxY = 4.0; // maximum price multiplier
    constexpr double diff = maxY - minY;
    static const double v = std::log2((maxY - minY) / (1 - minY));
    
    // Division-by-zero protection (bias)
    constexpr double bias = 0.001;

    // measure disbalance and ratio
    double balanceAmount = demand - supply;
    double balanceRatio = balanceAmount / (supply + bias);
   
    // evaluate asymmetric sigmoid [min, max)    
    double e = std::exp(-balanceRatio * k);    
    double sigmoid = minY + (diff / std::pow(1 + e, v));
   
    // evaluate price
    Money targetPrice = basePrice * std::clamp(sigmoid, minY, maxY);
    
    // TODO: add inertia
    products[id].price = targetPrice;

    return targetPrice;
}


Money ProductsPricer::evaluateProductCost(uint64_t productID) {
    size_t index = findProductIndexByID(productID);
    if (index == NOT_FOUND) return 0ULL;
    Product& productData = products[index];

    BillOfMaterials& bom = productData.materials;

    Money cost = 0;

    for (std::pair<uint64_t, double>& component : bom) {
        Money price = products[component.first].price;
        double amount = component.second;
        cost += price * amount;
    }

    productData.cost = cost;

    return cost;
}