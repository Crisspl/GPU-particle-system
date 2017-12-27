#ifndef FHL_GL_OPENGL_LOADER
#define FHL_GL_OPENGL_LOADER

#ifdef FHL_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN 1
	#include <windows.h>
#endif

namespace fhl { namespace impl {

	class OpenGlLoader
	{
	public:
#if defined(FHL_PLATFORM_WINDOWS)
		using fptr_t = PROC;
#else 
		using fptr_t = void(*)();
#endif
		OpenGlLoader();
		~OpenGlLoader();

		fptr_t load(const char * _name);

	private:
#if defined(FHL_PLATFORM_WINDOWS)
		HMODULE m_gllib;
#else
		void * m_gllib;
#endif
	};

}}

#endif
