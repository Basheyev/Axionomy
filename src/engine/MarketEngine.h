/*=============================================================================
*
*   Market Engine 
*   
*   
*   (C) Axiom Capital 2025
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
    using ProductID = size_t;
    using AgentID = size_t;
    
    struct Item {
        ProductID productID;
        Quantity quantity;
    };
    
    using BillOfMaterials = std::vector<Item>;
    enum class OrderSide : uint16_t { Buy, Sell };
    
    //-------------------------------------------------------------------------
    // Order data structure
    //-------------------------------------------------------------------------
    struct Order : Item {
        Money price;           // Order price
        OrderSide side;        // Order side
    };

    //-------------------------------------------------------------------------
    // Contract data structure
    //-------------------------------------------------------------------------
    struct Contract : Item {
        Money price;           // Actual deal price
        AgentID buyer;         // buyer agent ID
        AgentID seller;        // seller agent ID
    };
    
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
    using ProductsAggregate = std::unordered_map<ProductID, Quantity>;


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
        bool computeEquilibriumPrice(ProductID productID, Quantity demand, Quantity supply);

    private:
        ProductsList products;
        ProductsIndex indexByID;
        void evaluateProductPrice(Product& product);
        void evaluateProductCost(Product& product);
    };

    //-------------------------------------------------------------------------
    // Market Context - information Asymmetry & Bounded Rationality mechanism
    //-------------------------------------------------------------------------
    struct MarketContext {
        
        // Market Visibility

    };


    struct SellerPerception {
        AgentID id;
        float   awareness;
        float   nps;
    };

    //-------------------------------------------------------------------------
    // Base interface of simulation entity
    //-------------------------------------------------------------------------
    enum class EconomicAgentType : uint16_t { Household, Firm };
    class EconomicAgent {
    public:
        virtual ~EconomicAgent() = default;
        virtual void tick(MarketContext&) = 0;
    protected:
        AgentID  agentID;             // Agent ID
        std::vector<Item> inventory;  // Inventory (stock)
        std::vector<Order> bids;      // Buy orders
        std::vector<Order> asks;      // Sell orders

        std::vector<SellerPerception> sellersPerception;

        Money cash{ 0 };
        Money debt{ 0 };

        friend class MarketEngine;
    };


    //-------------------------------------------------------------------------
    // Market simulation engine core
    //-------------------------------------------------------------------------
    class MarketEngine {
    public:

        MarketEngine(const std::string& productsList);

        void processTick();

    private:

        size_t tickCounter;
        ProductsPricer productsPricer;
        std::vector<EconomicAgent> agents;
        std::unordered_map<ProductID, Quantity> aggregateDemand;
        std::unordered_map<ProductID, Quantity> aggregateSupply;
        std::unordered_map<ProductID, std::vector<Order>> bidOrdersBook;
        std::unordered_map<ProductID, std::vector<Order>> askOrdersBook;
        std::vector<Contract> contracts;
        
        void aggregateSupplyDemand();
        void computeEquilibriumPrice();
        void marketClearing();
        void updateAgentsState();


    };


    //-------------------------------------------------------------------------
    // Household
    //-------------------------------------------------------------------------
    class Household : public EconomicAgent {
    public:
        void tick(MarketContext&) override;
    };

    //-------------------------------------------------------------------------
    // Firm
    //-------------------------------------------------------------------------
    class Firm : public EconomicAgent {
    public:
        void tick(MarketContext&) override;
    };


}
