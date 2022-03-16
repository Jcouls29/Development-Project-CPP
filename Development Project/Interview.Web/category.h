#pragma once

#include <string>
#include <vector>
#include "json.hpp"

using nlohmann::json;

class category {
public:
	category() = delete;
    category(const std::string _name, const std::string _description);
	category &operator=(const category &) = delete;

	category getCategory(const int instanceId);
	std::vector<category> getCategories();
	json toJson() const;

private:
	int instanceId {};
	std::string name;
	std::string description;
};