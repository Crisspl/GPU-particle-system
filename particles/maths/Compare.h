#ifndef FHL_UTILITY_COMPARE_H
#define FHL_UTILITY_COMPARE_H

#include <limits>
#include <type_traits>
#include <cmath>

namespace fhl
{

	namespace detail
	{
		template<typename _T>
		struct FloatingPointCompare
		{
			static bool equal(const _T & _a, const _T & _b)
			{
				/*thanks goes to: http://floating-point-gui.de/errors/comparison/ */
				if (_a == _b) return true;

				const _T absDiff = std::abs(_a - _b);

				if (_a == _T(0) || _b == _T(0) || absDiff < std::numeric_limits<_T>::epsilon())
					return absDiff < std::numeric_limits<_T>::epsilon();

				return absDiff / (std::abs(_a) + std::abs(_b)) < std::numeric_limits<_T>::epsilon();
			}
		};

		template<typename T>
		struct IntegerCompare
		{
			constexpr static bool equal(const T & a, const T & b) { return a == b; }
		};
	}


	namespace
	{
		template<typename T>
		using Parent = typename std::conditional<std::is_floating_point<T>::value, detail::FloatingPointCompare<T>, detail::IntegerCompare<T>>::type;
	}

	template<typename _T>
	struct Compare : Parent<_T>
	{
		static bool equal(const _T & a, const _T & b)
		{
			return Parent<_T>::equal(a, b);
		}

		static bool notEqual(const _T & a, const _T & b)
		{
			return !equal(a, b);
		}

		constexpr static bool less(const _T & a, const _T & b)
		{
			return a < b;
		}

		constexpr static bool greater(const _T & a, const _T & b)
		{
			return a > b;
		}

		static bool lessEqual(const _T & a, const _T & b)
		{
			return less(a, b) || equal(a, b);
		}

		static bool greaterEqual(const _T & a, const _T & b)
		{
			return greater(a, b) || equal(a, b);
		}
	};


	template<typename T>
	struct EqualTo
	{
		bool operator()(const T & a, const T & b) { return Compare<T>::equal(a, b); }
	};

	template<typename T>
	struct NotEqualTo
	{
		bool operator()(const T & a, const T & b) { return Compare<T>::notEqual(a, b); }
	};

	template<typename T>
	struct Less
	{
		bool operator()(const T & a, const T & b) { return Compare<T>::less(a, b); }
	};

	template<typename T>
	struct Greater
	{
		bool operator()(const T & a, const T & b) { return Compare<T>::greater(a, b); }
	};

	template<typename T>
	struct LessEqual
	{
		bool operator()(const T & a, const T & b) { return Compare<T>::lessEqual(a, b); }
	};

	template<typename T>
	struct GreaterEqual
	{
		bool operator()(const T & a, const T & b) { return Compare<T>::greaterEqual(a, b); }
	};

}

#endif
