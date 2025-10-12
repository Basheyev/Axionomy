/*=============================================================================
*
*   Core Engine 
*   
*   
*   (Ñ) Axiom Capital 2025
* 
*=============================================================================*/

#pragma once

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>

namespace Axionomy {


    using Money = double;
    using Quantity = int64_t;


    //-------------------------------------------------------------------------
    // Offering Item data structure
    //-------------------------------------------------------------------------
    struct Item {
        uint64_t id;               // Offering Item ID        
        Money    basePrice;        // Base price based on supply chain
        Quantity demand;           // Aggregate demand quantity
        Quantity supply;           // Aggregate supply quantity        
        double   importance;       // Aggregate market importance
    };


    //-------------------------------------------------------------------------
    // Market Pricer
    //-------------------------------------------------------------------------
    class MarketPricer {
    public:

        MarketPricer(uint64_t size);

        uint64_t addItem(Money basePrice, Quantity demand, Quantity supply, double importance);

        void setBasePrice(uint64_t id, Money basePrice);
        void setDemand(uint64_t id, Quantity amount);
        void setSupply(uint64_t id, Quantity amount);
        void setImportance(uint64_t id, double amount);

        Money getBasePrice(uint64_t id);
        Quantity getDemand(uint64_t id);
        Quantity getSupply(uint64_t id);
        double getImportance(uint64_t id);

        Money getPrice(uint64_t id);

    private:

        std::vector<Item> offerings;

    };



    //-------------------------------------------------------------------------
    // Base interface of simulation entity
    //-------------------------------------------------------------------------
    class EconomicAgent {
    public:
        virtual ~EconomicAgent() = default;
        virtual void update(double deltaTime) = 0;
    };


    //-------------------------------------------------------------------------
    // National Bank simulator entity
    //-------------------------------------------------------------------------
    class NationalBank : public EconomicAgent {
    public:
        void update(double deltaTime) override;
        double getBaseRate();
    private:
    };


    //-------------------------------------------------------------------------
    // Household
    //-------------------------------------------------------------------------
    class Household : public EconomicAgent {
    public:
        void update(double deltaTime) override;

    protected:

        // Finance
        Money income;
        Money spendings;
        Money savings;
        Money debt;        
        Money assets;
        Money lastCashFlow;

        // People
        Quantity adults;
        Quantity children;

        // Behavior
        double sentiment;

    };


    //-------------------------------------------------------------------------
    // Simulation engine core
    //-------------------------------------------------------------------------
    class CoreEngine {
    public:



    private:



    };


}