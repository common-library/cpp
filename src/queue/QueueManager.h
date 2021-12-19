#pragma once

#include <map>
#include <mutex>
#include <queue>
#include <memory>
using namespace std;

#include <boost/variant.hpp>

enum class E_QUEUE_TYPE {
	SAMPLE,
};

static const map<E_QUEUE_TYPE, string> GmapQueueTypeInfo = {
		{E_QUEUE_TYPE::SAMPLE, "SAMPLE"},
	};

class queue_visitor : public boost::static_visitor<> {
public:
	void operator()(boost::blank &) const {}

	int& operator()(int &data) const {
		return data;
	}

	string& operator()(string &data) const {
		return data;
	}
};

class QueueManager {
private:
	using QUEUE_TYPE = boost::variant<
										boost::blank,
										int,
										string
									>;

	QueueManager();
	virtual ~QueueManager() = default;

	map<const E_QUEUE_TYPE, const unique_ptr<mutex>> mapMutex;
	map<const E_QUEUE_TYPE, queue<QUEUE_TYPE>> mapQueue;
public:
	void Pop(const E_QUEUE_TYPE &eType);
	bool Empty(const E_QUEUE_TYPE &eType);
	int Size(const E_QUEUE_TYPE &eType);

	template <typename T>
	void Push(const E_QUEUE_TYPE &eType, const T &data);

	template<typename T>
	T Front(const E_QUEUE_TYPE &eType);

	static QueueManager& Instance();
};
#include "QueueManager.hpp"
