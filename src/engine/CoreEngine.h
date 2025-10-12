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
    
    enum class OfferingType : uint8_t { Good, Service };
    enum class OfferingUnit : uint8_t { Piece, Kg, Liter, Hour, Unknown};

    //-------------------------------------------------------------------------
    // Product data structure
    //-------------------------------------------------------------------------
    struct Product {
        uint64_t id;               // Product ID

        Money    basePrice;        // Base price based on supply chain
        Quantity demand;           // Aggregate demand quantity
        Quantity supply;           // Aggregate supply quantity        
        double   importance;       // Aggregate consumer importance
    };


    //-------------------------------------------------------------------------
    // Supply Matrix (Input/Output Bill of Materials)
    //-------------------------------------------------------------------------
    class SupplyMatrix {
    public:        
        SupplyMatrix(size_t n);

        
        size_t getSize();


    private:
        size_t N;
        std::vector<double> matrix;

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

        Money evaluatePrice(uint64_t id);

    private:

        std::vector<Product> offerings;

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