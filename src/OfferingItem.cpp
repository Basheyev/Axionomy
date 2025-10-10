
#include "CoreEngine.h"

using namespace Axionomy;


OfferingItem::OfferingItem(Money basePrice, Quantity demand, Quantity supply, double importance) {
    this->basePrice = basePrice;
    this->demand = demand;
    this->supply = supply;
    this->importance = importance;
}

void OfferingItem::setBasePrice(Money basePrice) {
    this->basePrice = basePrice;
}

void OfferingItem::setDemand(Quantity amount) {
    this->demand = amount;
}

void OfferingItem::setSupply(Quantity amount) {
    this->supply = amount;
}


// importance (0,1]
void OfferingItem::setImportance(double importance) {
    this->importance = std::clamp(importance, 1e-6, 1.0);
}

Money OfferingItem::getPrice() {

    // disbalance sensitivity (maximum 1% deficit adds 10% to price)
    constexpr double maxElasticity = 12.305019857643899;
    double k = importance * maxElasticity;

    // sigmoid asymmetry parameters
    constexpr double minY = 0.4; // minimal price multiplier
    constexpr double maxY = 4.0; // maximum price multiplier
    constexpr double diff = maxY - minY;
    static const double v = std::log2((maxY - minY) / (1 - minY));
    
    // Division-by-zero protection (bias)
    constexpr double bias = 0.001;

    // measure disbalance and ratio
    double balanceAmount = double(demand - supply);
    double balanceRatio = balanceAmount / (double(supply) + bias);
   
    // evaluate asymmetric sigmoid [min, max)    
    double e = std::exp(-balanceRatio * k);    
    double sigmoid = minY + (diff / std::pow(1 + e, v));
   
    // evaluate price
    Money targetPrice = basePrice * std::clamp(sigmoid, minY, maxY);
        
    return targetPrice;
}
