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
#include "engine/pricer/ProductsPricer.h"


namespace Axionomy {

   
    //-------------------------------------------------------------------------
    // Base interface of simulation entity
    //-------------------------------------------------------------------------
    class EconomicAgent {
    public:
        virtual ~EconomicAgent() = default;
        virtual void update(double deltaTime) = 0;
    };


    //-------------------------------------------------------------------------
    // National Bank simulator entity
    //-------------------------------------------------------------------------
    class NationalBank : public EconomicAgent {
    public:
        void update(double deltaTime) override;
        double getBaseRate();
    private:
    };


    //-------------------------------------------------------------------------
    // Household
    //-------------------------------------------------------------------------
    class Household : public EconomicAgent {
    public:
        void update(double deltaTime) override;

    protected:

        // Finance
        Money income;
        Money spendings;
        Money savings;
        Money debt;        
        Money assets;
        Money lastCashFlow;

        // People
        Quantity adults;
        Quantity children;

        // Behavior
        double sentiment;

    };


    //-------------------------------------------------------------------------
    // Simulation engine core
    //-------------------------------------------------------------------------
    class CoreEngine {
    public:



    private:



    };


}