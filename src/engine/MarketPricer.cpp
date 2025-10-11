
#include "CoreEngine.h"

using namespace Axionomy;


MarketPricer::MarketPricer(uint32_t size) {
    offerings.reserve(size);
}


uint32_t MarketPricer::addItem(Money basePrice, Quantity demand, Quantity supply, double importance) {
    uint32_t index = uint32_t(offerings.size());
    offerings.push_back({index, 0, basePrice, demand, supply, importance});
    return index;
}


void MarketPricer::setBasePrice(uint32_t id, Money basePrice) {
    offerings[id].basePrice = basePrice;
}

void MarketPricer::setDemand(uint32_t id, Quantity amount) {
    offerings[id].demand = amount;
}

void MarketPricer::setSupply(uint32_t id, Quantity amount) {
    offerings[id].supply = amount;
}


// importance (0,1]
void MarketPricer::setImportance(uint32_t id, double importance) {
    offerings[id].importance = std::clamp(importance, 1e-6, 1.0);
}


Money MarketPricer::getBasePrice(uint32_t id) {
    return offerings[id].basePrice;
}

Quantity MarketPricer::getDemand(uint32_t id) {
    return offerings[id].demand;
}

Quantity MarketPricer::getSupply(uint32_t id) {
    return offerings[id].supply;
}

double MarketPricer::getImportance(uint32_t id) {
    return offerings[id].importance;
}

Money MarketPricer::getPrice(uint32_t id) {
    // fetch values and convert to double
    double demand = double(offerings[id].demand);
    double supply = double(offerings[id].supply);
    double basePrice = offerings[id].basePrice;

    // disbalance sensitivity (maximum 1% deficit adds 10% to price)
    constexpr double maxElasticity = 12.305019857643899;
    double k = offerings[id].importance * maxElasticity;

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
        
    return targetPrice;
}
