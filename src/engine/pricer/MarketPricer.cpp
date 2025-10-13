
#include "../CoreEngine.h"
#include <iostream>

using namespace Axionomy;


MarketPricer::MarketPricer(const std::string& path) {

    productsCount = loadProductList(path);

}



size_t MarketPricer::loadProductList(const std::string& path) {
    
    std::ifstream productListFile(path);
    if (!productListFile.is_open()) return 0;

    json productList;

    // try/catch
    productListFile >> productList;

    // parse JSON array

    size_t count = productList.size();

    for (size_t i = 0; i < count; i++) {

        json productJSON = productList.at(i);

        std::cout << "Prodict Number " << i << std::endl << productJSON << std::endl;
    }

    return count;
}


size_t MarketPricer::getProductID(const std::string& productName) {
    for (size_t i = 0; i < names.size(); i++) {
        std::string& name = names[i];
        if (name == productName) return i;
    }
    return 0;
}

size_t MarketPricer::getProductsCount() {
    return productsCount;
}

uint64_t MarketPricer::addProduct(Product product) {
    uint64_t index = products.size();
    product.productID = index;
    products.push_back(product);
    return index;
}


void MarketPricer::setBasePrice(uint64_t id, Money basePrice) {
    products[id].basePrice = basePrice;
}

void MarketPricer::setDemand(uint64_t id, Quantity amount) {
    products[id].demand = amount;
}

void MarketPricer::setSupply(uint64_t id, Quantity amount) {
    products[id].supply = amount;
}


// importance (0,1]
void MarketPricer::setImportance(uint64_t id, double importance) {
    products[id].importance = std::clamp(importance, 1e-6, 1.0);
}


Money MarketPricer::getBasePrice(uint64_t id) {
    return products[id].basePrice;
}

Quantity MarketPricer::getDemand(uint64_t id) {
    return products[id].demand;
}

Quantity MarketPricer::getSupply(uint64_t id) {
    return products[id].supply;
}

double MarketPricer::getImportance(uint64_t id) {
    return products[id].importance;
}

Money MarketPricer::evaluatePrice(uint64_t id) {
    // fetch values and convert to double
    double demand = double(products[id].demand);
    double supply = double(products[id].supply);
    double basePrice = products[id].basePrice;

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
    products[id].currentPrice = targetPrice;

    return targetPrice;
}
