#pragma once

#include "IType.hpp"

namespace Foundation
{
	class IFactory;
}

struct SRegFactoryNode;

#define DLL_EXPORT __declspec(dllexport)

extern SRegFactoryNode* g_pHeadToRegFactories;
extern "C" DLL_EXPORT SRegFactoryNode* GetHeadToRegFactories();
typedef SRegFactoryNode* (*PtrFunc_GetHeadToRegFactories)();

struct SRegFactoryNode
{
	SRegFactoryNode()
	{
	}

	SRegFactoryNode(Foundation::IFactory* pFactory)
		: m_pFactory(pFactory)
		, m_pNext(g_pHeadToRegFactories)
	{
		g_pHeadToRegFactories = this;
	}

	static void* operator new(size_t, void* p)
	{
		return p;
	}

	static void operator delete(void*, void*)
	{
	}

	Foundation::IFactory* m_pFactory;
	SRegFactoryNode* m_pNext;
};

struct IUnknownType;

namespace Foundation
{
	class IFactory
	{
	public:
		virtual const char* GetName() const = 0;
		virtual ClassID const& GetClassID() const = 0;
		virtual bool SupportsClass(InterfaceID const&) const = 0;
		virtual void SupportsClass(const InterfaceID*& pIds, size_t& idsCount) const = 0;
		virtual std::shared_ptr<IUnknownType> CreateClassInstance() const = 0;

		virtual ~IFactory() = default;

	private:
	};
}