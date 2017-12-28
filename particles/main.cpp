#include "gl/flextGL.h"

#include "maths/vectors.h"
#include "maths/Mat4.h"
#include "utility/Clock.h"

#include <SDL.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <random>

const char * const CS_SRC = "\
#version 430 core\n\
layout(local_size_x = 64) in;\n\
layout(std140, binding = 0) restrict buffer Pos {\n\
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
}\
";

const char * const VS_SRC = "\
#version 430 core\n\
layout(location = 0) in vec4 position;\n\
layout(location = 1) in vec4 velocity;\n\
layout(location = 0) uniform mat4 pv;\n\
out vec3 vs_color;\n\
const vec3 LO_COLOR = vec3(0xb3, 0xd9, 0xff)/255.f, HI_COLOR = vec3(0xff, 0, 0x66)/255.f;\n\
\n\
void main() {\n\
	vs_color = mix(LO_COLOR, HI_COLOR, smoothstep(0.f, 100.f, length(velocity.xyz)));\n\
	gl_Position = pv * vec4(position.xyz, 1.f);\n\
}\
";

const char * const FS_SRC = "\
#version 430 core\n\
in vec3 vs_color;\n\
out vec4 color;\n\
\n\
void main() {\n\
	color = vec4(vs_color, 1.f);\n\
}\
";

enum Bindings { PositionBuffer = 0, VelocityBuffer = 1 };
enum UniformLoc { DeltaTime = 0, ProjectionView = 0 };
enum AttrLoc { Position = 0, Velocity = 1 };

GLuint makeCs(const char * const _src);
GLuint makeGeneralShader(const char * const _vs, const char * const _fs);

void checkErrors()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		switch (err)
		{
		case GL_INVALID_ENUM: printf("err invalid enum\n"); break;
		case GL_INVALID_VALUE: printf("err invalid val\n"); break;
		case GL_INVALID_OPERATION: printf("err invalid op\n"); break;
		case GL_STACK_OVERFLOW: printf("err stack overflow\n"); break;
		case GL_STACK_UNDERFLOW: printf("err stack underflow\n"); break;
		case GL_OUT_OF_MEMORY: printf("err out of mem\n"); break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: printf("err invalid fb op\n"); break;
		case GL_CONTEXT_LOST: printf("err ctx lost\n"); break;
		}
	}
}

int main(int, char**)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(800, 600, "particles", NULL, NULL);
	glfwMakeContextCurrent(window);

	// load GL functions
	flextGLInit();

	std::srand(112);

	const std::size_t PARTICLE_CNT = 1u << 21; // ~2M
	const fhl::Vec2u WIN_SIZE{800u, 600u};

	std::vector<fhl::Vec4f> positions;
	positions.reserve(PARTICLE_CNT);
	for (float x = 0; x < (1u << 7); ++x)
	{
		for (float y = 0; y < (1u << 7); ++y)
		{
			for (float z = 0; z < (1u << 7); ++z)
				positions.push_back(fhl::Vec4f{ x, y, z, 1.f } + fhl::Vec4f{WIN_SIZE/2, fhl::Vec2f::zero()});
		}
	}
	std::vector<fhl::Vec4f> velocities;
	velocities.reserve(PARTICLE_CNT);
	for (std::size_t i = 0; i < PARTICLE_CNT; ++i)
		velocities.emplace_back(std::rand() % 200, std::rand() % 200, std::rand() % 200, 0);

	GLuint posBuffer{}, velBuffer{};
	glCreateBuffers(1, &posBuffer);
	glCreateBuffers(1, &velBuffer);

	glNamedBufferStorage(posBuffer, positions.size() * sizeof(fhl::Vec4f), positions.data(), GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(velBuffer, velocities.size() * sizeof(fhl::Vec4f), velocities.data(), GL_DYNAMIC_STORAGE_BIT);

	GLuint cs = makeCs(CS_SRC);
	GLuint shader = makeGeneralShader(VS_SRC, FS_SRC);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Bindings::PositionBuffer, posBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Bindings::VelocityBuffer, velBuffer);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glVertexAttribPointer(AttrLoc::Position, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(fhl::Vec4f::valueType), (void *)0);
	glEnableVertexAttribArray(AttrLoc::Position);
	glBindBuffer(GL_ARRAY_BUFFER, velBuffer);
	glVertexAttribPointer(AttrLoc::Velocity, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(fhl::Vec4f::valueType), (void *)0);
	glEnableVertexAttribArray(AttrLoc::Velocity);

	const fhl::Mat4f pv = fhl::Mat4f::perspective(45.f, -float(WIN_SIZE.x())/WIN_SIZE.y(), 0, 1e3f) * fhl::Mat4f::lookAt(fhl::Vec3f{828.f, 328.f, 350.f}, fhl::Vec3f{464.f, 364.f, 64.f}, fhl::Vec3f::up());
	
	fhl::Clock clock;
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		float dt = clock.restart();
		glUseProgram(cs);
		glUniform1f(UniformLoc::DeltaTime, dt);
		glDispatchCompute(PARTICLE_CNT >> 6, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glUseProgram(shader);
		glUniformMatrix4fv(UniformLoc::ProjectionView, 1, GL_FALSE, pv.data());
		// create attrib arrays? dummy vertex attrib? see https://stackoverflow.com/questions/8039929/opengl-drawarrays-without-binding-vbo
		glDrawArrays(GL_POINTS, 0, PARTICLE_CNT);

		checkErrors();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
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
			std::printf("CS COMPILATION ERROR: %s\n", infoLog);
			return 0;
		}
	}
	glAttachShader(program, cs);
	glLinkProgram(program);

	GLint success;
	GLchar infoLog[0x200];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 0x200, nullptr, infoLog);
		printf("CS LINK ERROR: %s\n", infoLog);
	}

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

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 0x200, nullptr, infoLog);
		printf("VS/FS LINK ERROR: %s\n", infoLog);
	}

	return program;
}