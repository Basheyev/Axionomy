
#include "../CoreEngine.h"

#include <iostream>
#include <utility>

using namespace Axionomy;


MarketPricer::MarketPricer(const std::string& path) {
    size_t productsCount = loadProductList(path);
}


/**
*  @brief Loads product list from JSON file
*  @param path relative path
*  @return total products count or zero if failed
*/
size_t MarketPricer::loadProductList(const std::string& path) {
    std::ifstream productListFile(path);                            // Open the file
    if (!productListFile.is_open()) return 0;                       // if file is can not be opened, return zero products    
    size_t count = 0;                                               // set initial products count to zero    
    try {        
        json productList;             
        productListFile >> productList;                             // read file to JSON        
        if (!validateSchema(productList)) return 0;                 // validate schema
        count = productList.size();                                 // if it's an array, get its size        
        for (size_t i = 0; i < count; i++) {                        // iterate over the array
            json productJSON = productList.at(i);                   // get each product object
            if (!loadProduct(productJSON)) {                        // load product description or return zero on failure                
                products.clear();                                   // clear products vector
                return 0;                
            }
        }
    } catch (const json::parse_error& e) {
        std::cerr << "Parse error at byte " << e.byte << ": " << e.what() << '\n';
        count = 0;
    } catch (const std::exception& e) {
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
bool MarketPricer::validateSchema(json& data) {

    // Check if it is an array containing at least one product
    if (!data.is_array() || data.empty())  return false;

    // Iterate over products list
    for (const auto& productJSON : data) {

        // Check if element is an object
        if (!productJSON.is_object()) return false;

        // Check required fields
        static const char* required[] = {
            "productID","name","type","unit", 
            "currentPrice","basePrice","demand","supply", 
            "importance","materials"};
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
        if (!productJSON["currentPrice"].is_number() || productJSON["currentPrice"].get<double>() >= 0) return false;
        if (!productJSON["basePrice"].is_number() || productJSON["basePrice"].get<double>() >= 0) return false;

        // Check if the "demand" & "supply" is integers and a non-negative integers
        if (!productJSON["demand"].is_number_integer() || productJSON["demand"].get<int64_t>() < 0) return false;
        if (!productJSON["supply"].is_number_integer() || productJSON["supply"].get<int64_t>() < 0) return false;

        // Check if the "importance" is number and a non-negative number [0:1]
        if (!productJSON["importance"].is_number()) return false;
        double importance = productJSON["importance"].get<double>();
        if (importance < 0 || importance > 1) return false;

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
*  @return true if the schema is valid, false otherwise
*/
bool MarketPricer::loadProduct(json& productData) {

    // Check for duplicates
    uint64_t id = productData.value("productID", 0);;
    if (productListContains(id)) return false;

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
    product.currentPrice = productData.value("currentPrice", 0.0);
    product.basePrice = productData.value("basePrice", 0.0);
    product.demand = productData.value("demand", 0ULL);
    product.supply = productData.value("supply", 0ULL);
    product.importance = productData.value("importance", 0.0);
    product.name = productData.value("name", "");
    const auto& materialsList = productData["materials"];
        
    for (const auto& material : materialsList) {
        uint64_t input = material.at("input").get<uint64_t>();
        double quantity = material.at("quantity").get<double>();
        // check if input exist in the product list
        if (!productListContains(input)) return false;
        product.materials.push_back({ input, quantity });
    }

    // if everything is ok then add product object to the list
    products.push_back(product);

    return true;
}


bool MarketPricer::productListContains(uint64_t productID) {
    for (Product& product : products) {
        if (product.productID == productID) return true;
    }
    return false;
}


size_t MarketPricer::getProductID(const std::string& productName) {
    for (size_t i = 0; i < products.size(); i++) {
        std::string& name = products[i].name;
        if (name == productName) return i;
    }
    return 0;
}

size_t MarketPricer::getProductsCount() {
    return products.size();
}

uint64_t MarketPricer::addProduct(Product product) {
    uint64_t index = products.size();
    product.productID = index;
    products.push_back(product);
    return index;
}


void MarketPricer::setBasePrice(uint64_t id, Money basePrice) {
    products[id].basePrice = basePrice;
}

void MarketPricer::setDemand(uint64_t id, Quantity amount) {
    products[id].demand = amount;
}

void MarketPricer::setSupply(uint64_t id, Quantity amount) {
    products[id].supply = amount;
}


// importance (0,1]
void MarketPricer::setImportance(uint64_t id, double importance) {
    products[id].importance = std::clamp(importance, 1e-6, 1.0);
}


Money MarketPricer::getBasePrice(uint64_t id) {
    return products[id].basePrice;
}

Quantity MarketPricer::getDemand(uint64_t id) {
    return products[id].demand;
}

Quantity MarketPricer::getSupply(uint64_t id) {
    return products[id].supply;
}

double MarketPricer::getImportance(uint64_t id) {
    return products[id].importance;
}

Money MarketPricer::evaluatePrice(uint64_t id) {
    // fetch values and convert to double
    double demand = double(products[id].demand);
    double supply = double(products[id].supply);
    double basePrice = products[id].basePrice;

    // disbalance sensitivity (maximum 1% deficit adds 10% to price)
    constexpr double maxElasticity = 12.305019857643899;
    double k = products[id].importance * maxElasticity;

    // sigmoid asymmetry parameters
    constexpr double minY = 0.4; // minimal price multiplier
    constexpr double maxY = 4.0; // maximum price multiplier
    constexpr double diff = maxY - minY;
    static const double v = std::log2((maxY - minY) / (1 - minY));
    
    // Division-by-zero protection (bias)
    constexpr double bias = 0.001;

    // measure disbalance and ratio
    double balanceAmount = demand - supply;
    double balanceRatio = balanceAmount / (supply + bias);
   
    // evaluate asymmetric sigmoid [min, max)    
    double e = std::exp(-balanceRatio * k);    
    double sigmoid = minY + (diff / std::pow(1 + e, v));
   
    // evaluate price
    Money targetPrice = basePrice * std::clamp(sigmoid, minY, maxY);
    
    // TODO: add inertia
    products[id].currentPrice = targetPrice;

    return targetPrice;
}
