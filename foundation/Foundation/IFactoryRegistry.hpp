#pragma once
#include "IType.hpp"
#include "IFactory.hpp"

class IFactoryRegistry
{
public:
	virtual Foundation::IFactory* GetFactory(ClassID const&) const = 0;
	virtual void IterateFactories(InterfaceID const&, Foundation::IFactory** pFactories, size_t& factoriesCount) const = 0;
};

class IFactoryRegistryImpl : public IFactoryRegistry
{
public:
	virtual Foundation::IFactory* GetFactory(const char* cname) const = 0;
	virtual Foundation::IFactory* GetFactory(ClassID const& cid) const = 0;
	virtual void IterateFactories(InterfaceID const& iid, Foundation::IFactory** pFactories, size_t& factoriesCount) const = 0;

	virtual void RegisterFactories(const SRegFactoryNode* pFactories) = 0;
	virtual void UnregisterFactories(const SRegFactoryNode* pFactories) = 0;
	virtual void UnregisterFactory(Foundation::IFactory* const pFactory) = 0;
};