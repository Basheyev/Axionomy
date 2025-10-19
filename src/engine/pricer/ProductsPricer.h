#pragma once


#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <utility>

#include "libs/json.hpp"


namespace Axionomy {

    constexpr uint64_t NOT_FOUND = 0xFFFFFFFFFFFFFFFF;

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
        Money    price;            // Market price
        Money    cost;             // Product cost based on bills of materials
        Quantity demand;           // Aggregate demand quantity
        Quantity supply;           // Aggregate supply quantity  
        double   importance;       // Aggregate consumer importance
        double   floorMargin;      // Minimal industry margin
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
        static bool productListContains(uint64_t productID, const ProductsList& productsList);
    };


    //-------------------------------------------------------------------------
    // Products Pricer
    //-------------------------------------------------------------------------
    class ProductsPricer {
    public:

        ProductsPricer(const std::string& path);
        const ProductsList& getProductsList() const;
        size_t findProductIndexByID(uint64_t productID);
        Money getProductPrice(uint64_t productID);
        bool getProductData(uint64_t productID, Product& product);
        bool setDemandAndSupply(uint64_t productID, Quantity demand, Quantity supply);        
        
        
        void tick(double deltaTime);
        
    private:
        ProductsList products;
        ProductsIndex indexByID;
        void evaluateProductPrice(Product& product);
        void evaluateProductCost(Product& product);
    };

}
