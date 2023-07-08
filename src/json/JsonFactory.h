#include "Json.h"
#include <memory>

using namespace std;

enum class JSON_TYPE {
	RABBIT,
	RAPIDJSON,
};

class JsonFactory {
	private:
		JsonFactory() = default;
		~JsonFactory() = default;

	public:
		unique_ptr<Json> Make(const JSON_TYPE &type);

		static JsonFactory &Instance();
};
