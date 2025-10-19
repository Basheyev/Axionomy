
#include "engine/CoreEngine.h"

#include <iostream>
#include <utility>

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



bool ProductsPricer::getProductData(uint64_t productID, Product& product) {
    for (const Product& productData : products) {
        if (productData.productID == productID) {
            product = productData;
            return true;
        }
    }
    return false;
}


bool ProductsPricer::setDemandAndSupply(uint64_t productID, Quantity demand, Quantity supply) {
    for (Product& productData : products) {
        if (productData.productID == productID) {
            productData.demand = demand;
            productData.supply = supply;
            return true;
        }
    }

    return false;
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
