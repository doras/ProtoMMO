#pragma once

#include "Types.h"

#pragma region TypeList
template<typename... Ts>
struct TypeList;

template<typename T1, typename T2>
struct TypeList<T1, T2>
{
	using Head = T1;
	using Tail = T2;
};

template<typename T, typename... Ts>
struct TypeList<T, Ts...>
{
	using Head = T;
	using Tail = TypeList<Ts...>;
};
#pragma endregion

#pragma region Length
template<typename TL>
struct Length;

template<>
struct Length<TypeList<>>
{
	static constexpr int32 value = 0;
};

template<typename T, typename... Ts>
struct Length<TypeList<T, Ts...>>
{
	static constexpr int32 value = 1 + Length<TypeList<Ts...>>::value;
};
#pragma endregion

#pragma region TypeAt
template<typename TL, int32 index>
struct TypeAt;

template<typename T, typename... Ts>
struct TypeAt<TypeList<T, Ts...>, 0>
{
	using type = T;
};

template<typename T, typename... Ts, int32 index>
struct TypeAt<TypeList<T, Ts...>, index>
{
	using type = typename TypeAt<TypeList<Ts...>, index - 1>::type;
};
#pragma endregion

#pragma region IndexOf
template<typename TL, typename T>
struct IndexOf;

template<typename T>
struct IndexOf<TypeList<>, T>
{
	static constexpr int32 value = -1;
};

template<typename T, typename... Ts>
struct IndexOf<TypeList<T, Ts...>, T>
{
	static constexpr int32 value = 0;
};

template<typename U, typename... Ts, typename T>
struct IndexOf<TypeList<U, Ts...>, T>
{
private:
	static constexpr int32 temp = IndexOf<TypeList<Ts...>, T>::value;
public:
	static constexpr int32 value = (temp == -1) ? -1 : 1 + temp;
};
#pragma endregion

#pragma region Conversion
template<typename From, typename To>
struct Conversion
{
private:
	using Yes = char;
	using No = struct { char dummy[2]; };

	static Yes Test(const To&);
	static No Test(...);
	static From MakeFrom();
public:
	static constexpr bool exists = sizeof(Test(MakeFrom())) == sizeof(Yes);
	static constexpr bool sameType = false;
};

template<typename T>
struct Conversion<T, T>
{
	static constexpr bool exists = true;
	static constexpr bool sameType = true;
};
#pragma endregion

#pragma region TypeCast
template<int32 i>
struct Int2Type 
{
	static constexpr int32 value = i;
};

template<typename TL>
class TypeConversion
{
public:
	static constexpr int32 length = Length<TL>::value;

	TypeConversion()
	{
		MakeTable(Int2Type<0>(), Int2Type<0>());
	}

	template<int32 i, int32 j>
	static void MakeTable(Int2Type<i>, Int2Type<j>)
	{
		using From = typename TypeAt<TL, i>::type;
		using To = typename TypeAt<TL, j>::type;

		sConversionTable[i][j] = Conversion<const From*, const To*>::exists;
		MakeTable(Int2Type<i>(), Int2Type<j + 1>());
	}

	template<int32 i>
	static void MakeTable(Int2Type<i>, Int2Type<length>)
	{
		MakeTable(Int2Type<i + 1>(), Int2Type<0>());
	}

	static void MakeTable(Int2Type<length>, Int2Type<0>) {}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion sInstance;
		return sConversionTable[from][to];
	}

public:
	static bool sConversionTable[length][length];
};

template<typename TL>
bool TypeConversion<TL>::sConversionTable[length][length] = {};

template<typename ToPointer, typename From>
ToPointer TypeCast(From* ptr)
{
	if (ptr == nullptr)
	{
		return nullptr;
	}

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, std::remove_pointer_t<ToPointer>>::value))
	{
		return static_cast<ToPointer>(ptr);
	}

	return nullptr;
}

template<typename To, typename From>
std::shared_ptr<To> TypeCast(std::shared_ptr<From> ptr)
{
	if (ptr == nullptr)
	{
		return nullptr;
	}

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, To>::value))
	{
		return std::static_pointer_cast<To>(ptr);
	}

	return nullptr;
}

template<typename ToPointer, typename From>
bool CanCast(From* ptr)
{
	if (ptr == nullptr)
	{
		return false;
	}

	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, std::remove_pointer_t<ToPointer>>::value);
}

template<typename To, typename From>
bool CanCast(std::shared_ptr<From> ptr)
{
	if (ptr == nullptr)
	{
		return false;
	}

	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, To>::value);
}
#pragma endregion

#define DECLARE_TYPELIST(TypeList)		using TL = TypeList; int32 _typeId;
#define INIT_TYPEID(Type)				_typeId = IndexOf<TL, Type>::value;
