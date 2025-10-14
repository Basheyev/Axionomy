/*=============================================================================
*
*   Core Engine 
*   
*   
*   (Ñ) Axiom Capital 2025
* 
*=============================================================================*/

#pragma once

#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include "../libs/json.hpp"


namespace Axionomy {

    using json = nlohmann::json;

    using Money = double;
    using Quantity = int64_t;
    using BillOfMaterials = std::vector<std::pair<uint64_t, double>>;
    
    enum class ProductType : uint32_t { Good, Service };
    enum class ProductUnit : uint32_t { Piece, Kg, Liter, Hour };

    //-------------------------------------------------------------------------
    // Product data structure
    //-------------------------------------------------------------------------
    struct Product {
        uint64_t productID;        // Product ID
        ProductType type;          // Good or Service
        ProductUnit unit;          // Measurement unit
        Money    currentPrice;     // Current price
        Money    basePrice;        // Base price based on supply matrix
        Quantity demand;           // Aggregate demand quantity
        Quantity supply;           // Aggregate supply quantity  
        double   importance;       // Aggregate consumer importance
        std::string name;          // Product name
        BillOfMaterials materials; // Bill of materials
    };


    //-------------------------------------------------------------------------
    // Market Pricer
    //-------------------------------------------------------------------------
    class MarketPricer {
    public:

        MarketPricer(const std::string& path);

        size_t getProductsCount();


        size_t getProductID(const std::string& name);

        uint64_t addProduct(Product product);

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
                
        std::vector<Product> products;
        
        size_t loadProductList(const std::string& path);
        bool validateSchema(json& productList);
        bool loadProduct(json& productDescription);

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