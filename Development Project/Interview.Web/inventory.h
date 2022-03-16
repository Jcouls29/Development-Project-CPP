#pragma once

#include <string>
#include <vector>
#include "dataAccess.h"
#include "product.h"

class inventory {
public:
	inventory();

	bool addProduct(product& prod);
	std::vector<product> getProducts(); // return all products
	std::vector<product> searchProduct(
		product& prod, const std::vector<int>& categories, const attrMap& attributes); // search products
	bool enableProduct(product& prod, const bool enable); // add/remove product from inventory
	int getProductCount(product& prod);

private:
	std::string connString;
	dataAccess dao;

	void initConnString();
	bool connectDb();
	void disconnectDb();
};