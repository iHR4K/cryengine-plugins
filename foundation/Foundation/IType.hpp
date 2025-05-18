#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <cstdio> 
#include <cstdarg>

typedef uint64_t __GUID;
typedef __GUID InterfaceID;
typedef __GUID ClassID;

// NOTE: some impl from web - CT are using another one, but this is much simplier in the code terms and also constexpr
constexpr size_t constexpr_hash(std::string_view str)
{
	size_t hash_value = 5381;
	for (char c : str)
	{
		hash_value = ((hash_value << 5) + hash_value) + c; // DJB2 hash algorithm
	}
	return hash_value;
}


constexpr __GUID operator"" _generate_guid(const char* const szInput, size_t)
{
	return (szInput[0] == '{') ? constexpr_hash(szInput + 1) : constexpr_hash(szInput);
}

namespace Type
{
	template<class T>
	struct has_iidof
	{
		typedef char(&Yes)[1];
		typedef char(&No)[2];

		template <typename C> static Yes Check(decltype(&C::IID));
		template <typename> static No Check(...);

		static constexpr bool value = sizeof(Check<T>(0)) == sizeof(Yes);
	};

	template<typename T>
	InterfaceID const& iidof()
	{
		return T::IID();
	}
}

struct IUnknownType;
namespace TypeCast
{
	template<class Dst, class Src>
	Dst* interface_cast(Src* p)
	{
		return static_cast<Dst*>(p ? p->QueryInterface(Type::iidof<Dst>()) : nullptr);
	}

	template<class Dst, class Src>
	Dst* interface_cast(const Src* p)
	{
		return static_cast<const Dst*>(p ? p->QueryInterface(Type::iidof<Dst>()) : nullptr);
	}

	template<class Dst, class Src>
	struct interface_cast_helper
	{
		static std::shared_ptr<Dst> Op(const std::shared_ptr<Src>& p)
		{
			Dst* dp = interface_cast<Dst>(p.get());
			return dp ? std::shared_ptr<Dst>(p, dp) : std::shared_ptr<Dst>();
		}
	};

	template<class Src>
	struct interface_cast_helper<IUnknownType, Src>
	{
		static std::shared_ptr<IUnknownType> Op(const std::shared_ptr<Src>& p)
		{
			IUnknownType* dp = interface_cast<IUnknownType>(p.get());
			return dp ? std::shared_ptr<IUnknownType>(*((const std::shared_ptr<IUnknownType>*) & p), dp) : std::shared_ptr<IUnknownType>();
		}
	};

	template<class Src>
	struct interface_cast_helper<const IUnknownType, Src>
	{
		static std::shared_ptr<const IUnknownType> Op(const std::shared_ptr<Src>& p)
		{
			const IUnknownType* dp = interface_cast<const IUnknownType>(p.get());
			return dp ? std::shared_ptr<const IUnknownType>(*((const std::shared_ptr<const IUnknownType>*) & p), dp) : std::shared_ptr<const IUnknownType>();
		}
	};

	template<class Dst, class Src>
	std::shared_ptr<Dst> interface_cast(const std::shared_ptr<Src>& p)
	{
		return interface_cast_helper<Dst, Src>::Op(p);
	}


#define _BEFRIEND_INTERFACE_CAST()  \
template<class Dst, class Src> friend Dst* TypeCast::interface_cast(Src*);       \
template<class Dst, class Src> friend Dst* TypeCast::interface_cast(const Src*); \
template<class Dst, class Src> friend std::shared_ptr<Dst> TypeCast::interface_cast(const std::shared_ptr<Src>&);
}

#define FRIEND_IDOF \
	template<class T> friend InterfaceID const & Type::iidof(); \
	template<class T> friend struct Type::has_iidof; \

#define GUID(name, guid) \
public: \
	FRIEND_IDOF \
	friend struct std::default_delete<name>; \
private: \
static InterfaceID const& IID() { static constexpr __GUID lguid = guid; return lguid; }\
public:


class IType
{
};