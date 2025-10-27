
#include "MarketEngine.h"


using namespace Axionomy;


MarketEngine::MarketEngine(const std::string& productsList) : productsPricer(productsList) {
    tickCounter = 0;
    size_t productsCount = productsPricer.getProductsList().size();
    aggregateDemand.reserve(productsCount);
    aggregateSupply.reserve(productsCount);
}


void MarketEngine::processTick() {    
    updateAgentsState();
    aggregateSupplyDemand();
    computeEquilibriumPrice();
    marketClearing();    
    tickCounter++;
}



//----------------------------------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------------------------------
void MarketEngine::aggregateSupplyDemand() {

    // Clear aggregates
    for (auto& [productID, bidOrders] : bidOrdersBook) bidOrders.clear();
    for (auto& [productID, askOrders] : askOrdersBook) askOrders.clear();
    for (auto& [productID, demandVolume] : aggregateDemand) demandVolume = 0;
    for (auto& [productID, supplyVolume] : aggregateSupply) supplyVolume = 0;

    // Compute demand/supply aggregates and make Product order books
    for (const EconomicAgent& agent : agents) {
        
        // Aggregate all bid orders by products
        for (const Order& bid : agent.bids)                   // Iterate over all agents bid orders
        if (bid.quantity > 0) {                               // Discard zero or negative quantities
            aggregateDemand[bid.productID] += bid.quantity;   // Compute demand aggregate
            auto& productBids = bidOrdersBook[bid.productID]; // Get product Bid Orders Book
            productBids.push_back(bid);                       // Copy bid order to Ask Orders Book
        }
        
        // Aggregate all ask orders by products
        for (const Order& ask : agent.asks)                   // Iterate over all agents ask orders
        if (ask.quantity > 0) {                               // Discard zero or negative quantities 
            aggregateSupply[ask.productID] += ask.quantity;   // Compute supply aggregate 
            auto& productAsks = askOrdersBook[ask.productID]; // Get product Ask Orders Book
            productAsks.push_back(ask);                       // Copy ask order to Ask Orders Book
        }

    }

}


//----------------------------------------------------------------------------------------------------
// 
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
// 
//----------------------------------------------------------------------------------------------------
void MarketEngine::marketClearing() {

    // TODO: Make clearing by prices and volumes, and pro rata in same price segment  
    // TODO: make contracts and fulfillment

    /* Algorithm:
    *


2. Price grid.
Collect all unique prices from Bid ∪ Ask. These are the candidate clearing prices. It is useful to track bestBid = max(Bid.price) and bestAsk = min(Ask.price).

3. Cumulative volumes at each candidate price p.
CumBid(p): total demand from all bids with price ≥ p.
CumAsk(p): total supply from all asks with price ≤ p.

4. Executable volume at price p.
Vol(p) = min(CumBid(p), CumAsk(p)).
This is the trading volume achievable if p is selected as the clearing price.

5. Selecting the clearing price p*.
Find V* = maxₚ Vol(p). All p with Vol(p) = V* are candidates.
Apply deterministic tie-breakers:
– minimize imbalance |CumBid(p) − CumAsk(p)|
– choose the price closest to (bestBid + bestAsk) / 2
– if still tied: apply a market pressure rule (if demand > supply choose the higher price; if supply > demand choose the lower price; or use a fixed rule such as “lower price wins”)

6. Full executions “in-the-money.”
All bids with price > p* are fully executed.
All asks with price < p* are fully executed.
Sum their volumes: QB_full and QA_full.

7. Residual volume at p*.
Total traded volume Q* = V*.
Residuals to be allocated on each side:
RB = max(0, Q* − QB_full) for bids at p*
RA = max(0, Q* − QA_full) for asks at p*
We must have RB = RA.

8. Pro rata allocation at the marginal price (p*).
Let SB be total bid size at p*, and SA be total ask size at p*.
Distribute proportionally:
alloc_i = RB · q_i / SB for each bid at p*
alloc_j = RA · q_j / SA for each ask at p*
Round to tick/lot size using a largest remainder method:
first take floors, then distribute remaining lots in descending order of fractional remainders (tie-break by time/ID).

9. Trade matching.
Construct two pools at p*: buyers (full + pro rata) and sellers (full + pro rata).
Match pairs in a deterministic order (e.g., arrival time or agentID).
All trades occur at price p*.

10. Order book update.
Remove fully filled orders; reduce remaining orders by executed quantities.
Record the clearing log: p*, Q*, imbalance at p*, and the execution list.

11. Invariants and validations.
Total bought volume = total sold volume = Q*.
No participant receives more than requested.
After rounding, total_buy == total_sell (if mismatch occurs, adjust via largest-remainder correction).

12. Edge cases.
– No market overlap (bestBid < bestAsk): CumBid and CumAsk do not yield positive Vol → no trades.
– All liquidity at a single price: clearing at that price with full pro rata distribution.
– Auction price limits / collars: if p* is outside allowed bounds, clamp to the permitted range and recompute.


     */
}


void getProductOrderBook(std::vector<Order>& ask, std::vector<Order>& bid) {
    ask.clear();
    bid.clear();

}


void MarketEngine::updateAgentsState() {
    for (EconomicAgent& agent : agents) {
        MarketContext mc;
        agent.tick(mc);
    }
}