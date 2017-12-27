#ifndef FHL_MATHS_BOOL_VEC_H
#define FHL_MATHS_BOOL_VEC_H

#include <type_traits>
#include <utility>

namespace fhl
{

	template<std::size_t _N>
	class BoolVec
	{
		static_assert(_N > 0, "fhl::BoolVec cannot be of size 0");
		enum
		{
			BytesCount = (_N - 1) / 8 + 1
		};

		using ubyte = unsigned char;
		using byte = char;

	public:
		enum { Dimensions = _N };

		constexpr BoolVec() : BoolVec(std::make_index_sequence<BytesCount>{}) {}

	private:
		template<std::size_t ...Is> /* helper ctor to zero-init all bytes */
		constexpr BoolVec(std::index_sequence<Is...>) : m_data{(false && Is)...} {}

	public:
		template<typename ...Args>
		BoolVec(Args... _args) : BoolVec()
		{
			static_assert(sizeof...(Args) == _N, "Cannot initialize more than `Dimensions` bits");
			initBits(bool(_args)...);
		}

		constexpr operator bool() const { return all(); }

		constexpr bool operator[](std::size_t _idx) const
		{
			return static_cast<bool>( (m_data[_idx / 8] >> (_idx % 8)) & 1u );
		}

		BoolVec & set(std::size_t _i, bool _val)
		{
			m_data[_i / 8] ^= (-byte(_val) ^ m_data[_i / 8]) & (1u << (_i % 8));
			return *this;
		}

		bool all() const
		{
			for (std::size_t i = 0u; i < _N; i++)
				if (!(*this)[i]) return false;
			return true;
		}

		bool none() const
		{
			for (std::size_t i = 0u; i < _N; i++)
				if ((*this)[i]) return false;
			return true;
		}

		bool any() const { return !none(); }

		constexpr BoolVec<_N> operator~() const
		{
			return negated(std::make_index_sequence<_N>{});
		}

	private:
		template<std::size_t ...Is>
		constexpr BoolVec<_N> negated(std::index_sequence<Is...>) const
		{
			return BoolVec<_N>(!(*this)[Is]...);
		}

		template<typename ...Rest>
		void initBits(bool _first, Rest... _rest)
		{
			static_assert(sizeof...(Rest) < _N, "fhl::BoolVec: Cannot initialize more then `Dimensions` bits");

			set(_N - sizeof...(Rest) - 1, _first);
			initBits(bool(_rest)...);
		}
		void initBits(bool _val)
		{
			set(_N - 1, _val);
		}

	private:
		ubyte m_data[BytesCount];
	};

	template<std::size_t N>
	std::ostream & operator<<(std::ostream & _out, BoolVec<N> _v)
	{
		_out << "{ ";
		for (std::size_t i = 0; i < N; i++)
			_out << _v[i] << (i < N - 1 ? ", " : " }");
		return _out;
	}

}

#endif
