#pragma once

#include <utility>

using namespace std;

template <class T> class Singleton {
	private:
		Singleton();
		Singleton(Singleton const &);
		~Singleton();

		Singleton &operator=(Singleton const &);

	public:
		template <class... Args> static T &Instance(Args &&...args) {
			static T instance(forward<Args>(args)...);
			return instance;
		}
};
