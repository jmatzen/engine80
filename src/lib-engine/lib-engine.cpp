#include "class_factory.hpp"

#include <vulkan/vulkan.h>


#include <unordered_map>

using namespace qf;

namespace qf
{
	class Sdl2PlatformInterface;
	
	std::unordered_map<UUID, ptr<AbstractClassFactory>> factoryMap;
}

void qf::registerFactories()
{
	registerFactory<Sdl2PlatformInterface>();

}

void qf::registerFactory(const UUID& clsid, ptr<AbstractClassFactory> const& cf)
{
	factoryMap.emplace(clsid, cf);
}

ptr<Serializable> qf::createInstance_(const UUID& clsid) {
	auto it = factoryMap.find(clsid);
	if (it == factoryMap.end()) {
		return nullptr;
	}
	return it->second->create();
}