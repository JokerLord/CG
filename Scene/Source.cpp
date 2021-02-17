#define GLEW_STATIC
#include <GL\glew.h>

#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void mouse_callback(GLFWwindow* window, double xposition, double yposition);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool compare_func(const glm::vec3& a, const glm::vec3& b);
void render_wall();
void set_object_parameters(Shader& object_shader);
void set_wall_parameters(Shader& wall_shader);

const unsigned int WIDTH = 800, HEIGHT = 600;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

const unsigned int NUM_OF_CUBES = 2;
const unsigned int NUM_OF_BILLBOARDS = 3;

float delta_time = 0.0f;
float last_frame = 0.0f;

float yaw = -90.0f;
float pitch =   0.0f;
float last_x = WIDTH / 2.0f;
float last_y = HEIGHT / 2.0f;
bool is_first_mouse = true;

float fov = 45.0f;

bool is_inversed = false;
bool is_inverse_key_pressed = false;

unsigned int wall_VAO = 0;
unsigned int wall_VBO = 0;

glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_direction = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 point_light_pos(0.0f, 0.0f, -3.0f);
glm::vec3 dir_light_pos(-2.0f, 4.0f, -1.0f);
glm::vec3 wall_position(-6.0f, 1.0f, -6.0f);

int main()
{
	glfwInit();
	GLFWwindow* win = glfwCreateWindow(WIDTH, HEIGHT, "Scene", nullptr, nullptr);
	glfwMakeContextCurrent(win);
	glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
	glfwSetCursorPosCallback(win, mouse_callback);
	glfwSetScrollCallback(win, scroll_callback);
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader object_shader("shaders/object_vs.txt", "shaders/object_fs.txt");
	Shader light_cube_shader("shaders/light_cube_vs.txt", "shaders/light_cube_fs.txt");
	Shader dir_light_source_shader("shaders/dir_light_cube_vs.txt", "shaders/dir_light_cube_fs.txt");
	Shader billboard_shader("shaders/billboard_vs.txt", "shaders/billboard_fs.txt");
	Shader screen_shader("shaders/screen_vs.txt", "shaders/screen_fs.txt");
	Shader depth_map_shader("shaders/depth_map_vs.txt", "shaders/depth_map_fs.txt");
	Shader wall_shader("shaders/wall_vs.txt", "shaders/wall_fs.txt");

	float cube_vertices[] = {
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f
	};

	float plane_vertices[] = {
		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
	};

	float billboard_vertices[] = {
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	float rectangle_vertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glm::vec3 cube_positions[NUM_OF_CUBES] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(-2.0f,  2.0f, -1.0f),
	};

	std::vector<glm::vec3> billboard_positions = {
		glm::vec3(-0.5f, 0.0f, 0.51f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, -0.5f),
	};

	unsigned int cube_VAO, cube_VBO;
	glGenVertexArrays(1, &cube_VAO);
	glGenBuffers(1, &cube_VBO);
	glBindVertexArray(cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	unsigned int light_cube_VAO;
	glGenVertexArrays(1, &light_cube_VAO);
	glBindVertexArray(light_cube_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int billboard_VAO, billboard_VBO;
	glGenVertexArrays(1, &billboard_VAO);
	glGenBuffers(1, &billboard_VBO);
	glBindVertexArray(billboard_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(billboard_vertices), &billboard_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int plane_VAO, plane_VBO;
	glGenVertexArrays(1, &plane_VAO);
	glGenBuffers(1, &plane_VBO);
	glBindVertexArray(plane_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, plane_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int rectangle_VAO, rectangle_VBO;
	glGenVertexArrays(1, &rectangle_VAO);
	glGenBuffers(1, &rectangle_VBO);
	glBindVertexArray(rectangle_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), &rectangle_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);


	unsigned int object_texture;
	glGenTextures(1, &object_texture);
	int width, height, comp;
	unsigned char* image = stbi_load("textures/object_texture.jpg", &width, &height, &comp, 0);
	if (image) {
		glBindTexture(GL_TEXTURE_2D, object_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Object texture loading failed:(" << std::endl;
	}
	stbi_image_free(image);

	unsigned int object_specular_map;
	glGenTextures(1, &object_specular_map);
	image = stbi_load("textures/object_texture_specular.jpg", &width, &height, &comp, 0);
	if (image) {
		glBindTexture(GL_TEXTURE_2D, object_specular_map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Object specular map loading failed:)" << std::endl;
	}
	stbi_image_free(image);


	unsigned int billboard_texture;
	glGenTextures(1, &billboard_texture);
	image = stbi_load("textures/frost.png", &width, &height, &comp, 0);
	if (image) {
		glBindTexture(GL_TEXTURE_2D, billboard_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Billboard texture loading failed:)" << std::endl;
	}
	stbi_image_free(image);


	unsigned int wall_diffuse_map;
	glGenTextures(1, &wall_diffuse_map);
	image = stbi_load("textures/wall_diffuse_map.jpg", &width, &height, &comp, 0);
	if (image) {
		glBindTexture(GL_TEXTURE_2D, wall_diffuse_map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Wall texture loading failed:)" << std::endl;
	}
	stbi_image_free(image);

	unsigned int wall_specular_map;
	glGenTextures(1, &wall_specular_map);
	image = stbi_load("textures/wall_specular_map.jpg", &width, &height, &comp, 0);
	if (image) {
		glBindTexture(GL_TEXTURE_2D, wall_specular_map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Wall diffuse map loading failed:)" << std::endl;
	}
	stbi_image_free(image);

	unsigned int wall_normal_map;
	glGenTextures(1, &wall_normal_map);
	image = stbi_load("textures/wall_normal_map.jpg", &width, &height, &comp, 0);
	if (image) {
		glBindTexture(GL_TEXTURE_2D, wall_normal_map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Wall normal map loading failed:)" << std::endl;
	}
	stbi_image_free(image);

	unsigned int wall_disp_map;
	glGenTextures(1, &wall_disp_map);
	image = stbi_load("textures/wall_disp_map.jpg", &width, &height, &comp, 0);
	if (image) {
		glBindTexture(GL_TEXTURE_2D, wall_disp_map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Wall displacement map loading failed:)" << std::endl;
	}
	stbi_image_free(image);


	unsigned int inversion_FBO;
	glGenFramebuffers(1, &inversion_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, inversion_FBO);

	unsigned int inversion_color_buf;
	glGenTextures(1, &inversion_color_buf);
	glBindTexture(GL_TEXTURE_2D, inversion_color_buf);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inversion_color_buf, 0);

	unsigned int inversion_RBO;
	glGenRenderbuffers(1, &inversion_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, inversion_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, inversion_RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Ineverison framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	unsigned int depth_map_FBO;
	glGenFramebuffers(1, &depth_map_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_FBO);

	unsigned int depth_map;
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
 	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer for shadows is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	object_shader.use();
	object_shader.set_int("material.diffuse", 0);
	object_shader.set_int("material.specular", 1);
	object_shader.set_int("shadow_map", 2);


	billboard_shader.use();
	billboard_shader.set_int("texture", 0);

	wall_shader.use();
	wall_shader.set_int("material.diffuse", 0);
	wall_shader.set_int("material.specular", 1);
	wall_shader.set_int("material.normal_map", 2);
	wall_shader.set_int("material.disp_map", 3);
	wall_shader.set_int("shadow_map", 4);


	while (!glfwWindowShouldClose(win)) {
		float currentFrame = glfwGetTime();
		delta_time = currentFrame - last_frame;
		last_frame = currentFrame;

		process_input(win);

		std::sort(billboard_positions.begin(), billboard_positions.end(), compare_func);

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 light_view = glm::lookAt(dir_light_pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 light_space_mat = light_projection * light_view;
		depth_map_shader.use();
		depth_map_shader.set_mat4("light_space_mat", light_space_mat);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_FBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(cube_VAO);
		for (unsigned int i = 0; i < NUM_OF_CUBES; ++i) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cube_positions[i]);
			model = glm::scale(model, glm::vec3(0.5f));
			depth_map_shader.set_mat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(plane_VAO);
		glm::mat4 model = glm::mat4(1.0f);
		depth_map_shader.set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		model = glm::translate(model, wall_position);
		model = glm::rotate(model, glm::radians(-25.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		depth_map_shader.set_mat4("model", model);
		render_wall();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glViewport(0, 0, WIDTH, HEIGHT);

		if (is_inversed) {
			glBindFramebuffer(GL_FRAMEBUFFER, inversion_FBO);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		object_shader.use();
		set_object_parameters(object_shader);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(camera_position, camera_position + camera_direction, camera_up);
		object_shader.set_mat4("view", view);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		object_shader.set_mat4("projection", projection);

		object_shader.set_mat4("light_space_mat", light_space_mat);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, object_texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, object_specular_map);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depth_map);

		glBindVertexArray(cube_VAO);
		for (unsigned int i = 0; i < NUM_OF_CUBES; ++i) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cube_positions[i]);
			model = glm::scale(model, glm::vec3(0.5f));
			object_shader.set_mat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(plane_VAO);
		model = glm::mat4(1.0f);
		object_shader.set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		light_cube_shader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, point_light_pos);
		model = glm::scale(model, glm::vec3(0.2f));
		light_cube_shader.set_mat4("model", model);
		light_cube_shader.set_mat4("view", view);
		light_cube_shader.set_mat4("projection", projection);
		glBindVertexArray(light_cube_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		dir_light_source_shader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, dir_light_pos);
		model = glm::scale(model, glm::vec3(0.2f));
		light_cube_shader.set_mat4("model", model);
		light_cube_shader.set_mat4("view", view);
		light_cube_shader.set_mat4("projection", projection);
		glBindVertexArray(light_cube_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		wall_shader.use();
		set_wall_parameters(wall_shader);
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, wall_position);
		model = glm::rotate(model, glm::radians(-25.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
		wall_shader.set_mat4("model", model);
		wall_shader.set_mat4("view", view);
		wall_shader.set_mat4("projection", projection);
		wall_shader.set_mat4("light_space_mat", light_space_mat);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wall_diffuse_map);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wall_specular_map);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, wall_normal_map);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, wall_disp_map);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depth_map);
		render_wall();


		billboard_shader.use();
		billboard_shader.set_mat4("view", view);
		billboard_shader.set_mat4("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, billboard_texture);
		glBindVertexArray(billboard_VAO);
		for (auto it = billboard_positions.rbegin(); it != billboard_positions.rend(); ++it)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, *it);
			billboard_shader.set_mat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}


		if (is_inversed) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glClear(GL_COLOR_BUFFER_BIT);

			screen_shader.use();
			screen_shader.set_int("is_inversed", is_inversed);
			glBindVertexArray(rectangle_VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, inversion_color_buf);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cube_VAO);
	glDeleteVertexArrays(1, &light_cube_VAO);
	glDeleteVertexArrays(1, &billboard_VAO);
	glDeleteVertexArrays(1, &plane_VAO);
	glDeleteVertexArrays(1, &rectangle_VAO);

	glDeleteBuffers(1, &cube_VBO);
	glDeleteBuffers(1, &billboard_VBO);
	glDeleteBuffers(1, &plane_VBO);
	glDeleteBuffers(1, &rectangle_VBO);

	glDeleteFramebuffers(1, &inversion_FBO);
	glDeleteFramebuffers(1, &depth_map_FBO);

	glfwTerminate();
	return 0;
}

void set_object_parameters(Shader &object_shader)
{
	object_shader.set_vec3("view_pos", camera_position);

	object_shader.set_vec3("dir_light.position", dir_light_pos);
	object_shader.set_vec3("dir_light.ambient", 0.0f, 0.0f, 0.0f);
	object_shader.set_vec3("dir_light.diffuse", 0.2f, 0.2f, 0.2f);
	object_shader.set_vec3("dir_light.specular", 0.2f, 0.2f, 0.2f);

	object_shader.set_vec3("point_light.position", point_light_pos);
	object_shader.set_vec3("point_light.ambient", 0.09f, 0.03f, 0.03f);
	object_shader.set_vec3("point_light.diffuse", 0.9f, 0.3f, 0.3f);
	object_shader.set_vec3("point_light.specular", 0.9f, 0.3f, 0.3f);
	object_shader.set_float("point_light.constant", 1.0f);
	object_shader.set_float("point_light.linear", 0.14f);
	object_shader.set_float("point_light.quadratic", 0.07f);

	object_shader.set_vec3("spot_light.position", camera_position);
	object_shader.set_vec3("spot_light.direction", camera_direction);
	object_shader.set_float("spot_light.cut_off", glm::cos(glm::radians(10.0f)));
	object_shader.set_float("spot_light.outer_cut_off", glm::cos(glm::radians(15.0f)));
	object_shader.set_vec3("spot_light.ambient", 0.0f, 0.0f, 0.0f);
	object_shader.set_vec3("spot_light.diffuse", 1.0f, 1.0f, 1.0f);
	object_shader.set_vec3("spot_light.specular", 1.0f, 1.0f, 1.0f);
	object_shader.set_float("spot_light.constant", 1.0f);
	object_shader.set_float("spot_light.linear", 0.09f);
	object_shader.set_float("spot_light.quadratic", 0.032f);

	object_shader.set_float("material.shininess", 64.0f);
}

void set_wall_parameters(Shader& wall_shader)
{
	wall_shader.set_vec3("view_pos", camera_position);

	wall_shader.set_vec3("dir_light_pos", dir_light_pos);
	wall_shader.set_vec3("dir_light.ambient", 0.0f, 0.0f, 0.0f);
	wall_shader.set_vec3("dir_light.diffuse", 0.2f, 0.2f, 0.2f);
	wall_shader.set_vec3("dir_light.specular", 0.2f, 0.2f, 0.2f);

	wall_shader.set_vec3("point_light_pos", point_light_pos);
	wall_shader.set_vec3("point_light.ambient", 0.09f, 0.03f, 0.03f);
	wall_shader.set_vec3("point_light.diffuse", 0.9f, 0.3f, 0.3f);
	wall_shader.set_vec3("point_light.specular", 0.9f, 0.3f, 0.3f);
	wall_shader.set_float("point_light.constant", 1.0f);
	wall_shader.set_float("point_light.linear", 0.14f);
	wall_shader.set_float("point_light.quadratic", 0.007f);

	wall_shader.set_vec3("spot_light_pos", camera_position);
	wall_shader.set_vec3("spot_light_dir", camera_direction);
	wall_shader.set_float("spot_light.cut_off", glm::cos(glm::radians(10.0f)));
	wall_shader.set_float("spot_light.outer_cut_off", glm::cos(glm::radians(15.0f)));
	wall_shader.set_vec3("spot_light.ambient", 0.0f, 0.0f, 0.0f);
	wall_shader.set_vec3("spot_light.diffuse", 1.0f, 1.0f, 1.0f);
	wall_shader.set_vec3("spot_light.specular", 1.0f, 1.0f, 1.0f);
	wall_shader.set_float("spot_light.constant", 1.0f);
	wall_shader.set_float("spot_light.linear", 0.09f);
	wall_shader.set_float("spot_light.quadratic", 0.032f);

	wall_shader.set_float("material.shininess", 64.0f);
	wall_shader.set_float("height_scale", 0.1);
}


bool compare_func(const glm::vec3 &a, const glm::vec3 &b)
{
	float val1 = glm::length(camera_position - a);
	float val2 = glm::length(camera_position - b);

	return val1 < val2;
}

void render_wall()
{
	if (wall_VAO == 0) {
		glm::vec3 v1(-1.0f, 1.0f, 0.0f);
		glm::vec3 v2(-1.0f, -1.0f, 0.0f);
		glm::vec3 v3(1.0f, -1.0f, 0.0f);
		glm::vec3 v4(1.0f, 1.0f, 0.0f);

		glm::vec2 tex1(0.0f, 1.0f);
		glm::vec2 tex2(0.0f, 0.0f);
		glm::vec2 tex3(1.0f, 0.0f);
		glm::vec2 tex4(1.0f, 1.0f);

		glm::vec3 normal(0.0f, 0.0f, 1.0f);

		glm::vec3 e1 = v2 - v1;
		glm::vec3 e2 = v3 - v1;
		glm::vec2 delta_tex1 = tex2 - tex1;
		glm::vec2 delta_tex2 = tex3 - tex1;

		glm::vec3 tan1, bitang1;
		glm::vec3 tan2, bitang2;

		float f = 1.0f / (delta_tex1.x * delta_tex2.y - delta_tex2.x * delta_tex1.y);

		tan1.x = f * (delta_tex2.y * e1.x - delta_tex1.y * e2.x);
		tan1.y = f * (delta_tex2.y * e1.y - delta_tex1.y * e2.y);
		tan1.z = f * (delta_tex2.y * e1.z - delta_tex1.y * e2.z);

		bitang1.x = f * (-delta_tex2.x * e1.x + delta_tex1.x * e2.x);
		bitang1.y = f * (-delta_tex2.x * e1.y + delta_tex1.x * e2.y);
		bitang1.z = f * (-delta_tex2.x * e1.z + delta_tex1.x * e2.z);


		e1 = v3 - v1;
		e2 = v4 - v1;
		delta_tex1 = tex3 - tex1;
		delta_tex2 = tex4 - tex1;

		f = 1.0f / (delta_tex1.x * delta_tex2.y - delta_tex2.x * delta_tex1.y);

		tan2.x = f * (delta_tex2.y * e1.x - delta_tex1.y * e2.x);
		tan2.y = f * (delta_tex2.y * e1.y - delta_tex1.y * e2.y);
		tan2.z = f * (delta_tex2.y * e1.z - delta_tex1.y * e2.z);

		bitang2.x = f * (-delta_tex2.x * e1.x + delta_tex1.x * e2.x);
		bitang2.y = f * (-delta_tex2.x * e1.y + delta_tex1.x * e2.y);
		bitang2.z = f * (-delta_tex2.x * e1.z + delta_tex1.x * e2.z);

		float wall_vertices[] = {
			v1.x, v1.y, v1.z, normal.x, normal.y, normal.z, tex1.x, tex1.y, tan1.x, tan1.y, tan1.z, bitang1.x, bitang1.y, bitang1.z,
			v2.x, v2.y, v2.z, normal.x, normal.y, normal.z, tex2.x, tex2.y, tan1.x, tan1.y, tan1.z, bitang1.x, bitang1.y, bitang1.z,
			v3.x, v3.y, v3.z, normal.x, normal.y, normal.z, tex3.x, tex3.y, tan1.x, tan1.y, tan1.z, bitang1.x, bitang1.y, bitang1.z,

			v1.x, v1.y, v1.z, normal.x, normal.y, normal.z, tex1.x, tex1.y, tan2.x, tan2.y, tan2.z, bitang2.x, bitang2.y, bitang2.z,
			v3.x, v3.y, v3.z, normal.x, normal.y, normal.z, tex3.x, tex3.y, tan2.x, tan2.y, tan2.z, bitang2.x, bitang2.y, bitang2.z,
			v4.x, v4.y, v4.z, normal.x, normal.y, normal.z, tex4.x, tex4.y, tan2.x, tan2.y, tan2.z, bitang2.x, bitang2.y, bitang2.z
		};

		glGenVertexArrays(1, &wall_VAO);
		glGenBuffers(1, &wall_VBO);
		glBindVertexArray(wall_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, wall_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(wall_vertices), &wall_vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(wall_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	const float cameraSpeed = 2.0f * delta_time;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera_position += cameraSpeed * camera_direction;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera_position -= cameraSpeed * camera_direction;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera_position -= glm::normalize(glm::cross(camera_direction, camera_up)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera_position += glm::normalize(glm::cross(camera_direction, camera_up)) * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !is_inverse_key_pressed)
	{
		is_inversed = !is_inversed;
		is_inverse_key_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE)
	{
		is_inverse_key_pressed = false;
	}
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposition, double yposition)
{
	if (is_first_mouse)
	{
		last_x = xposition;
		last_y = yposition;
		is_first_mouse = false;
	}

	float xoffset = xposition - last_x;
	float yoffset = last_y - yposition;
	last_x = xposition;
	last_y = yposition;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	else if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camera_direction = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= yoffset;
	if (fov < 1.0f) {
		fov = 1.0f;
	}
	else if (fov > 45.0f){
		fov = 45.0f;
	}
}