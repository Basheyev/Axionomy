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
#include <iostream>

#include "libs/json.hpp"

namespace Axionomy {

    using json = nlohmann::json;

    constexpr size_t NOT_FOUND = 0xFFFFFFFFFFFFFFFF;

    using Money = double;
    using Quantity = double;
    using ProductID = uint32_t;
    using AgentID = uint32_t;
    struct Item {
        ProductID productID;
        Quantity quantity;
    };
    using BillOfMaterials = std::vector<Item>;
    enum class ProductType : uint16_t { Good, Service };
    enum class ProductUnit : uint16_t { Piece, Kg, Liter, Hour };

    //-------------------------------------------------------------------------
    // Product data structure
    //-------------------------------------------------------------------------    
    struct Product {
        ProductID productID;       // Product ID
        ProductType type;          // Good or Service
        ProductUnit unit;          // Measurement unit
        Money    price;            // Market price
        Money    cost;             // Product cost based on bills of materials
        Quantity demand;           // Aggregate demand quantity
        Quantity supply;           // Aggregate supply quantity  
        double   importance;       // Aggregate consumer importance
        double   floorMargin;      // Minimal industry margin
        double   turnover;         // Average turnover duration in days
        std::string name;          // Product name
        BillOfMaterials materials; // Bill of materials
    };

    using ProductsList = std::vector<Product>;
    using ProductsIndex = std::unordered_map<ProductID, size_t>;


    //-------------------------------------------------------------------------
    // Products list loader
    //-------------------------------------------------------------------------
    class ProductsLoader {
    public:
        static size_t loadProductList(const std::string& path, ProductsList& productsList);
    private:

        ProductsLoader() = delete;
        ~ProductsLoader() = delete;
        ProductsLoader(const ProductsLoader&) = delete;
        ProductsLoader& operator=(const ProductsLoader&) = delete;

        static bool validateSchema(json& productList);
        static bool loadProduct(json& productData, ProductsList& productsList);
        static bool productListContains(ProductID productID, const ProductsList& productsList);
    };


    //-------------------------------------------------------------------------
    // Products Pricer
    //-------------------------------------------------------------------------
    class ProductsPricer {
    public:

        ProductsPricer(const std::string& path);
        const ProductsList& getProductsList() const;
        size_t getIndexByProductID(ProductID productID) const;
        Money getProductPrice(ProductID productID) const;
        bool setDemandAndSupply(ProductID productID, Quantity demand, Quantity supply);

        void tick();

    private:
        ProductsList products;
        ProductsIndex indexByID;
        void evaluateProductPrice(Product& product);
        void evaluateProductCost(Product& product);
    };


    //-------------------------------------------------------------------------
    // Base interface of simulation entity
    //-------------------------------------------------------------------------
    enum class EconomicAgentType : uint16_t { Household, Firm, NationalBank };
    class EconomicAgent {
    public:
        virtual ~EconomicAgent() = default;
        virtual void tick() = 0;
    protected:
        AgentID  agentID;             // Agent ID
        std::vector<Item> inputs;     // Buy (demand input)
        std::vector<Item> inventory;  // Inventory (stock)
        std::vector<Item> outputs;    // Sell (supply output)
        
        Money cash;
        Money debt;

    };



    //-------------------------------------------------------------------------
    // National Bank simulator entity
    //-------------------------------------------------------------------------
    class NationalBank : public EconomicAgent {
    public:
        void tick() override;
        double getBaseRate();
    private:
        double baseRate;
    };


    //-------------------------------------------------------------------------
    // Household
    //-------------------------------------------------------------------------
    class Household : public EconomicAgent {
    public:
        void tick() override;
    };

    //-------------------------------------------------------------------------
    // Firm
    //-------------------------------------------------------------------------
    class Firm : public EconomicAgent {
    public:
        void tick() override;
    };


    //-------------------------------------------------------------------------
    // Simulation engine core
    //-------------------------------------------------------------------------
    class MarketEngine {
    public:

    // for each tick (day)
       // AggregateSupplyDemand()
       // ComputeEquilibriumPrice()
       // clearMarketProRata()
       // updateAgentsState()
    private:



    };


}
