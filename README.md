# Axionomy

**Axionomy** is an educational startup simulator developed by **X Academy** — a web-based game designed to teach the fundamentals of business and startup management through interactive market and agent-based simulation.

The project combines a **high-performance C++ simulation server** with a **JavaScript browser client**, creating a responsive, data-driven world where markets, firms, and households evolve dynamically over time.

---

## Vision

The simulator models a realistic startup economy where the player builds and manages a company in a living market environment.  
The goal is to teach the logic of business formation, supply chains, pricing, and financial management through experimentation rather than theory.

---

## Simulation Layers

### 1. Product Pricer
A dynamic market engine that:
- Maintains a catalog of goods and services  
- Simulates price formation based on demand, supply, and basePrice  
- Computes basePrice recursively from each product’s Bill of Materials (BoM) and labor costs  
- Evolves prices over time through market feedback loops  

### 2. Households (Agents)
An agent-based population of 50–100K households differentiated by:
- Social status (low, mid, high)  
- Age and education structure  
- Income, assets, and liabilities  
- Consumption baskets linked to the catalog  
Households form demand curves for all market offerings based on preferences, prices, and budget constraints.

### 3. Firms (Agents)
Autonomous company agents that:
- Search for market arbitrage opportunities  
- Build supply chains with other firms and households  
- Manage production, wages, and dividends  
- Adapt strategies based on profitability and liquidity  
Each firm dynamically allocates labor, capital, and pricing decisions to maximize long-term value.

### 4. Financial System
An aggregated financial layer that includes:
- National Bank controlling base rate and liquidity  
- Commercial Banks issuing loans to firms and households  
- Credit markets and default propagation dynamics  

### 5. Player Company (Core Gameplay)
The player acts as a founder managing their own firm:
- Choose a product or service to produce  
- Hire staff, design processes, and select suppliers  
- Manage technology, production chains, and finances  
- Compete in an evolving market with other agents  

---

## Technical Overview

| Component | Technology | Description |
|------------|-------------|-------------|
| Server | C++20+ | Core simulation engine, tick-based world loop, REST + WebSocket interface |
| Client | JavaScript / WebGL | Browser-based UI and visualization |
| Data | JSON / FlatBuffers | Catalogs, agents, and simulation states |
| Scale Target | 50K–100K agents | Multi-threaded simulation on commodity hardware |

---

## Purpose

Axionomy is built to teach **startup thinking and systems reasoning**.  
Players experience firsthand how pricing, finance, and operations interact — learning by managing uncertainty, not memorizing formulas.

---

## Status

- [x] Architecture design  
- [x] Simulation model defined  
- [ ] Core engine implementation (C++)  
- [ ] Web client prototype  
- [ ] Educational scenarios and tutorials  

---

## License

TBD — internal educational prototype (X Academy)

