

#include "CoreEngine.h"

using namespace Axionomy;


void Household::update(double deltaTime) {
        
    
    Money cashFlow = income - spendings;                    // Evaluate current cashflow
    sentiment = double(cashFlow) - double(lastCashFlow);    // Evaluate sentiment as comparison to previous period

    if (cashFlow >= 0) {
        savings += cashFlow;
    } else {
        if (savings > cashFlow) {
            savings -= cashFlow;
        } else {
            if (assets > cashFlow) {
                assets -= cashFlow;
            } else {
                // default
            }
        }
    }


    lastCashFlow = cashFlow;

}

