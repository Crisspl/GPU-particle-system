#include "OpenGlLoader.h"

#include <stdexcept>
#include <cstddef>

#if defined(FHL_PLATFORM_WINDOWS)
	// windows.h included in header
	#if defined(_MSC_VER)
		#pragma warning(disable: 4055)
		#pragma warning(disable: 4054)
	#endif
#elif defined(FHL_PLATFORM_APPLE)
	#include <dlfcn.h>
#else
	#include <dlfcn.h>
	#include <GL/glx.h>
#endif

namespace fhl { namespace impl {

#if defined(FHL_PLATFORM_WINDOWS)
	OpenGlLoader::OpenGlLoader()
	{
		m_gllib = LoadLibraryA("opengl32.dll");
		if (!m_gllib)
			throw std::runtime_error{ "FHL: Failed loading OpenGL" };
	}

	OpenGlLoader::~OpenGlLoader()
	{
		FreeLibrary(m_gllib);
	}

	auto OpenGlLoader::load(const char * _name) -> fptr_t
	{
		const fptr_t address = wglGetProcAddress(reinterpret_cast<LPCSTR>(_name));
		const auto integerAddress = reinterpret_cast<std::ptrdiff_t>(address);

		if (address && integerAddress != 1 && integerAddress != 2 && integerAddress != 3 && integerAddress != -1)
			return address;
		return GetProcAddress(m_gllib, reinterpret_cast<LPCSTR>(_name));
	}
#elif defined(FHL_PLATFORM_APPLE)
	OpenGlLoader::OpenGlLoader()
	{
		m_gllib = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY);
		if (!m_gllib)
			throw std::runtime_error{ "FHL: Failed loading OpenGL" };
	}

	OpenGlLoader::~OpenGlLoader()
	{
		dlclose(m_gllib);
	}

	auto OpenGlLoader::load(const char * _name) -> fptr_t
	{
		return reinterpret_cast<fptr_t>(dlsym(m_gllib, _name));
	}
#else
	OpenGlLoader::OpenGlLoader()
	{
		m_gllib = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY);
		if (!m_gllib)
			throw std::runtime_error{ "FHL: Failed loading OpenGL" };
	}

	OpenGlLoader::~OpenGlLoader()
	{
		dlclose(m_gllib);
	}

	auto OpenGlLoader::load(const char * _name) -> fptr_t
	{
		static PFNGLXGETPROCADDRESSPROC glx_get_proc_address = reinterpret_cast<PFNGLXGETPROCADDRESSPROC>(dlsym(m_gllib, "glXGetProcAddressARB"));

		return reinterpret_cast<fptr_t>(glx_get_proc_address(reinterpret_cast<const GLubyte *>(_name)));
	}
#endif

}}
