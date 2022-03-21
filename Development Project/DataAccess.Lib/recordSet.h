#pragma once

#include <vector>
#include <string>
#include <map>
#include "dataAccess.h"

using record = std::vector<std::string>;

// object for holding results of SQL queries
class recordSet {
public:
	size_t getCount() const { return records.size(); };

	std::string getData(const int row, const std::string column);

private:
	friend class dataAccess;
	std::vector<std::string> columnNames;
    std::vector<SQLSMALLINT> columnTypes;
	std::vector<record> records;
	std::map<std::string, int> columnMap; // map column name to column index
};
