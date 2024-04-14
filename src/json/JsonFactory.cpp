#include "JsonFactory.h"
#include "RabbitJson.h"
#include "RapidjsonJson.h"
#include <memory>

unique_ptr<Json> JsonFactory::Make(const JSON_TYPE &type) {
	switch (type) {
	case JSON_TYPE::RABBIT:
		return make_unique<RabbitJson>();
	case JSON_TYPE::RAPIDJSON:
		return make_unique<RapidjsonJson>();
	default:
		return nullptr;
	}
}

JsonFactory &JsonFactory::Instance() {
	static JsonFactory jsonFactory;
	return jsonFactory;
}
