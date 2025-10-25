
#include "MarketEngine.h"


using namespace Axionomy;


MarketEngine::MarketEngine(const std::string& productsList) : marketPricer(productsList) {

}


void MarketEngine::tick() {
    
    
}


void MarketEngine::aggregateSupplyDemand() {
    
    for (const EconomicAgent& agent : agents) {

        // accumulate supply
        // accumulate demand
        // accumulate stock
    }

}


void MarketEngine::computeEquilibriumPrice() {

}


void MarketEngine::clearMarketProRata() {

}


void MarketEngine::updateAgentsState() {

}