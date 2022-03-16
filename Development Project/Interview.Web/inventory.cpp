#include <iostream>
#include <vector>
#include "product.h"
#include "productDao.h"
#include "inventory.h"
#include "dataAccess.h"
#include "config.h"


inventory::inventory() {
    initConnString();
}

void inventory::initConnString() {
	CfgService &cfg = CfgService::GetInstance();

    connString = cfg.GetConnectionString();
    // fake connection string for testing on a dev machine
    connString = "DRIVER={SQL Server};SERVER=localhost,1433;DATABASE=Instances;UID=gsuing;PWD=CreightonBluejays";
}

bool inventory::connectDb() {
	bool result{ false };

	if (!dao.connect(connString)) {
		std::cerr << "failed to connect to database" << std::endl;
	} else {
		std::cout << "connected to database" << std::endl;
		result = true;
	}
	return result;
}

void inventory::disconnectDb() {
	dao.disconnect();
	std::cout << "disconnected from database" << std::endl;
}

// add new product
bool inventory::addProduct(product& prod) {
	bool result{ false };

	if (connectDb()) {
		try {
			productDao pdao;
			pdao.add(prod, dao);
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}

		disconnectDb();
	}

	return result;
}

// get all products in inventory
std::vector<product> inventory::getProducts() {
	std::vector<product> productsInInventory;

	if (connectDb()) {
		try {
			productDao pdao;
			productsInInventory = pdao.getProducts(dao);
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}

		disconnectDb();
	}

	// make some fake data

	productsInInventory.emplace_back("guitar", "musical instrument",
	                                 "http://products.com/guitar.png", "123");
    productsInInventory.emplace_back("wine", "alcohol", "http://products.com/wine.png", "124");
	productsInInventory.emplace_back("pillow", "housewares", "http://products.com/pillow.png", "125");

	return productsInInventory;
}

std::vector<product>
inventory::searchProduct(product &prod, const std::vector<int> &categories,
                         const attrMap &attributes) {
	std::vector<product> products;

	if (connectDb()) {
		try {
			productDao pdao;
			products = pdao.searchProduct(prod, categories, attributes, dao);
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}

		disconnectDb();
	}

	return products;
}

// add/remove product from inventory
bool inventory::enableProduct(product &prod, const bool enable) {
	bool result{ false };

	if (connectDb()) {
		try {
			productDao pdao;
			result = pdao.productInInventory(prod, enable, dao);
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}

		disconnectDb();
	}

	return result;
}

int inventory::getProductCount(product &prod) { 
	int count{};

	if (connectDb()) {
		try {
			productDao pdao;
			count = pdao.getProductCount(prod, dao);
		} catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}

		disconnectDb();
	}

	return count;
}

