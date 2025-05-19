#pragma once

#include <Foundation/IFactory.hpp>
#include <Foundation/IFactoryRegistry.hpp>

#include <vector>
#include <algorithm>
#include <cassert>

using namespace Foundation;

class FactoryRegistryImpl : public IFactoryRegistryImpl
{
public:
	virtual IFactory* GetFactory(const char* cname) const override
	{
		FactoryToName search{ cname, nullptr };
		auto it = std::lower_bound(m_byName.begin(), m_byName.end(), search);
		return (it != m_byName.end() ? (*it).Pointer : nullptr);
	}

	virtual IFactory* GetFactory(ClassID const& cid) const
	{
		FactoryToClassID search{ cid, nullptr };
		auto it = std::lower_bound(m_FactoriesToClassID.begin(), m_FactoriesToClassID.end(), search);
		return (it != m_FactoriesToClassID.end() ? (*it).Pointer : nullptr);
	}

	virtual void IterateFactories(InterfaceID const& iid, IFactory** pFactories, size_t& factoriesCount) const override
	{
		struct LessPredFactoryByIIDOnly
		{
			bool operator()(const FactoryToInterfaceID& lhs, const FactoryToInterfaceID& rhs) const { return lhs.ID < rhs.ID; }
		};

		typedef std::pair<FactoriesToInterfaceIDConstIt, FactoriesToInterfaceIDConstIt> SearchResult;
		SearchResult res = std::equal_range(m_FactoriesToInterfaceID.begin(), m_FactoriesToInterfaceID.end(), FactoryToInterfaceID{ iid, nullptr }, LessPredFactoryByIIDOnly());

		const size_t numFactoriesFound = std::distance(res.first, res.second);
		if (pFactories)
		{
			factoriesCount = std::min(factoriesCount, numFactoriesFound);
			FactoriesToInterfaceIDConstIt it = res.first;
			for (size_t i = 0; i < factoriesCount; ++i, ++it)
				pFactories[i] = (*it).Pointer;
		}
		else
			factoriesCount = numFactoriesFound;
	}

	virtual void RegisterFactories(const SRegFactoryNode* pFactories)
	{
		size_t numFactoriesToAdd = 0;
		size_t numInterfacesSupported = 0;
		{
			const SRegFactoryNode* p = pFactories;
			while (p)
			{
				IFactory* pFactory = p->m_pFactory;
				assert(pFactory);
				if (pFactory)
				{
					const InterfaceID* pIIDs = 0;
					size_t numIIDs = 0;
					pFactory->SupportsClass(pIIDs, numIIDs);

					numInterfacesSupported += numIIDs;
					++numFactoriesToAdd;
				}

				p = p->m_pNext;
			}
		}

		{
			m_byName.reserve(m_byName.size() + numFactoriesToAdd);
			m_FactoriesToClassID.reserve(m_FactoriesToClassID.size() + numFactoriesToAdd);
			m_FactoriesToInterfaceID.reserve(m_FactoriesToInterfaceID.size() + numInterfacesSupported);

			size_t numFactoriesAdded = 0;
			const SRegFactoryNode* p = pFactories;
			while (p)
			{
				IFactory* pFactory = p->m_pFactory;
				if (pFactory)
				{
					FactoriesByNameIt itPosForCName;
					FactoriesToClassIDIt itPosForCID;
					if (GetInsertionPosition(pFactory, itPosForCName, itPosForCID))
					{
						m_byName.insert(itPosForCName, FactoryToName{ pFactory->GetName(), pFactory });
						m_FactoriesToClassID.insert(itPosForCID, FactoryToClassID{ pFactory->GetClassID(), pFactory });

						const InterfaceID* pIIDs = 0;
						size_t numIIDs = 0;
						pFactory->SupportsClass(pIIDs, numIIDs);

						for (size_t i = 0; i < numIIDs; ++i)
						{
							const FactoryToInterfaceID newFactory{ pIIDs[i], pFactory };
							m_FactoriesToInterfaceID.push_back(newFactory);
						}

						++numFactoriesAdded;
					}
				}

				p = p->m_pNext;
			}

			if (numFactoriesAdded)
			{
				std::sort(m_FactoriesToInterfaceID.begin(), m_FactoriesToInterfaceID.end());
			}
		}
	}

	virtual void UnregisterFactories(const SRegFactoryNode* pFactories)
	{
	}

	virtual void UnregisterFactory(IFactory* const pFactory)
	{
	}

	static FactoryRegistryImpl& Access()
	{
		return s_Registry;
	}

private:
	struct FactoryToName
	{
		const char* Name;
		IFactory* Pointer{ nullptr };

		bool operator<(const FactoryToName& rhs) const { return strcmp(Name, rhs.Name) < 0; }
	};


	typedef std::vector<FactoryToName>      FactoriesByName;
	typedef FactoriesByName::iterator       FactoriesByNameIt;
	typedef FactoriesByName::const_iterator FactoriesByNameConstIt;

	struct FactoryToClassID
	{
		ClassID ID;
		IFactory* Pointer{ nullptr };

		bool operator<(const FactoryToClassID& rhs) const { return ID < rhs.ID; }
	};

	typedef std::vector<FactoryToClassID>      FactoriesToClassID;
	typedef FactoriesToClassID::iterator       FactoriesToClassIDIt;
	typedef FactoriesToClassID::const_iterator FactoriesToClassIDConstIt;

	struct FactoryToInterfaceID
	{
		InterfaceID ID;
		Foundation::IFactory* Pointer{ nullptr };

		bool operator<(const FactoryToInterfaceID& rhs) const { if (ID != rhs.ID) return ID < rhs.ID; return Pointer < rhs.Pointer; }
	};

	typedef std::vector<FactoryToInterfaceID>      FactoriesToInterfaceID;
	typedef FactoriesToInterfaceID::iterator       FactoriesToInterfaceIDIt;
	typedef FactoriesToInterfaceID::const_iterator FactoriesToInterfaceIDConstIt;

private:
	FactoryRegistryImpl()
	{
	}

	~FactoryRegistryImpl()
	{
	}

	bool GetInsertionPosition(Foundation::IFactory* pFactory, FactoriesByNameIt& itPosForCName, FactoriesToClassIDIt& itPosForCID)
	{
		assert(pFactory);

		struct FatalError
		{
			static void Report(Foundation::IFactory* pKnownFactory, Foundation::IFactory* pNewFactory)
			{
				exit(0);
			}
		};


		FactoryToName searchByClassName{ pFactory->GetName(), pFactory };
		auto itForCName = std::lower_bound(m_byName.begin(), m_byName.end(), searchByClassName);
		auto itForCNameIsExist = itForCName != m_byName.end();

		FactoryToClassID searchByClassID{ pFactory->GetClassID(), pFactory };
		auto itForCID = std::lower_bound(m_FactoriesToClassID.begin(), m_FactoriesToClassID.end(), searchByClassID);
		auto itForCIDIsExist = itForCID != m_FactoriesToClassID.end();

		itPosForCName = itForCName;
		itPosForCID = itForCID;

		return true;
	}

	void UnregisterFactoryInternal(Foundation::IFactory* const pFactory)
	{
	}

	static FactoryRegistryImpl s_Registry; // singleton

	FactoriesByName          m_byName;
	FactoriesToInterfaceID  m_FactoriesToInterfaceID;
	FactoriesToClassID m_FactoriesToClassID;
};

FactoryRegistryImpl FactoryRegistryImpl::s_Registry;