
#include "engine/CoreEngine.h"

#include <iostream>
#include <utility>

using namespace Axionomy;


ProductsPricer::ProductsPricer(const std::string& path) {
    products.reserve(16);
    size_t productsCount = ProductsLoader::loadProductList(path, products);
}



size_t ProductsPricer::getProductID(const std::string& productName) {
    for (size_t i = 0; i < products.size(); i++) {
        std::string& name = products[i].name;
        if (name == productName) return i;
    }
    return 0;
}

size_t ProductsPricer::getProductsCount() {
    return products.size();
}

uint64_t ProductsPricer::addProduct(Product product) {
    uint64_t index = products.size();
    product.productID = index;
    products.push_back(product);
    return index;
}


void ProductsPricer::setBasePrice(uint64_t id, Money basePrice) {
    products[id].basePrice = basePrice;
}

void ProductsPricer::setDemand(uint64_t id, Quantity amount) {
    products[id].demand = amount;
}

void ProductsPricer::setSupply(uint64_t id, Quantity amount) {
    products[id].supply = amount;
}


// importance (0,1]
void ProductsPricer::setImportance(uint64_t id, double importance) {
    products[id].importance = std::clamp(importance, 1e-6, 1.0);
}


Money ProductsPricer::getBasePrice(uint64_t id) {
    return products[id].basePrice;
}

Quantity ProductsPricer::getDemand(uint64_t id) {
    return products[id].demand;
}

Quantity ProductsPricer::getSupply(uint64_t id) {
    return products[id].supply;
}

double ProductsPricer::getImportance(uint64_t id) {
    return products[id].importance;
}

Money ProductsPricer::evaluatePrice(uint64_t id) {
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
