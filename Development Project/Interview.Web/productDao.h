#pragma once
#include <vector>
#include "product.h"
#include "recordSet.h"

class productDao {
public:
	bool add(product &prod, dataAccess& dao); // add single product
	std::vector<product> getProducts(dataAccess& dao); // get all products in inventory
	std::vector<product> searchProduct(product &prod, const std::vector<int> &categories, // search for products
	              const attrMap &attributes, dataAccess &dao);

	int getProductCount(product &prod, dataAccess& dao); // get inventory count of a product
	bool productInInventory(product& prod, const bool inInventory, dataAccess& dao); // add/remove from inventory
	product productFromRow(const int row, recordSet& rs);

private:
	bool addAttribute(product &prod, prodAttr &attr, dataAccess &dao);
	bool setAttribute(product &prod, prodAttr &attr, dataAccess &dao);
};
