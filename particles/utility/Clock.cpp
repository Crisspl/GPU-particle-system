#include "Clock.h"

namespace fhl
{

	 Clock::Clock()
		 : m_start(InnerClock::now())
	 {

	 }

	 template <typename T>
	 float Clock::getElapsedTime() const
	 {
		 T elapsedTime = InnerClock::now() - m_start;
		 return elapsedTime.count();
	 }

	 template <typename T>
	 float Clock::restart()
	 {
		 float elapsedTime = getElapsedTime<T>();
		 m_start = InnerClock::now();
		 return elapsedTime;
	 }

	 template float Clock::getElapsedTime<Seconds>() const;
	 template float Clock::getElapsedTime<Milliseconds>() const;
	 template float Clock::getElapsedTime<Microseconds>() const;

	 template float Clock::restart<Seconds>();
	 template float Clock::restart<Milliseconds>();
	 template float Clock::restart<Microseconds>();

}
