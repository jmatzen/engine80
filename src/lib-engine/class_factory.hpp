#pragma once

#include "engine80.hpp"

namespace qf
{
	class AbstractClassFactory
	{
	public:
		virtual ptr<Serializable> create() const = 0;
	};


	template<typename T>
	class TClassFactory : public AbstractClassFactory
	{
		virtual ptr<Serializable> create() const override {
			auto obj = std::make_shared<T>();
			if (obj) {
				if (auto res = obj->postConstruct(); res.has_value()) {
					return obj;
				}
			}
			return nullptr;
		}
	};
}
