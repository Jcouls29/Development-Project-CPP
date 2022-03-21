#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include "fmt\format.h"
#include "productDao.h"
#include "recordSet.h"

using namespace std::literals::string_literals;

const auto whereStr = std::string{" WHERE "};
const auto whereLen = whereStr.length();

// add a new product, dao is connected to the database at entry
bool productDao::add(product &prod, dataAccess &dao) { 
	bool result = false;

	// build the query string, hacky due to time constraints
	std::string query =
	    fmt::format(
			R"(INSERT INTO[dbo].[Products]([Name], [Description], [ProductImageUris], [ValidSkus])
				OUTPUT Inserted.InstanceId
				VALUES('{}', '{}', '{}', '{}'))",
				prod.name, prod.description, prod.imageUri, prod.sku);

	recordSet results;
	dao.execQuery(query, &results);
	prod.setInstanceId(std::stoi(results.getData(0, "InstanceID")));

	// TODO: insert associated categories and attributes

	prodAttr inv{"inInventory", "No"};
	addAttribute(prod, inv, dao);
	prodAttr count{"count", "0"};
	addAttribute(prod, count, dao);

	return result;
}

// get all products in inventory, dao is connected to the database at entry
std::vector<product> productDao::getProducts(dataAccess &dao) {
	std::vector<product> products;

	// build the query string, hacky due to time constraints
	std::string query = 
		R"(SELECT p.[InstanceId], p.[Name], p.[Description], p.[ProductImageUris], p.[ValidSkus]
			  FROM [dbo].[Products] AS p
			  JOIN [dbo].[ProductAttributes] AS pa
			  ON p.InstanceId=pa.InstanceID
			  WHERE pa.[Key]='inInventory' and pa.[Value]='Yes')";

	recordSet results;
	dao.execQuery(query, &results);

	productDao pdao;

	for (int i = 0; i < static_cast<int>(results.getCount()); ++i) {
		products.emplace_back(pdao.productFromRow(i, results));
	}

	return products;
}

// search for products meeting criteria, dao is connected to the database at entry
std::vector<product>
productDao::searchProduct(product &prod, const std::vector<int> &categories,
                          const attrMap &attributes, dataAccess &dao) {
	std::vector<product> products;

	std::string query =
	    R"(SELECT DISTINCT p.[InstanceId],p.[Name], p.[Description], p.[ProductImageUris], p.[ValidSkus]
			FROM [dbo].[Products] as p
			JOIN [dbo].[ProductCategories] AS pc ON p.instanceid = pc.instanceid
			JOIN [dbo].[Categories] AS c ON pc.categoryinstanceID = c.instanceid
			JOIN [dbo].[ProductAttributes] AS pa ON p.instanceID = pa.InstanceId
			AND pa.[Key] = 'inInventory' AND pa.[Value]='Yes')";

	std::stringstream where;
	where << whereStr;

	// set product search criteria for non-empty product fields
	if (prod.instanceId != 0) {
		where << "p.[InstanceId] = " << prod.instanceId;
	}
	if (prod.name.length() > 0) {
		where << (where.str().length() > whereLen) ? L" AND " : L"";
		where << "p.[Name] = " << prod.name;
	}
	if (prod.description.length() > 0) {
		where << (where.str().length() > whereLen) ? L" AND " : L"";
		where << "p.[Description] = " << prod.description;
	}
	if (prod.sku.length() > 0) {
		where << (where.str().length() > whereLen) ? L" AND " : L"";
		where << L"p.[ValidSkus] = " << prod.sku;
	}
	// add any category IDs to the query
	if (!categories.empty()) {
		where << (where.str().length() > whereLen) ? " AND pc.[Categories].categoryinstanceId IN (" : "";
		for (auto id : categories) {
			where << id;
			if (id == categories.back()) {
				where << ")";
			} else {
				where << ",";
			}
		}
	}
	// add any attributes to the query
	// TODO: implement attribute clause

	if (where.str().length() > whereLen) {
		query += where.str();
	}

	recordSet results;
	dao.execQuery(query, &results);

	productDao pdao;

	for (int i = 0; i < static_cast<int>(results.getCount()); ++i) {
		products.emplace_back(pdao.productFromRow(i, results));
	}

	return products;
}

// get inventory count for a product, dao is connected to the database at entry
int productDao::getProductCount(product &prod, dataAccess &dao) {
	int count {};

	std::string query = fmt::format(
		R"(SELECT [Value] FROM [dbo].[ProductAttributes] WHERE InstanceID={} AND [key]='count')",
		prod.instanceId);

	recordSet results;
	dao.execQuery(query, &results);

	if (results.getCount() > 0) {
		count = std::stoi(results.getData(0, "Value"));
	}

	return count;
}

// make a product active/inactive in inventory, dao is connected to the database
// at entry
bool productDao::productInInventory(product &prod, const bool inInventory,
                                    dataAccess &dao) {
	bool result { false };

	std::string query = fmt::format(
		R"(UPDATE [dbo].[ProductAttributes] SET [Value] = '{}' WHERE InstanceId={} AND [key]='inInventory')",
		(inInventory ? "Yes" : "No"), prod.instanceId);

	result = dao.execQuery(query, std::nullopt);
	
	return result;
}

// add product attribute, dao is connected to the database at entry
bool productDao::addAttribute(product &prod, prodAttr &attr, dataAccess &dao) {
	bool result{};

	std::string query = fmt::format(R"(INSERT INTO [dbo].[ProductAttributes]
           ([InstanceId],[Key],[Value]) VALUES ({}, '{}', '{}'))",
	       prod.instanceId, attr.first, attr.second);

	result = dao.execQuery(query, std::nullopt);

	return result;
}

// update product attribute, dao is connected to the database at entry
bool productDao::setAttribute(product &prod, prodAttr &attr, dataAccess &dao) {
	bool result{};

	std::string query = fmt::format(R"(UPDATE [dbo].[ProductAttributes]
		   SET [Value]='{}' WHERE [Key]='{}')",
	       prod.instanceId, attr.second, attr.first);

	result = dao.execQuery(query, std::nullopt);

	return result;
}

// create a product object from a recordSet row
product productDao::productFromRow(const int row, recordSet &rs) { 

	product prod(
		rs.getData(row, "Name"),
		rs.getData(row, "Description"),
		rs.getData(row, "ProductImageUris"),
		rs.getData(row, "ValidSkus"));

	int id{ std::stoi(rs.getData(row, "InstanceId")) };
	prod.setInstanceId(id);

	return prod;
}

