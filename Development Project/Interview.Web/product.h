#pragma once

#include <string>
#include <vector>
#include <vector>
#include "dataAccess.h"
#include "json.hpp"

class productDao;

using nlohmann::json;
using prodAttr = std::pair<std::string, std::string>; // a single product attribute
using attrMap = std::map<std::string, std::string>; // map attribute name to value

class product {
public:
	product() = delete;
	product(const std::string _name, const std::string _description, const std::string _imageUri, const std::string _sku);
	product &operator=(const product &) = delete;

	~product() = default;

	void setInstanceId(const int id);
	void setCategories(const std::vector<int>& _categories);
	void setAttributes(const attrMap &_attributes);

	product getProduct(const int instanceId);
	std::vector<product> getProducts();

	json toJson() const;	

private:
	friend class productDao;

	int instanceId {};
    bool inInventory {};
	std::string name;
	std::string description;
	std::string imageUri;
	std::string sku;
	std::vector<int> categories;
	attrMap attributes;
};