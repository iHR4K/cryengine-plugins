#pragma once
#include "IType.hpp"
#include "IFactory.hpp"

// conversion

namespace TC
{
	template<typename Base, typename Derived>
	struct SuperSubClass
	{
	private:
		typedef char y[1];
		typedef char n[2];

		template<typename T>
		static y& check(const volatile Derived&, T);
		static n& check(const volatile Base&, int);

		struct C
		{
			operator const volatile Base& () const;
			operator const volatile Derived& ();
		};

		static C getC();

	public:
		enum
		{
			exists = sizeof(check(getC(), 0)) == sizeof(y),
			sameType = false
		};
	};

	template<typename T>
	struct SuperSubClass<T, T>
	{
		enum
		{
			exists = true
		};
	};

}
// end conversion 

// typelist

//! \cond INTERNAL

#pragma once

namespace TL
{

	//! Typelist terminator.
	class NullType
	{
	};

	//! Structure for typelist generation.
	template<class T, class U = NullType>
	struct Typelist
	{
		typedef T Head;
		typedef U Tail;
	};

	//! Helper structure to automatically build typelists containing n types.
	template
		<
		typename T0 = NullType, typename T1 = NullType, typename T2 = NullType, typename T3 = NullType, typename T4 = NullType,
		typename T5 = NullType, typename T6 = NullType, typename T7 = NullType, typename T8 = NullType, typename T9 = NullType,
		typename T10 = NullType, typename T11 = NullType, typename T12 = NullType, typename T13 = NullType, typename T14 = NullType,
		typename T15 = NullType, typename T16 = NullType, typename T17 = NullType, typename T18 = NullType, typename T19 = NullType
		>
		struct BuildTypelist
	{
	private:
		typedef typename BuildTypelist<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19>::Result TailResult;

	public:
		typedef Typelist<T0, TailResult> Result;
	};

	template<>
	struct BuildTypelist<>
	{
		typedef NullType Result;
	};

	//! Typelist operation : Length.
	template<class TList> struct Length;

	template<>
	struct Length<NullType>
	{
		enum { value = 0 };
	};

	template<class T, class U>
	struct Length<Typelist<T, U>>
	{
		enum { value = 1 + Length<U>::value };
	};

	//! Typelist operation : TypeAt.
	template<class TList, unsigned int index> struct TypeAt;

	template<class Head, class Tail>
	struct TypeAt<Typelist<Head, Tail>, 0>
	{
		typedef Head Result;
	};

	template<class Head, class Tail, unsigned int index>
	struct TypeAt<Typelist<Head, Tail>, index>
	{
		typedef typename TypeAt<Tail, index - 1>::Result Result;
	};

	//! Typelist operation : IndexOf.
	template<class TList, class T> struct IndexOf;

	template<class T>
	struct IndexOf<NullType, T>
	{
		enum { value = -1 };
	};

	template<class T, class Tail>
	struct IndexOf<Typelist<T, Tail>, T>
	{
		enum { value = 0 };
	};

	template<class Head, class Tail, class T>
	struct IndexOf<Typelist<Head, Tail>, T>
	{
	private:
		enum { temp = IndexOf<Tail, T>::value };
	public:
		enum { value = temp == -1 ? -1 : 1 + temp };
	};

	//! Typelist operation : Append.
	template<class TList, class T> struct Append;

	template<> struct Append<NullType, NullType>
	{
		typedef NullType Result;
	};

	template<class T> struct Append<NullType, T>
	{
		typedef Typelist<T, NullType> Result;
	};

	template<class Head, class Tail>
	struct Append<NullType, Typelist<Head, Tail>>
	{
		typedef Typelist<Head, Tail> Result;
	};

	template<class Head, class Tail, class T>
	struct Append<Typelist<Head, Tail>, T>
	{
		typedef Typelist<Head, typename Append<Tail, T>::Result> Result;
	};

	//! Typelist operation : Erase.
	template<class TList, class T> struct Erase;

	template<class T>
	struct Erase<NullType, T>
	{
		typedef NullType Result;
	};

	template<class T, class Tail>
	struct Erase<Typelist<T, Tail>, T>
	{
		typedef Tail Result;
	};

	template<class Head, class Tail, class T>
	struct Erase<Typelist<Head, Tail>, T>
	{
		typedef Typelist<Head, typename Erase<Tail, T>::Result> Result;
	};

	//! Typelist operation : Erase All.
	template<class TList, class T> struct EraseAll;

	template<class T>
	struct EraseAll<NullType, T>
	{
		typedef NullType Result;
	};

	template<class T, class Tail>
	struct EraseAll<Typelist<T, Tail>, T>
	{
		typedef typename EraseAll<Tail, T>::Result Result;
	};

	template<class Head, class Tail, class T>
	struct EraseAll<Typelist<Head, Tail>, T>
	{
		typedef Typelist<Head, typename EraseAll<Tail, T>::Result> Result;
	};

	//! Typelist operation : NoDuplicates.
	template<class TList> struct NoDuplicates;

	template<>
	struct NoDuplicates<NullType>
	{
		typedef NullType Result;
	};

	template<class Head, class Tail>
	struct NoDuplicates<Typelist<Head, Tail>>
	{
	private:
		typedef typename NoDuplicates<Tail>::Result L1;
		typedef typename Erase<L1, Head>::Result    L2;
	public:
		typedef Typelist<Head, L2>                  Result;
	};

}

//! \endcond
// end typelist

namespace CW
{
	namespace Internal
	{
		template<class Dst> struct InterfaceCast;

		template<class Dst>
		struct InterfaceCast
		{
			template<class T>
			static void* Op(T* p)
			{
				return (Dst*)p;
			}
		};

		template<>
		struct InterfaceCast<IUnknownType>
		{
			template<class T>
			static void* Op(T* p)
			{
				return const_cast<IUnknownType*>(static_cast<const IUnknownType*>(static_cast<const void*>(p)));
			}
		};
	}

	template<class TList> struct InterfaceCast;

	template<>
	struct InterfaceCast<TL::NullType>
	{
		template<class T>
		static void* Op(T*, const InterfaceID&)
		{
			return 0;
		}
	};

	template<class Head, class Tail>
	struct InterfaceCast<TL::Typelist<Head, Tail>>
	{
		template<class T>
		static void* Op(T* p, const InterfaceID& iid)
		{
			if (Type::iidof<Head>() == iid)
				return Internal::InterfaceCast<Head>::Op(p);
			return InterfaceCast<Tail>::Op(p, iid);
		}
	};

	template<class TList> struct FillIIDs;

	template<>
	struct FillIIDs<TL::NullType>
	{
		static void Op(InterfaceID*)
		{
		}
	};

	template<class Head, class Tail>
	struct FillIIDs<TL::Typelist<Head, Tail>>
	{
		static void Op(InterfaceID* p)
		{
			*p++ = Type::iidof<Head>();
			FillIIDs<Tail>::Op(p);
		}
	};

	namespace Internal
	{
		template<bool, typename S> struct PickList;

		template<bool, typename S>
		struct PickList
		{
			typedef TL::BuildTypelist<>::Result Result;
		};

		template<typename S>
		struct PickList<true, S>
		{
			typedef typename S::FullCompositeList Result;
		};
	}

	template<typename T>
	struct ProbeFullCompositeList
	{
	private:
		typedef char y[1];
		typedef char n[2];

		template<typename S>
		static y& test(typename S::FullCompositeList*);

		template<typename>
		static n& test(...);

	public:
		enum
		{
			listFound = sizeof(test<T>(0)) == sizeof(y)
		};

		typedef typename Internal::PickList<listFound, T>::Result ListType;
	};

	namespace Internal
	{
		template<class TList> struct CompositeQuery;

		template<>
		struct CompositeQuery<TL::NullType>
		{
			template<typename T>
			static void* Op(const T&, const char*)
			{
				return 0;
			}
		};

		template<class Head, class Tail>
		struct CompositeQuery<TL::Typelist<Head, Tail>>
		{
			template<typename T>
			static void* Op(const T& ref, const char* name)
			{
				void* p = ref.Head::CompositeQueryImpl(name);
				return p ? p : CompositeQuery<Tail>::Op(ref, name);
			}
		};
	}

	struct CompositeQuery
	{
		template<typename T>
		static void* Op(const T& ref, const char* name)
		{
			return Internal::CompositeQuery<typename ProbeFullCompositeList<T>::ListType>::Op(ref, name);
		}
	};

	inline bool NameMatch(const char* name, const char* compositeName)
	{
		if (!name || !compositeName)
			return false;
		size_t i = 0;
		for (; name[i] && name[i] == compositeName[i]; ++i)
		{
		}
		return name[i] == compositeName[i];
	}

	template<typename T>
	void* CheckCompositeMatch(const char* name, const std::shared_ptr<T>& composite, const char* compositeName)
	{
		typedef TC::SuperSubClass<IUnknownType, T> Rel;
		static_assert(Rel::exists, "Unexpected enum value!");
		return NameMatch(name, compositeName) ? const_cast<void*>(static_cast<const void*>(&composite)) : 0;
	}

}



#define _BEFRIEND_CRYCOMPOSITE_QUERY()                                                                                                             
// NOTE: original code has these methods - but we don't need this for our study targets
//template<class Src> friend std::shared_ptr<ICryUnknown> CompositeQuerySemantics::crycomposite_query(Src*, const char*, bool*);                         \
  //template<class Src> friend std::shared_ptr<const ICryUnknown> CompositeQuerySemantics::crycomposite_query(const Src*, const char*, bool*);             \
  //template<class Src> friend std::shared_ptr<ICryUnknown> CompositeQuerySemantics::crycomposite_query(const std::shared_ptr<Src> &, const char*, bool*); \
  //template<class Src> friend std::shared_ptr<const ICryUnknown> CompositeQuerySemantics::crycomposite_query(const std::shared_ptr<const Src> &, const char*, bool*);




#define CRYINTERFACE_BEGIN() \
  private:                   \
    typedef TL::BuildTypelist < IUnknownType

#define CRYINTERFACE_ADD(iname)                        , iname

#define CRYINTERFACE_END()                             > ::Result _UserDefinedPartialInterfaceList; \
  protected:                                                                                        \
    typedef TL::NoDuplicates<_UserDefinedPartialInterfaceList>::Result FullInterfaceList;

#define _CRY_TPL_APPEND0(base)                         TL::Append<base::FullInterfaceList, _UserDefinedPartialInterfaceList>::Result
#define _CRY_TPL_APPEND(base, intermediate)            TL::Append<base::FullInterfaceList, intermediate>::Result

#define CRYINTERFACE_ENDWITHBASE(base)                 > ::Result _UserDefinedPartialInterfaceList; \
  protected:                                                                                        \
    typedef TL::NoDuplicates<_CRY_TPL_APPEND0(base)>::Result FullInterfaceList;

#define CRYINTERFACE_ENDWITHBASE2(base0, base1)        > ::Result _UserDefinedPartialInterfaceList; \
  protected:                                                                                        \
    typedef TL::NoDuplicates<_CRY_TPL_APPEND(base0, _CRY_TPL_APPEND0(base1))>::Result FullInterfaceList;

#define CRYINTERFACE_ENDWITHBASE3(base0, base1, base2) > ::Result _UserDefinedPartialInterfaceList; \
  protected:                                                                                        \
    typedef TL::NoDuplicates<_CRY_TPL_APPEND(base0, _CRY_TPL_APPEND(base1, _CRY_TPL_APPEND0(base2)))>::Result FullInterfaceList;

#define CRYINTERFACE_SIMPLE(iname) \
  CRYINTERFACE_BEGIN()             \
  CRYINTERFACE_ADD(iname)          \
  CRYINTERFACE_END()

#define CRYCOMPOSITE_BEGIN()                         \
  private:                                           \
    void* CompositeQueryImpl(const char* name) const \
    {                                                \
      (void)(name);                                  \
      void* res = 0; (void)(res);                    \

#define CRYCOMPOSITE_ADD(member, membername)                                   \
  static_assert(CRY_ARRAY_COUNT(membername) > 1, "'membername' is too short"); \
  if ((res = CW::CheckCompositeMatch(name, member, membername)) != 0)          \
    return res;

#define _CRYCOMPOSITE_END(implclassname)                                  \
  return 0;                                                               \
  };                                                                      \
protected:                                                                \
  typedef TL::BuildTypelist<implclassname>::Result _PartialCompositeList; \
                                                                          \
  template<bool, typename S> friend struct CW::Internal::PickList;

#define CRYCOMPOSITE_END(implclassname) \
  _CRYCOMPOSITE_END(implclassname)      \
protected:                              \
  typedef _PartialCompositeList FullCompositeList;

#define _CRYCOMPOSITE_APPEND0(base)              TL::Append<_PartialCompositeList, CW::ProbeFullCompositeList<base>::ListType>::Result
#define _CRYCOMPOSITE_APPEND(base, intermediate) TL::Append<intermediate, CW::ProbeFullCompositeList<base>::ListType>::Result

#define CRYCOMPOSITE_ENDWITHBASE(implclassname, base) \
  _CRYCOMPOSITE_END(implclassname)                    \
protected:                                            \
  typedef _CRYCOMPOSITE_APPEND0 (base) FullCompositeList;

#define CRYCOMPOSITE_ENDWITHBASE2(implclassname, base0, base1) \
  _CRYCOMPOSITE_END(implclassname)                             \
protected:                                                     \
  typedef TL::NoDuplicates<_CRYCOMPOSITE_APPEND(base1, _CRYCOMPOSITE_APPEND0(base0))>::Result FullCompositeList;

#define CRYCOMPOSITE_ENDWITHBASE3(implclassname, base0, base1, base2) \
  _CRYCOMPOSITE_END(implclassname)                                    \
protected:                                                            \
  typedef TL::NoDuplicates<_CRYCOMPOSITE_APPEND(base2, _CRYCOMPOSITE_APPEND(base1, _CRYCOMPOSITE_APPEND0(base0)))>::Result FullCompositeList;

template<typename T>
class Factory : public Foundation::IFactory
{
public:
	virtual const char* GetName() const
	{
		return T::GetClassName();
	}

	virtual ClassID const& GetClassID() const
	{
		static constexpr ClassID cid = T::GetClassID();
		return cid;
	}

	virtual bool SupportsClass(InterfaceID const& iid) const
	{
		for (size_t i = 0; i < m_numIIDs; ++i)
		{
			if (iid == m_pIIDs[i])
				return true;
		}
		return false;
	}

	virtual void SupportsClass(const InterfaceID*& pIIDs, size_t& numIIDs) const
	{
		pIIDs = m_pIIDs;
		numIIDs = m_numIIDs;
	}

public:
	struct CustomDeleter
	{
		void operator()(T* p)
		{
			// NOTE: oversimplifed version - original code calls the destructor here - because of the custom alloc
			free(p);
		}
	};

	virtual std::shared_ptr<IUnknownType> CreateClassInstance() const
	{
		// NOTE: oversimplifed version 
		return TypeCast::interface_cast<IUnknownType>(std::shared_ptr<T>(new T(), CustomDeleter()));
	}

	Factory()
		: m_numIIDs(0)
		, m_pIIDs(0)
		, m_regFactory()
	{
		static InterfaceID supportedIIDs[TL::Length < typename T::FullInterfaceList > ::value];
		CW::FillIIDs<typename T::FullInterfaceList>::Op(supportedIIDs);
		m_pIIDs = &supportedIIDs[0];
		m_numIIDs = TL::Length<typename T::FullInterfaceList>::value;
		new(&m_regFactory)SRegFactoryNode(this);
	}

protected:
	Factory(const Factory&);
	Factory& operator=(const Factory&);

	size_t          m_numIIDs;
	InterfaceID* m_pIIDs;
	SRegFactoryNode m_regFactory;
};

template<typename T>
class CSingletonFactory : public Factory<T>
{
public:
	CSingletonFactory()
		: Factory<T>()
	{
	}

	virtual std::shared_ptr<IUnknownType> CreateClassInstance() const
	{
		static std::shared_ptr<IUnknownType> p = Factory<T>::CreateClassInstance();
		return p;
	}
};

#define _CRYFACTORY_DECLARE(implclassname) \
  private:                                 \
    friend class Factory<implclassname>;  \
    static Factory<implclassname> s_factory;

#define _CRYFACTORY_DECLARE_SINGLETON(implclassname) \
  private:                                           \
    friend class Factory<implclassname>;            \
    static CSingletonFactory<implclassname> s_factory;

#define _IMPLEMENT_ICRYUNKNOWN()                                           \
  public:                                                                  \
    virtual Foundation::IFactory* GetFactory() const override                       \
    {                                                                      \
      return &s_factory;                                                   \
    }                                                                      \
                                                                           \
  protected:                                                               \
    virtual void* QueryInterface(const InterfaceID &iid) const override \
    {                                                                      \
      return CW::InterfaceCast<FullInterfaceList>::Op(this, iid);          \
    }                                                                      \
                                                                           \
    template<class TList> friend struct CW::Internal::CompositeQuery;      \
                                                                           \
    virtual void* QueryComposite(const char* name) const override          \
    {                                                                      \
      return CW::CompositeQuery::Op(*this, name);                          \
    }

#define _ENFORCE_CRYFACTORY_USAGE(implclassname, cname, cidHigh, cidLow) CRY_PP_ERROR("Deprecated macro: Use _ENFORCE_CRYFACTORY_USAGE_GUID instead. Please refer to the Migrating Guide from CRYENGINE 5.3 to CRYENGINE 5.4 for more details.")

#define _ENFORCE_CRYFACTORY_USAGE_GUID(implclassname, cname, guid)                                                  \
  public:                                                                                                           \
    static const char* GetClassName() { return cname; }                                                                 \
    static constexpr ClassID GetClassID() { return guid; }                                                           \
    static std::shared_ptr<implclassname> CreateClassInstance()                                                     \
    {                                                                                                               \
      std::shared_ptr<IUnknownType> p = s_factory.CreateClassInstance();                                                           \
      return std::shared_ptr<implclassname>(*static_cast<std::shared_ptr<implclassname>*>(static_cast<void*>(&p))); \
    }

#define _BEFRIEND_OPS()           \
  _BEFRIEND_INTERFACE_CAST()   \
  _BEFRIEND_CRYCOMPOSITE_QUERY()

#define CRYGENERATE_CLASS(implclassname, cname, cidHigh, cidLow) CRY_PP_ERROR("Deprecated macro: Use CRYGENERATE_CLASS_GUID instead. Please refer to the Migrating Guide from CRYENGINE 5.3 to CRYENGINE 5.4 for more details.")

#define CRYGENERATE_CLASS_GUID(implclassname, cname, classGuid)  \
  friend struct Factory<implclassname>::CustomDeleter;          \
  _CRYFACTORY_DECLARE(implclassname)                             \
  _BEFRIEND_OPS()                                                \
  _IMPLEMENT_ICRYUNKNOWN()                                       \
  _ENFORCE_CRYFACTORY_USAGE_GUID(implclassname, cname, classGuid)


#define CRYGENERATE_CLASS_FROM_INTERFACE(implclassname, interfaceName, cname, cidHigh, cidLow) CRY_PP_ERROR("Deprecated macro: Use CRYGENERATE_CLASS_FROM_INTERFACE_GUID instead. Please refer to the Migrating Guide from CRYENGINE 5.3 to CRYENGINE 5.4 for more details.")

#define CRYGENERATE_CLASS_FROM_INTERFACE_GUID(implclassname, interfaceName, cname, classGuid) \
  CRYINTERFACE_SIMPLE(interfaceName)                                                           \
  CRYGENERATE_CLASS_GUID(implclassname, cname, classGuid)

#define CRYGENERATE_SINGLETONCLASS(implclassname, cname, cidHigh, cidLow) CRY_PP_ERROR("Deprecated macro: Use CRYGENERATE_SINGLETONCLASS_GUID instead. Please refer to the Migrating Guide from CRYENGINE 5.3 to CRYENGINE 5.4 for more details.")

#define CRYGENERATE_SINGLETONCLASS_GUID(implclassname, cname, classGuid) \
  friend struct Factory<implclassname>::CustomDeleter;                  \
  _CRYFACTORY_DECLARE_SINGLETON(implclassname)                           \
  _BEFRIEND_OPS()                                                        \
  _IMPLEMENT_ICRYUNKNOWN()                                               \
  _ENFORCE_CRYFACTORY_USAGE_GUID(implclassname, cname, classGuid)

#define CRYREGISTER_CLASS(implclassname) \
  Factory<implclassname> implclassname::s_factory;

#define CRYREGISTER_SINGLETON_CLASS(implclassname) \
  CSingletonFactory<implclassname> implclassname::s_factory;



struct IUnknownType
{
	GUID(IUnknownType, "unknown_type"_generate_guid);

	_BEFRIEND_INTERFACE_CAST()
		//_BEFRIEND_COMPOSITE_QUERY()

		virtual Foundation::IFactory* GetFactory() const = 0;

protected:
	virtual void* QueryInterface(const InterfaceID& iid) const = 0;
	virtual void* QueryComposite(const char* name) const = 0;
};
