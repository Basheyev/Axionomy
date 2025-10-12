
#include "../CoreEngine.h"

using namespace Axionomy;



SupplyMatrix::SupplyMatrix(size_t n) : N(n), matrix(n*n, 0.0) {

}


size_t SupplyMatrix::getSize() { 
    return matrix.size() / N; 
}