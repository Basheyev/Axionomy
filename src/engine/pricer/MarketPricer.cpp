
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




bool MarketPricer::validateSchema(json& data) {

    if (!data.is_array() || data.empty())  return false;

    for (const auto& p : data) {
        if (!p.is_object()) return false;

        // required fields
        static const char* required[] = {
            "productID","name","type","unit",
            "currentPrice","basePrice","demand","supply",
            "importance","materials"
        };
        for (auto key : required)
            if (!p.contains(key)) return false;

        // basic types and values
        if (!p["productID"].is_number_integer() || p["productID"].get<int64_t>() < 0) return false;
        if (!p["name"].is_string() || p["name"].get<std::string>().empty()) return false;
        if (!p["type"].is_string()) return false;
        if (p["type"] != "Good" && p["type"] != "Service") return false;
        if (!p["unit"].is_string()) return false;
        if (p["unit"] != "Piece" && p["unit"] != "Kg" && p["unit"] != "Liter" && p["unit"] != "Hour") return false;

        auto check_nonneg = [&](const char* field) {
            return p[field].is_number() && p[field].get<double>() >= 0;
            };
        if (!check_nonneg("currentPrice") || !check_nonneg("basePrice")) return false;

        if (!p["demand"].is_number_integer() || p["demand"].get<int64_t>() < 0) return false;
        if (!p["supply"].is_number_integer() || p["supply"].get<int64_t>() < 0) return false;

        if (!p["importance"].is_number()) return false;
        double imp = p["importance"].get<double>();
        if (imp < 0 || imp > 1) return false;

        // materials
        const auto& mats = p["materials"];
        if (!mats.is_array()) return false;
        for (const auto& m : mats) {
            if (!m.is_object() || !m.contains("input") || !m.contains("quantity")) return false;
            if (!m["input"].is_number_integer() || m["input"].get<int64_t>() < 0) return false;
            if (!m["quantity"].is_number() || m["quantity"].get<double>() < 0) return false;
        }
    }
    return true;
}



bool MarketPricer::loadProduct(json& description) {

    Product product;
    
    product.productID = description.value("productID", 0);    
    product.type = description.value("type", "") == "Good" ? ProductType::Good : ProductType::Service;
    std::string unitStr = description.value("unit", "");
    product.unit = ProductUnit::Piece;
    if (unitStr == "Piece")  product.unit = ProductUnit::Piece;
    if (unitStr == "Kg")     product.unit = ProductUnit::Kg;
    if (unitStr == "Liter")  product.unit = ProductUnit::Liter;
    if (unitStr == "Hour")   product.unit = ProductUnit::Hour;
    product.currentPrice = description.value("currentPrice", 0.0);
    product.basePrice = description.value("basePrice", 0.0);
    product.demand = description.value("demand", 0ULL);
    product.supply = description.value("supply", 0ULL);
    product.importance = description.value("importance", 0.0);

    product.name = description.value("name", "");
    const auto& mats = description["materials"];
    if (!mats.is_array()) return false;
    
    for (const auto& m : mats) {
        uint64_t input = m.at("input").get<uint64_t>();
        double quantity = m.at("quantity").get<double>();
        product.materials.push_back({ input, quantity });
    }

    // TODO: add to the list
    products.push_back(product);

    return true;
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
