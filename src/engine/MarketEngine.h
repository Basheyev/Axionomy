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

    constexpr uint64_t NOT_FOUND = 0xFFFFFFFFFFFFFFFF;

    using Money = double;
    using Quantity = int64_t;
    using ProductID = uint32_t;
    using AgentID = uint32_t;
    using BillOfMaterials = std::vector<std::pair<ProductID, double>>;

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
    using ProductsIndex = std::unordered_map<uint64_t, size_t>;


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
        size_t getIndexByProductID(uint64_t productID) const;
        Money getProductPrice(uint64_t productID) const;
        bool setDemandAndSupply(uint64_t productID, Quantity demand, Quantity supply);

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
    class EconomicAgent {
    public:
        virtual ~EconomicAgent() = default;
        virtual void tick() = 0;
    protected:
        AgentID  agentID;             // Agent ID
        uint32_t rngSeed;             // 

        // Buy (demand input) household
        // Sell (supply output)

    };



    //-------------------------------------------------------------------------
    // National Bank simulator entity
    //-------------------------------------------------------------------------
    class NationalBank : public EconomicAgent {
    public:
        void tick() override;
        double getBaseRate();
    private:

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



    private:



    };


}