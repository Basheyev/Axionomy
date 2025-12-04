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
        AgentID agent;         // Order agent
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
    // Base interface of simulation entity
    //-------------------------------------------------------------------------
    enum class EconomicAgentType : uint16_t { Household, Firm };
   
    class EconomicAgent {    
    public:
        virtual ~EconomicAgent() = default;
        virtual void tick() = 0;
    protected:
        AgentID  agentID;              // Agent ID
        std::vector<Item> inventory;   // Inventory (stock)

        // FIXME: I think that there is no reason store store orders here
        // maybe its better to directly submit orders to the market engine
        std::vector<Order> buyOrders;  // Buy orders
        std::vector<Order> sellOrders; // Sell orders

        Money cash{ 0 };
        Money debt{ 0 };

        friend class MarketEngine;
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
    // Market simulation engine core
    //-------------------------------------------------------------------------
    class MarketEngine {
    public:

        MarketEngine(const std::string& productsList);

        void processTick();
            
        void submitOrder(AgentID agent, Quantity qty, Money limitPrice, OrderSide side);
        void executeTrade(AgentID buyer, AgentID seller, Quantity qty, Money tradePrice);
            

    private:

        size_t tickCounter;
        ProductsPricer productsPricer;
        std::vector<EconomicAgent> agents;
        std::unordered_map<ProductID, Quantity> aggregateDemand;
        std::unordered_map<ProductID, Quantity> aggregateSupply;

        // TODO: I thinks its better to separate buy / sell orders and calculate aggregates in submitOrder method
        std::unordered_map<ProductID, std::vector<Order>> ordersBook;
                
        void aggregateSupplyDemand();
        void computeEquilibriumPrice();
        void processMarketClearing();
        void processProductClearing(const ProductID productID);
        void updateAgentsState();


    };




}
