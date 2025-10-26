
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


void MarketEngine::aggregateSupplyDemand() {

    // Clear aggregates
    orders.clear();
    for (auto& [key, value] : aggregateDemand) value = 0;
    for (auto& [key, value] : aggregateSupply) value = 0;

    // Compute demand/supply aggregates
    for (const EconomicAgent& agent : agents) {
        
        for (const Order& bid : agent.bids) 
        if (bid.quantity > 0) {
            aggregateDemand[bid.productID] += bid.quantity;
            orders.push_back(bid);
        }
        
        for (const Order& ask : agent.asks) 
        if (ask.quantity > 0) {
            aggregateSupply[ask.productID] += ask.quantity;
            orders.push_back(ask);
        }

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

    /* Algorithm:
    *
     Input and filtering.
Take orders for a single product only. Discard zero or negative quantities. Split the orders into two groups: Bid (buy) and Ask (sell).

Price grid.
Collect all unique prices from Bid ∪ Ask. These are the candidate clearing prices. It is useful to track bestBid = max(Bid.price) and bestAsk = min(Ask.price).

Cumulative volumes at each candidate price p.
CumBid(p): total demand from all bids with price ≥ p.
CumAsk(p): total supply from all asks with price ≤ p.

Executable volume at price p.
Vol(p) = min(CumBid(p), CumAsk(p)).
This is the trading volume achievable if p is selected as the clearing price.

Selecting the clearing price p*.
Find V* = maxₚ Vol(p). All p with Vol(p) = V* are candidates.
Apply deterministic tie-breakers:
– minimize imbalance |CumBid(p) − CumAsk(p)|
– choose the price closest to (bestBid + bestAsk) / 2
– if still tied: apply a market pressure rule (if demand > supply choose the higher price; if supply > demand choose the lower price; or use a fixed rule such as “lower price wins”)

Full executions “in-the-money.”
All bids with price > p* are fully executed.
All asks with price < p* are fully executed.
Sum their volumes: QB_full and QA_full.

Residual volume at p*.
Total traded volume Q* = V*.
Residuals to be allocated on each side:
RB = max(0, Q* − QB_full) for bids at p*
RA = max(0, Q* − QA_full) for asks at p*
We must have RB = RA.

Pro rata allocation at the marginal price (p*).
Let SB be total bid size at p*, and SA be total ask size at p*.
Distribute proportionally:
alloc_i = RB · q_i / SB for each bid at p*
alloc_j = RA · q_j / SA for each ask at p*
Round to tick/lot size using a largest remainder method:
first take floors, then distribute remaining lots in descending order of fractional remainders (tie-break by time/ID).

Trade matching.
Construct two pools at p*: buyers (full + pro rata) and sellers (full + pro rata).
Match pairs in a deterministic order (e.g., arrival time or agentID).
All trades occur at price p*.

Order book update.
Remove fully filled orders; reduce remaining orders by executed quantities.
Record the clearing log: p*, Q*, imbalance at p*, and the execution list.

Invariants and validations.
Total bought volume = total sold volume = Q*.
No participant receives more than requested.
After rounding, total_buy == total_sell (if mismatch occurs, adjust via largest-remainder correction).

Edge cases.
– No market overlap (bestBid < bestAsk): CumBid and CumAsk do not yield positive Vol → no trades.
– All liquidity at a single price: clearing at that price with full pro rata distribution.
– Auction price limits / collars: if p* is outside allowed bounds, clamp to the permitted range and recompute.


     */
}


void MarketEngine::updateAgentsState() {
    for (EconomicAgent& agent : agents) {
        MarketContext mc;
        agent.tick(mc);
    }
}