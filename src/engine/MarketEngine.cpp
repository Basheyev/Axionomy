
#include "MarketEngine.h"


using namespace Axionomy;


MarketEngine::MarketEngine(const std::string& productsList) : productsPricer(productsList) {
    tickCounter = 0;
    size_t productsCount = productsPricer.getProductsList().size();
    aggregateDemand.reserve(productsCount);
    aggregateSupply.reserve(productsCount);
}


void MarketEngine::processTick() {    
    aggregateSupplyDemand();
    computeEquilibriumPrice();
    marketClearing();
    updateAgentsState();
    tickCounter++;
}


void MarketEngine::aggregateSupplyDemand() {

    // Clear aggregates
    for (auto& [key, value] : aggregateDemand) value = 0;
    for (auto& [key, value] : aggregateSupply) value = 0;

    // Compute demand/supply aggregates
    for (const EconomicAgent& agent : agents) {
        for (const Item& inputItem : agent.bids)
            aggregateDemand[inputItem.productID] += inputItem.quantity;
        for (const Item& outputItem : agent.asks)
            aggregateSupply[outputItem.productID] += outputItem.quantity;
    }

}


void MarketEngine::computeEquilibriumPrice() {

    const ProductsList& productsList = productsPricer.getProductsList();

    for (const Product& product : productsList) {
        Quantity totalDemand = aggregateDemand[product.productID];
        Quantity totalSupply = aggregateSupply[product.productID];
        productsPricer.computeEquilibriumPrice(product.productID, totalDemand, totalSupply);
    }

}


void MarketEngine::marketClearing() {


    // TODO: Make clearing by prices and volumes, and pro rata in same price segment  
    // TODO: make contracts and fulfillment


}


void MarketEngine::updateAgentsState() {
    for (EconomicAgent& agent : agents) {
        MarketContext mc;
        agent.tick(mc);
    }
}