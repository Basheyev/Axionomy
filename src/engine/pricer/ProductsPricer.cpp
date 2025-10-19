/**============================================================================
 * 
 * @class ProductsPricer
 * @brief Evaluates product costs and market prices based on demand, supply,
 *        and bill of materials (BoM).
 *
 * The ProductsPricer class loads a list of products from a JSON file and
 * maintains an indexed catalog for fast lookup by product ID. It calculates
 * each product’s cost using the prices of its BoM components and evaluates
 * market prices dynamically using an asymmetric sigmoid function that models
 * the relationship between demand, supply, and price elasticity.
 *
 * Main responsibilities:
 *  - Load and index all products from a JSON catalog.
 *  - Compute product costs from Bill of Materials.
 *  - Evaluate product prices based on demand–supply imbalance.
 *  - Update market data such as demand and supply for each product.
 *
 * Notes:
 *  - Lookup operations are O(1) using an internal hash map.
 *  - Cost and price evaluations are O(N).
 *  - Not thread-safe for concurrent modifications.
 *  - Designed for use in economic and agent-based simulations.
 *
 * (C) Axionomy, Bolat Basheyev 2025
 *
 *=============================================================================*/


#include "engine/pricer/ProductsPricer.h"
#include <algorithm>


using namespace Axionomy;


ProductsPricer::ProductsPricer(const std::string& path) {    

    // Load products
    size_t productsCount = ProductsLoader::loadProductList(path, products);    
    if (productsCount == 0) {

        // TODO: do something about absenñe of file

    }

    // Create index
    indexByID.reserve(productsCount);
    for (size_t index = 0; index < products.size(); index++) {
        uint64_t id = products[index].productID;
        indexByID.emplace(id, index);
    }

}


const ProductsList& ProductsPricer::getProductsList() const {
    return products;
}


size_t ProductsPricer::findProductIndexByID(uint64_t productID) {
    auto it = indexByID.find(productID);
    if (it == indexByID.end()) return NOT_FOUND;
    return it->second;
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



void ProductsPricer::tick(double deltaTime) {

    for (Product& product : products) {
        evaluateProductPrice(product);
    }
}



void ProductsPricer::evaluateProductPrice(Product& product) {

    // fetch values and convert to double
    double demand = double(product.demand);
    double supply = double(product.supply);
    
    // disbalance sensitivity (maximum 1% deficit adds 10% to price)
    constexpr double maxElasticity = 12.305019857643899;
    double k = product.importance * maxElasticity;

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

    // Update product cost using bill of materials
    evaluateProductCost(product);

    // Add minimal industry margin to the cost
    double basePrice = product.cost * (1.0 + product.floorMargin);

    // evaluate target price
    Money targetPrice = basePrice * std::clamp(sigmoid, minY, maxY);
    
    // TODO: exponential interpolation of price (make speed adjustable)
    constexpr double ticksToAdjust = 7.0; // Days to adjust
    constexpr double alpha = 1.0 / ticksToAdjust;
    product.price += alpha * (targetPrice - product.price);
}


void ProductsPricer::evaluateProductCost(Product& product) {

    BillOfMaterials& bom = product.materials;

    Money cost = 0;

    if (bom.size() > 0) {
        for (std::pair<uint64_t, double>& component : bom) {
            uint64_t componentID = component.first;
            // straightforward non recursive approach
            Money price = getProductPrice(componentID);
            double amount = component.second;
            cost += price * amount;
        }
        product.cost = cost;
    }     

}