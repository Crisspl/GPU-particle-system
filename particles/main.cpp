#include "gl/flextGL.h"

#include "maths/vectors.h"
#include "maths/Mat4.h"
#include "tga/tga.h"
#include "utility/Clock.h"
#include "CameraController.h"

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
layout(location = 1) uniform bool attractorActive;\n\
layout(location = 2) uniform vec3 attractorPosition;\n\
\n\
void main() {\n\
	const uint idx = gl_GlobalInvocationID.x;\n\
	velocity[idx] *= 1 - .99f * dt;\n\
	if (attractorActive) {\n\
		float dist = distance(attractorPosition, position[idx].xyz);\n\
		float acc = 10000.f / max(1.f, 0.01f * pow(dist, 1.5f));\n\
		velocity[idx] += vec4(normalize(attractorPosition - position[idx].xyz), 0.f) * acc * dt;\n\
	}\n\
	position[idx] += velocity[idx] * dt;\n\
}\
";

const char * const VS_SRC = "\
#version 430 core\n\
layout(location = 0) in vec4 position;\n\
layout(location = 1) in vec4 velocity;\n\
layout(location = 0) uniform mat4 view;\n\
out vec3 vs_color;\n\
const vec3 LO_COLOR = vec3(0xb3, 0xd9, 0xff)/255.f, HI_COLOR = vec3(0xff, 0, 0x66)/255.f;\n\
\n\
void main() {\n\
	vs_color = mix(LO_COLOR, HI_COLOR, smoothstep(0.f, 700.f, length(velocity.xyz)));\n\
	gl_Position = view * vec4(position.xyz, 1.f);\n\
}\
";

const char * const GS_SRC = "\
#version 430 core\n\
layout(points) in;\n\
layout(triangle_strip, max_vertices = 4) out;\n\
\n\
in vec3 vs_color[];\n\
out vec3 fs_color;\n\
out vec2 fs_txCoords;\n\
\n\
layout(location = 1) uniform mat4 projection;\n\
const vec2 offsets[4] = {\n\
	vec2(0.f, 0.f), vec2(1.f, 0.f), vec2(0.f, 1.f), vec2(1.f, 1.f) };\n\
\n\
void main() {\n\
	fs_color = vs_color[0];\n\
	for (int i = 0; i < 4; ++i) {\n\
		fs_txCoords = offsets[i];\n\
		vec4 pos = gl_in[0].gl_Position;\n\
		pos.xy += .5f * (offsets[i] - vec2(0.5f));\n\
		gl_Position = projection * pos;\n\
		EmitVertex();\n\
	}\n\
}\
";

const char * const FS_SRC = "\
#version 430 core\n\
in vec3 fs_color;\n\
in vec2 fs_txCoords;\n\
out vec4 color;\n\
layout(location = 2) uniform sampler2D txSampler;\n\
\n\
void main() {\n\
	color = texture(txSampler, fs_txCoords) * vec4(fs_color, 1.f);\n\
}\
";

const char * const PARTICLE_TEXTURE_PATH = "particle.tga";

enum Bindings { PositionBuffer = 0, VelocityBuffer = 1 };
enum CsUniformLoc { DeltaTime = 0, AttractorActive = 1, AttractorPosition = 2 };
enum GeneralShaderUniformLoc { View = 0, Projection = 1, Texture = 2 };
enum AttrLoc { Position = 0, Velocity = 1 };

GLuint makeCs(const char * const _src);
GLuint makeGeneralShader(const char * const _vs, const char * const _gs, const char * const _fs);
GLuint loadTexture(const char * const _path);

void checkErrors();

int main(int, char**)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const fhl::Vec2u WIN_SIZE{1280, 720u};
	auto window = glfwCreateWindow(WIN_SIZE.x(), WIN_SIZE.y(), "particles", NULL, NULL);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// load GL functions
	flextGLInit();

	std::srand(997);

	const std::size_t PARTICLE_CNT = 1u << 21; // ~2M
	const float GRAVITY_POINT_DISTANCE_FROM_CAM = 800.f;

	std::vector<fhl::Vec4f> positions;
	positions.reserve(PARTICLE_CNT);
	for (float x = 0; x < (1u << 7); ++x)
		for (float y = 0; y < (1u << 7); ++y)
			for (float z = 0; -z < (1u << 7); --z)
				positions.push_back(fhl::Vec4f{x, y, z, 1.f});
	std::vector<fhl::Vec4f> velocities(PARTICLE_CNT, fhl::Vec4f::zero());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);

	GLuint posBuffer{}, velBuffer{};
	glCreateBuffers(1, &posBuffer);
	glCreateBuffers(1, &velBuffer);

	glNamedBufferStorage(posBuffer, positions.size() * sizeof(fhl::Vec4f), positions.data(), GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(velBuffer, velocities.size() * sizeof(fhl::Vec4f), velocities.data(), GL_DYNAMIC_STORAGE_BIT);
	positions.clear();
	velocities.clear();

	GLuint cs = makeCs(CS_SRC);
	GLuint shader = makeGeneralShader(VS_SRC, GS_SRC, FS_SRC);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Bindings::PositionBuffer, posBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Bindings::VelocityBuffer, velBuffer);

	GLuint vao{};
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glVertexAttribPointer(AttrLoc::Position, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(fhl::Vec4f::valueType), (void *)0);
	glEnableVertexAttribArray(AttrLoc::Position);
	glBindBuffer(GL_ARRAY_BUFFER, velBuffer);
	glVertexAttribPointer(AttrLoc::Velocity, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(fhl::Vec4f::valueType), (void *)0);
	glEnableVertexAttribArray(AttrLoc::Velocity);

	GLuint particleTex = loadTexture(PARTICLE_TEXTURE_PATH);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particleTex);

	const fhl::Mat4f projection = fhl::Mat4f::perspective(45.f, -float(WIN_SIZE.x()) / WIN_SIZE.y(), 0, 1e3f);
	
	//Camera cam{fhl::Vec3f{828.f, 328.f, 350.f}};
	Camera cam{fhl::Vec3f{64.f, 64.f, GRAVITY_POINT_DISTANCE_FROM_CAM + 64.f}};
	CameraController camController({&cam});
	camController.setTranslationSpeed(10.f);

	std::map<int, int> keyStates;
	fhl::Clock clock;
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		const float dt = clock.restart();
		bool mblPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		const fhl::Vec3f attractorPosition = cam.getPosition() + -cam.getDirectionVector() * GRAVITY_POINT_DISTANCE_FROM_CAM * .75f;

		glUseProgram(cs);
		glUniform1f(CsUniformLoc::DeltaTime, dt);
		glUniform1i(CsUniformLoc::AttractorActive, mblPressed);
		glUniform3fv(CsUniformLoc::AttractorPosition, 1, attractorPosition.data());
		glDispatchCompute(PARTICLE_CNT >> 6, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glUseProgram(shader);
		glUniformMatrix4fv(GeneralShaderUniformLoc::View, 1, GL_FALSE, cam.getView().data());
		glUniformMatrix4fv(GeneralShaderUniformLoc::Projection, 1, GL_FALSE, projection.data());
		glUniform1i(GeneralShaderUniformLoc::Texture, 0);

		fhl::Vec2lf currentMouse;
		glfwGetCursorPos(window, &currentMouse.x(), &currentMouse.y());
		camController.processMousePosition(currentMouse);

		for (int k : {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D})
			keyStates[k] = glfwGetKey(window, k);
		camController.processKeyStates(keyStates);
		camController.updateAll();

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

	glDetachShader(program, cs);
	glDeleteShader(cs);

	GLint success;
	GLchar infoLog[0x200];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 0x200, nullptr, infoLog);
		printf("CS LINK ERROR: %s\n", infoLog);
	}

	return program;
}

GLuint makeGeneralShader(const char * const _vs, const char * const _gs, const char * const _fs)
{
	GLuint program = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &_vs, nullptr);
	glCompileShader(vs);
	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gs, 1, &_gs, nullptr);
	glCompileShader(gs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &_fs, nullptr);
	glCompileShader(fs);

	GLchar infoLog[0x200];
	for (GLuint s : {vs, gs, fs})
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
	glAttachShader(program, gs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	glDetachShader(program, vs);
	glDeleteShader(vs);
	glDetachShader(program, gs);
	glDeleteShader(gs);
	glDetachShader(program, fs);
	glDeleteShader(fs);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 0x200, nullptr, infoLog);
		printf("VS/FS LINK ERROR: %s\n", infoLog);
	}

	return program;
}

GLuint loadTexture(const char * const _path)
{
	tTGA  tga;
	if (!load_TGA(&tga, _path)) 
		throw std::runtime_error("Could not load particle texture!");
	GLuint tex{};
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		(tga.alpha) ? (GL_RGB) : (GL_RGB8),
		tga.width,
		tga.height,
		0,
		(tga.alpha) ? (GL_RGBA) : (GL_RGB),
		GL_UNSIGNED_BYTE,
		tga.data
	);
	free_TGA(&tga);
	return tex;
}

void checkErrors()
{
	GLenum err{};
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