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
        uint32_t id;               // Offering Item ID
        uint32_t bitFlags;         // Item flags (reserved)
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

        MarketPricer(uint32_t size);

        uint32_t addItem(Money basePrice, Quantity demand, Quantity supply, double importance);

        void setBasePrice(uint32_t id, Money basePrice);
        void setDemand(uint32_t id, Quantity amount);
        void setSupply(uint32_t id, Quantity amount);
        void setImportance(uint32_t id, double amount);

        Money getBasePrice(uint32_t id);
        Quantity getDemand(uint32_t id);
        Quantity getSupply(uint32_t id);
        double getImportance(uint32_t id);

        Money getPrice(uint32_t id);

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