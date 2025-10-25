
#include "MarketEngine.h"


using namespace Axionomy;


MarketEngine::MarketEngine(const std::string& productsList) : productsPricer(productsList) {
    tickCounter = 0;
    size_t productsCount = productsPricer.getProductsList().size();
    aggregateDemand.reserve(productsCount);
    aggregateSupply.reserve(productsCount);
}


void MarketEngine::tick() {    
    aggregateSupplyDemand(aggregateDemand, aggregateSupply);
    computeEquilibriumPrice(aggregateDemand, aggregateSupply);
    clearMarketProRata(aggregateDemand, aggregateSupply);
    updateAgentsState();
    tickCounter++;
}


void MarketEngine::aggregateSupplyDemand(ProductsAggregate& demand, ProductsAggregate& supply) {

    // Clear aggregates
    for (auto& [key, value] : aggregateDemand) value = 0;
    for (auto& [key, value] : aggregateSupply) value = 0;

    // Compute demand/supply aggregates
    for (const EconomicAgent& agent : agents) {
        for (const Item& inputItem : agent.bids)
            demand[inputItem.productID] += inputItem.quantity;
        for (const Item& outputItem : agent.asks)
            supply[outputItem.productID] += outputItem.quantity;
    }

}


void MarketEngine::computeEquilibriumPrice(const ProductsAggregate& demand, const ProductsAggregate& supply) {

    ProductsList productsList = productsPricer.getProductsList();

    for (const Product& product : productsList) {
        Quantity totalDemand = aggregateDemand[product.productID];
        Quantity totalSupply = aggregateSupply[product.productID];
        productsPricer.computeEquilibriumPrice(product.productID, totalDemand, totalSupply);
    }

}


void MarketEngine::clearMarketProRata(ProductsAggregate& demand, ProductsAggregate& supply) {


    // clear between agent pro rata


}


void MarketEngine::updateAgentsState() {
    for (EconomicAgent& agent : agents) {
        agent.tick();
    }
}