#include "class_factory.hpp"

#include <vulkan/vulkan.h>


#include <unordered_map>

using namespace e80;

namespace e80
{
	class Sdl2PlatformInterface;
	
	std::unordered_map<UUID, ptr<AbstractClassFactory>> factoryMap;
}

void e80::registerFactories()
{
	registerFactory<Sdl2PlatformInterface>();

}

void e80::registerFactory(const UUID& clsid, ptr<AbstractClassFactory> const& cf)
{
	factoryMap.emplace(clsid, cf);
}

ptr<Serializable> e80::createInstance_(const UUID& clsid) {
	auto it = factoryMap.find(clsid);
	if (it == factoryMap.end()) {
		return nullptr;
	}
	return it->second->create();
}