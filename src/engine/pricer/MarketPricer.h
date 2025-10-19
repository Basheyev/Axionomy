#pragma once


#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include "libs/json.hpp"


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
    // Products list loader
    //-------------------------------------------------------------------------
    class ProductsLoader {
    public:
        static size_t loadProductList(const std::string& path, std::vector<Product>& productsList);
    private:
        
        ProductsLoader() = delete;              
        ~ProductsLoader() = delete;             
        ProductsLoader(const ProductsLoader&) = delete;
        ProductsLoader& operator=(const ProductsLoader&) = delete;

        static bool validateSchema(json& productList);
        static bool loadProduct(json& productData, std::vector<Product>& productsList);
        static bool productListContains(uint64_t productID, std::vector<Product>& productsList);
    };


    //-------------------------------------------------------------------------
    // Products Pricer
    //-------------------------------------------------------------------------
    class ProductsPricer {
    public:

        ProductsPricer(const std::string& path);

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

    };

}
