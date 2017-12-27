#ifndef FHL_MATHS_CONSTANTS_H
#define FHL_MATHS_COSNTANTS_H

namespace fhl
{

	template<typename T>
	struct Constants
	{
		Constants() = delete;

		constexpr static T Pi() { return T(3.141592653589793238); }

		constexpr static T e() { return T(2.718281828459045235); }

		constexpr static T sqrt2() { return T(1.414213562373095048); }

		constexpr static T sqrt3() { return T(1.732050807568877293); }
	};

}

#endif
