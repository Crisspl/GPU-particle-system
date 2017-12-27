#define FHL_PLATFORM_WINDOWS
#include "gl/flextGL.h"

#include "maths/vectors.h"

#include <SDL.h>
#include <vector>

const char * const CS_SRC = "\
#version 430\n\
layout(local_size_x = 64) in;\n\
layout(std140, binding = 0) restrict writeonly buffer Pos {\n\
	vec4 position[];\n\
};\n\
layout(std140, binding = 1) restrict buffer Vel {\n\
	vec4 velocity[];\n\
};\n\
layout(location = 0) uniform float dt;\n\
\n\
void main() {\n\
	const uint idx = gl_GlobalInvocationID.x;\n\
	velocity[idx] *= 1 - .9f * dt;\n\
	position[idx] += velocity[idx] * dt;\n\
	\n\
}\
";

const char * const VS_SRC = "\
#version 430\n\
layout(std140, binding = 0) restrict readonly buffer Pos {\n\
	vec4 position[];\n\
};\n\
layout(std140, binding = 1) restrict readonly buffer Vel {\n\
	vec4 velocity[];\n\
};\n\
out vec3 vs_color;\n\
const vec3 LO_COLOR = vec3(0xe6, 0xf3, 0xff), HI_COLOR = vec3(0xff, 0, 0x66);\n\
\n\
void main() {\n\
	const uint idx = gl_VertexID;\n\
	vs_color = mix(LO_COLOR, HI_COLOR, smoothstep(0.f, 100.f, length(velocity[idx].xyz)));\n\
	gl_Position = vec4(position[idx].xyz, 1.f);\n\
}\
";

const char * const FS_SRC = "\
#version 430\n\
in vec3 vs_color;\n\
out vec4 color;\n\
\n\
void main() {\n\
	color = vec4(vs_color, 1.f);\n\
}\
";

enum Bindings { PositionBuffer = 0, VelocityBuffer = 1 };
enum UniformLoc { DeltaTime = 0 };

GLuint makeCs(const char * const _src);
GLuint makeGeneralShader(const char * const _vs, const char * const _fs);

int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	auto window = SDL_CreateWindow(
		"EndlessRunner",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800,
		600,
		SDL_WINDOW_OPENGL
	);
	auto context = SDL_GL_CreateContext(window);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

	//SDL_GL_SetSwapInterval(1);

	// load GL functions
	flextGLInit();

	const std::size_t PARTICLE_CNT = 1u << 21;

	std::vector<fhl::Vec4f> positions(PARTICLE_CNT, fhl::Vec4f::zero());
	std::vector<fhl::Vec4f> velocities(PARTICLE_CNT, fhl::Vec4f::zero());

	GLuint posBuffer{}, velBuffer{};
	glGenBuffers(1, &posBuffer);
	glGenBuffers(1, &velBuffer);

	glNamedBufferStorage(posBuffer, positions.size() * sizeof(fhl::Vec4f), positions.data(), GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(velBuffer, velocities.size() * sizeof(fhl::Vec4f), velocities.data(), GL_DYNAMIC_STORAGE_BIT);

	GLuint cs = makeCs(CS_SRC);
	GLuint shader = makeGeneralShader(VS_SRC, FS_SRC);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Bindings::PositionBuffer, posBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Bindings::VelocityBuffer, velBuffer);

	float dt{};
	while (1)
	{
		glUseProgram(cs);
		glUniform1f(UniformLoc::DeltaTime, dt);
		glDispatchCompute(1u << 15, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glUseProgram(shader);
		// create attrib arrays? dummy vertex attrib? see https://stackoverflow.com/questions/8039929/opengl-drawarrays-without-binding-vbo
		glDrawArrays(GL_POINTS, 0, PARTICLE_CNT);
	}
}

GLuint makeCs(const char * const _src)
{
	GLuint program = glCreateProgram();
	GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(cs, 1, &_src, nullptr);
	glCompileShader(cs);

	{ // check for compilation errors
		GLint success{};
		GLchar infoLog[0x200];
		glGetShaderiv(cs, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(cs, 0x200, nullptr, infoLog);
			std::printf("CS COMPILATION ERROR: %s", infoLog);
			return 0;
		}
	}
	glAttachShader(program, cs);
	glLinkProgram(program);

	return program;
}

GLuint makeGeneralShader(const char * const _vs, const char * const _fs)
{
	GLuint program = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &_vs, nullptr);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &_fs, nullptr);
	glCompileShader(fs);

	GLchar infoLog[0x200];
	for (GLuint s : {vs, fs})
	{ // check for compilation errors
		GLint success{};
		glGetShaderiv(s, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(s, 0x200, nullptr, infoLog);
			std::printf("VS/FS COMPILATION ERROR: %s", infoLog);
			return 0;
		}
	}
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	return program;
}