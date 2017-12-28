#ifndef FHL_UTILITY_CLOCK_H
#define FHL_UTILITY_CLOCK_H

#include <chrono>
#include <ratio>

namespace fhl
{

	 using Seconds = std::chrono::duration<float>;
	 using Milliseconds = std::chrono::duration<float, std::milli>;
	 using Microseconds = std::chrono::duration<float, std::micro>;

	 class Clock
	 {
		  using InnerClock = std::chrono::high_resolution_clock;
		  using TimePoint = std::chrono::time_point<InnerClock>;

	 public:
		  Clock();

		  template<typename T = Seconds>
		  float getElapsedTime() const;

		  template<typename T = Seconds>
		  float restart();

	 private:
		  TimePoint m_start;
	 };

}

#endif
