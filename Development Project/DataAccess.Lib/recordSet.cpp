#include <stdexcept>
#include "recordSet.h"

std::string recordSet::getData(const int row, const std::string column) {

	if (row < 0 || row >= static_cast<int>(records.size()))
		throw std::out_of_range("invalid row index: " + row);

	if (columnMap.find(column) == columnMap.end())
		throw std::out_of_range("invalid column name: " + column);

	return records[row][columnMap[column]];
}
