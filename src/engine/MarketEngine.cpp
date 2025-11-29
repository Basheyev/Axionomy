
#include "MarketEngine.h"

#include <unordered_set>


using namespace Axionomy;


MarketEngine::MarketEngine(const std::string& productsList) : productsPricer(productsList) {
    tickCounter = 0;
    size_t productsCount = productsPricer.getProductsList().size();
    aggregateDemand.reserve(productsCount);
    aggregateSupply.reserve(productsCount);
}


//----------------------------------------------------------------------------------------------------
// Process economy simulation tick
//----------------------------------------------------------------------------------------------------
void MarketEngine::processTick() {    
    updateAgentsState();
    aggregateSupplyDemand();
    computeEquilibriumPrice();
    processMarketClearing();    
    tickCounter++;
}



//----------------------------------------------------------------------------------------------------
// Process agents next step
//----------------------------------------------------------------------------------------------------
void MarketEngine::updateAgentsState() {
    for (EconomicAgent& agent : agents) {
        // provide market context
        agent.tick();
    }
}

//----------------------------------------------------------------------------------------------------
// Copy all orders from agents to market orders book and calculate aggregates
//----------------------------------------------------------------------------------------------------
void MarketEngine::aggregateSupplyDemand() {

    // Clear aggregates
    for (auto& [productID, orders] : ordersBook) orders.clear();
    for (auto& [productID, demandVolume] : aggregateDemand) demandVolume = 0;
    for (auto& [productID, supplyVolume] : aggregateSupply) supplyVolume = 0;

    // Compute demand/supply aggregates and make Product order books
    for (const EconomicAgent& agent : agents) {
        
        // Aggregate all bid orders by products
        for (const Order& bid : agent.bids)                     // Iterate over all agents bid orders
        if (bid.quantity > 0) {                                 // Discard zero or negative quantities
            aggregateDemand[bid.productID] += bid.quantity;     // Compute demand aggregate
            auto& productOrderBook = ordersBook[bid.productID]; // Get product Orders Book
            productOrderBook.push_back(bid);                    // Copy bid order to Orders Book
        }
        
        // Aggregate all ask orders by products
        for (const Order& ask : agent.asks)                     // Iterate over all agents ask orders
        if (ask.quantity > 0) {                                 // Discard zero or negative quantities 
            aggregateSupply[ask.productID] += ask.quantity;     // Compute supply aggregate 
            auto& productOrderBook = ordersBook[ask.productID]; // Get product Ask Orders Book
            productOrderBook.push_back(ask);                    // Copy ask order to Ask Orders Book
        }

    }

}


//----------------------------------------------------------------------------------------------------
// Calculate equilibrium price for each product using aggegates in products pricer
//----------------------------------------------------------------------------------------------------
void MarketEngine::computeEquilibriumPrice() {

    const ProductsList& productsList = productsPricer.getProductsList();

    for (const Product& product : productsList) {
        Quantity totalDemand = aggregateDemand[product.productID];
        Quantity totalSupply = aggregateSupply[product.productID];
        productsPricer.computeEquilibriumPrice(product.productID, totalDemand, totalSupply);
    }

}


//----------------------------------------------------------------------------------------------------
// Process market clearing
//----------------------------------------------------------------------------------------------------
void MarketEngine::processMarketClearing() {

    const ProductsList& productsList = productsPricer.getProductsList();

    // Iterate over all market products
    for (const Product& product : productsList) {
        // If product demand and supply is greater than zero then do the clearing
        ProductID productID = product.productID;
        if (aggregateDemand[productID] > 0 && aggregateSupply[productID] > 0) {
            processProductClearing(productID);
        }
    }

}



//----------------------------------------------------------------------------------------------------
// Pro rata product clearing
//----------------------------------------------------------------------------------------------------
void MarketEngine::processProductClearing(const ProductID productID) {
    
    auto orders = ordersBook[productID];
    auto demand = aggregateDemand[productID];
    auto supply = aggregateSupply[productID];

    // build price grid (with matching bid/ask)
    // fulfill best prices
    // same prices fulfill pro rata
    //

   
}

