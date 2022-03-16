#include "category.h"

category::category(const std::string _name, const std::string _description)
	: instanceId{}, name(_name), description(_description) {

}

category category::getCategory(const int instanceId) {
	category cat("name", "description");

	// TODO: query for a single category

	return cat;
}

std::vector<category> category::getCategories() {
	std::vector<category> cats;

	// TODO: query for all categories

	return cats;
}

json category::toJson() const {
	json j;

	j["instanceId"]  = instanceId;
	j["name"]        = name;
	j["description"] = description;

	return j;
}
