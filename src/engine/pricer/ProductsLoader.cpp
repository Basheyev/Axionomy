/**
 * =============================================================================
 * 
 * @class ProductsLoader
 * @brief Loads and validates a list of products from a JSON file.
 *
 * The ProductsLoader class is responsible for reading product data from
 * a JSON file, validating its schema, and populating a list of Product
 * structures. It ensures that each product entry contains all required
 * fields, correct data types, and valid values.
 *
 * Main responsibilities:
 *  - Parse JSON file containing products.
 *  - Validate data schema and field consistency.
 *  - Detect duplicates and missing dependencies.
 *  - Convert JSON objects into Product structures.
 *
 * Returns the total number of successfully loaded products or zero
 * if validation or parsing fails.
 *
 * (C) Axionomy, Bolat Basheyev 2025
 * 
 * ============================================================================= */
#include "engine/CoreEngine.h"

#include <filesystem>
#include <fstream>

using namespace Axionomy;


/**
*  @brief Loads product list from JSON file
*  @param path relative path
*  @param products output vector of products
*  @return total products count or zero if failed
*/
size_t ProductsLoader::loadProductList(const std::string& path, ProductsList& products) {
    std::ifstream productListFile(path);                            // Open the file
    if (!productListFile.is_open()) return 0;                       // if file is can not be opened, return zero products    
    size_t count = 0;                                               // set initial products count to zero    
    try {
        json productList;
        productListFile >> productList;                             // read file to JSON        
        if (!validateSchema(productList)) {                         // validate schema
            std::cerr << "Invalid schema: " << path << '\n';
            return 0;                 
        }
        count = productList.size();                                 // if it's an array, get its size 
        products.reserve(count);                                    // Allocate required memory only once
        for (size_t i = 0; i < count; i++) {                        // iterate over the array
            json productJSON = productList.at(i);                   // get each product object
            if (!loadProduct(productJSON, products)) {              // load product description or return zero on failure                
                products.clear();                                   // clear products vector
                return 0;
            }
        }
    }
    catch (const json::parse_error& e) {
        std::cerr << "Parse error at byte " << e.byte << ": " << e.what() << '\n';
        count = 0;
    }
    catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << '\n';
        count = 0;
    }
    return count;
}



/**
*  @brief Validates the JSON schema of products file
*  @param data JSON object containing product list
*  @return true if the schema is valid, false otherwise
*/
bool ProductsLoader::validateSchema(json& data) {

    // Check if it is an array containing at least one product
    if (!data.is_array() || data.empty())  return false;

    // Iterate over products list
    for (const auto& productJSON : data) {

        // Check if element is an object
        if (!productJSON.is_object()) return false;

        // Check required fields
        static const char* required[] = {
            "productID","name","type","unit",
            "price","cost","demand","supply",
            "importance", "floorMargin", "turnover", 
            "materials"};
        for (auto key : required)
            if (!productJSON.contains(key)) return false;

        // Check basic types of fields and values        
        if (!productJSON["productID"].is_number_integer() || productJSON["productID"].get<int64_t>() < 0) return false;
        if (!productJSON["name"].is_string() || productJSON["name"].get<std::string>().empty()) return false;
        if (!productJSON["type"].is_string()) return false;
        if (productJSON["type"] != "Good" && productJSON["type"] != "Service") return false;
        if (!productJSON["unit"].is_string()) return false;
        if (productJSON["unit"] != "Piece" && productJSON["unit"] != "Kg"
            && productJSON["unit"] != "Liter" && productJSON["unit"] != "Hour") return false;

        // Check if the "currentPrice" & "basePrice" fields are numbers and a non-negative numbers
        if (!productJSON["price"].is_number() || productJSON["price"].get<double>() < 0) return false;
        if (!productJSON["cost"].is_number() || productJSON["cost"].get<double>() < 0) return false;

        // Check if the "demand" & "supply" is integers and a non-negative integers
        if (!productJSON["demand"].is_number_integer() || productJSON["demand"].get<int64_t>() < 0) return false;
        if (!productJSON["supply"].is_number_integer() || productJSON["supply"].get<int64_t>() < 0) return false;

        // Check if the "importance" is number and a non-negative number [0:1]
        if (!productJSON["importance"].is_number()) return false;
        double importance = productJSON["importance"].get<double>();
        if (importance < 0 || importance > 1) return false;

        // Check if the "floorMargin" is number and a non-negative number [0:1]
        if (!productJSON["floorMargin"].is_number()) return false;
        double floorMargin = productJSON["floorMargin"].get<double>();
        if (floorMargin < 0 || floorMargin > 1) return false;

        // Check if the "turnover" is number and a non-negative number [0:365]
        if (!productJSON["turnover"].is_number()) return false;
        double turnover = productJSON["turnover"].get<double>();
        if (turnover < 0) return false;

        // Check if the "materials" array exists and each entry contains 
        // a non-negative integer "input" field and double "quantity" field
        const auto& materials = productJSON["materials"];
        if (!materials.is_array()) return false;
        for (const auto& m : materials) {
            if (!m.is_object() || !m.contains("input") || !m.contains("quantity")) return false;
            if (!m["input"].is_number_integer() || m["input"].get<int64_t>() < 0) return false;
            if (!m["quantity"].is_number() || m["quantity"].get<double>() < 0) return false;
        }
    }
    return true;
}


/**
*  @brief Loads the product JSON to the products list
*  @param productData JSON object containing product
*  @param products output vector of products
*  @return true if the schema is valid, false otherwise
*/
bool ProductsLoader::loadProduct(json& productData, ProductsList& products) {

    // Check for duplicates
    uint64_t id = productData.value("productID", 0);;
    if (productListContains(id, products)) return false;

    // Fetch fields from JSON to Product object
    Product product;
    product.productID = id;
    product.type = productData.value("type", "") == "Good" ? ProductType::Good : ProductType::Service;
    std::string unitStr = productData.value("unit", "");
    product.unit = ProductUnit::Piece;
    if (unitStr == "Piece")  product.unit = ProductUnit::Piece;
    if (unitStr == "Kg")     product.unit = ProductUnit::Kg;
    if (unitStr == "Liter")  product.unit = ProductUnit::Liter;
    if (unitStr == "Hour")   product.unit = ProductUnit::Hour;
    product.price = productData.value("price", 0.0);
    product.cost = productData.value("cost", 0.0);
    product.demand = productData.value("demand", 0ULL);
    product.supply = productData.value("supply", 0ULL);
    product.importance = productData.value("importance", 0.0);
    product.floorMargin = productData.value("floorMargin", 0.0);
    product.turnover = productData.value("turnover", 0.0);
    product.name = productData.value("name", "");
    const auto& materialsList = productData["materials"];

    product.materials.reserve(materialsList.size());

    for (const auto& material : materialsList) {
        uint64_t input = material.at("input").get<uint64_t>();
        double quantity = material.at("quantity").get<double>();
        // check if input exist in the product list
        if (!productListContains(input, products)) return false;
        product.materials.push_back({ input, quantity });
    }

    // if everything is ok then add product object to the list
    products.push_back(product);

    return true;
}


/**
*  @brief Checks if product list already contains product ID
*  @param productID ID of product
*  @param products output vector of products
*  @return true if the schema is valid, false otherwise
*/
bool ProductsLoader::productListContains(uint64_t productID, const ProductsList& products) {
    for (const Product& product : products) {
        if (product.productID == productID) return true;
    }
    return false;
}
