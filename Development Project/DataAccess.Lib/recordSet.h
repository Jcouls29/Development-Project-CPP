#pragma once

#include <vector>
#include <string>
#include "dataAccess.h"

// object for holding results of SQL queries
class recordSet {
public:
	size_t getCount() const { return records.size(); };

	// TODO: implement and API to populate column names, types, and data records with query results
	std::string getData(const int row, const std::string column);

private:
	friend class dataAccess;
	std::vector<std::string> columnNames;
    std::vector<SQLSMALLINT> columnTypes;
	std::vector<std::string> records;
};
