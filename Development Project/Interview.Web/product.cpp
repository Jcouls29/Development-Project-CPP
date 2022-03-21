#include "product.h"
#include "dataAccess.h"
#include "recordSet.h"

product::product(const std::string _name, const std::string _description,
                 const std::string _imageUri, const std::string _sku)
    : instanceId {}, name(_name), description(_description), imageUri(_imageUri), sku(_sku) {
}

void product::setInstanceId(const int id) {
    instanceId = id;
}

void product::setCategories(const std::vector<int> &_categories) {
    categories = _categories;
}

void product::setAttributes(const attrMap &_attributes) {
    attributes = _attributes;
}

json product::to_json() const {
    json j;

    j["instanceId"] = instanceId;
	j["name"] = name;
	j["description"] = description;
	j["imageUri"] = imageUri;
	j["sku"] = sku;
	j["categories"] = categories;
	j["attributes"] = attributes;

    return j;
}
