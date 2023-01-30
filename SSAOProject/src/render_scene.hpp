#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
//#include "Texture.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include "utils/skybox_util.h"
#include "SOIL/stb_image_aug.h"

const unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;
unsigned int skyboxVAO, skyboxVBO, cubemapTexture;

int WIDTH = 500, HEIGHT = 500;

namespace models {
	Core::RenderContext wallsContext;
	Core::RenderContext planeContext;
	Core::RenderContext floorContext;
	Core::RenderContext roofContext;
	Core::RenderContext window1Context;
	Core::RenderContext window2Context;
	Core::RenderContext doorContext;
	Core::RenderContext bedLegsContext;
	Core::RenderContext bedMainContext;
	Core::RenderContext bedBackContext;
	Core::RenderContext bedMateraceContext;
	Core::RenderContext bedSphereContext;
	Core::RenderContext nightstand1LegsContext;
	Core::RenderContext nightstand1MainContext;
	Core::RenderContext nightstand1SphereContext;
	Core::RenderContext bedlamp1MainContext;
	Core::RenderContext bedlamp1TopContext;
	Core::RenderContext nightstand2LegsContext;
	Core::RenderContext nightstand2MainContext;
	Core::RenderContext nightstand2SphereContext;
	Core::RenderContext bedlamp2MainContext;
	Core::RenderContext bedlamp2TopContext;
	Core::RenderContext wardrobeMainContext;
	Core::RenderContext wardrobeSphere1Context;
	Core::RenderContext wardrobeSphere2Context;
	Core::RenderContext bookshelfContext;
	Core::RenderContext deskLegsContext;
	Core::RenderContext deskMainContext;
	Core::RenderContext deskTopContext;
	Core::RenderContext deskDrawerContext;
	Core::RenderContext deskSphereContext;
	Core::RenderContext tvBottomContext;
	Core::RenderContext tvMiddleContext;
	Core::RenderContext tvTopContext;
	Core::RenderContext tvMainContext;
	Core::RenderContext chairContext;
	Core::RenderContext lampPart1Context;
	Core::RenderContext lampPart2Context;
	Core::RenderContext lampPart3Context;
	Core::RenderContext lampPart4Context;
	Core::RenderContext lampPart5Context;
	Core::RenderContext lampBulbContext;

	Core::RenderContext spaceshipContext;
	Core::RenderContext sphereContext;
	Core::RenderContext testContext;

	/*
	Core::RenderContext bedContext;
	Core::RenderContext chairContext;
	Core::RenderContext deskContext;
	Core::RenderContext doorContext;
	Core::RenderContext drawerContext;
	Core::RenderContext marbleBustContext;
	Core::RenderContext materaceContext;
	Core::RenderContext pencilsContext;
	Core::RenderContext planeContext;
	Core::RenderContext roomContext;
	Core::RenderContext spaceshipContext;
	Core::RenderContext sphereContext;
	Core::RenderContext windowContext;
	Core::RenderContext testContext;
	*/
}

GLuint depthMapFBO;
GLuint depthMap;

GLuint program;
GLuint programDepth;
GLuint programSun;
GLuint programTest;
GLuint programTex;
GLuint skyboxProgram;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;

//DO NOT TOUCH
glm::vec3 sunPos = glm::vec3(15.740971f, 7.149999f, -6.369280f);
glm::vec3 sunDir = glm::vec3(0.93633f, 0.351106, 0.003226f);
glm::vec3 sunColor = glm::vec3(0.9f, 0.9f, 0.7f) * 5;

//glm::vec3 sunPos = glm::vec3(-4.740971f, 2.149999f, 0.369280f);
//glm::vec3 sunDir = glm::vec3(-0.93633f, 0.351106, 0.003226f);

glm::vec3 cameraPos = glm::vec3(0.479490f, 1.250000f, -2.124680f);
glm::vec3 cameraDir = glm::vec3(-0.354510f, 0.000000f, 0.935054f);


glm::vec3 spaceshipPos = glm::vec3(0.065808f, 1.250000f, -2.189549f);
glm::vec3 spaceshipDir = glm::vec3(-0.490263f, 0.000000f, 0.871578f);
GLuint VAO, VBO;



float aspectRatio = 1.f;

float exposition = 5.f;

glm::vec3 pointlightPos = glm::vec3(0, 2, 0);
glm::vec3 pointlightColor = glm::vec3(0.9, 0.6, 0.6);

glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);
glm::vec3 spotlightColor = glm::vec3(0.4, 0.4, 0.9) * 3;
float spotlightPhi = 3.14 / 4;

//DO NOT TOUCH
glm::mat4 lightVP = glm::ortho(-15.f, 2.f, -10.f, 10.f, -5.0f, 35.0f) * glm::lookAt(sunPos, sunPos - sunDir, glm::vec3(0, 1, 0));

float lastTime = -1.f;
float deltaTime = 0.f;

void renderSkybox();

void updateDeltaTime(float time) {
	if (lastTime < 0) {
		lastTime = time;
		return;
	}

	deltaTime = time - lastTime;
	if (deltaTime > 0.1) deltaTime = 0.1;
	lastTime = time;
}

void initDepthMap() {
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{

	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 20.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectPBR(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, float roughness, float metallic) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//glm::mat4 lightVP = glm::ortho(-3.f, 2.2f, -2.f, 3.5f, 1.f, 30.0f) * glm::lookAt(sunPos, sunPos - sunDir, glm::vec3(0, 1, 0));

	glUniformMatrix4fv(glGetUniformLocation(program, "lightVP"), 1, GL_FALSE, (float*)&lightVP);

	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(program, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3f(glGetUniformLocation(program, "lightPos"), pointlightPos.x, pointlightPos.y, pointlightPos.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), pointlightColor.x, pointlightColor.y, pointlightColor.z);

	glUniform3f(glGetUniformLocation(program, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(program, "spotlightPhi"), spotlightPhi);
	Core::DrawContext(context);

}

void drawObjectDepth(Core::RenderContext& context, glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix) {
	glUniformMatrix4fv(glGetUniformLocation(programDepth, "viewProjectionMatrix"), 1, GL_FALSE, (float*)&viewProjectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(programDepth, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	Core::DrawContext(context);
}

void renderShadowapSun() {
	float time = glfwGetTime();
	//uzupelnij o renderowanie glebokosci do tekstury
	glUseProgram(programDepth);

	//ustawianie przestrzeni rysowania 
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//bindowanie FBO
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//czyszczenie mapy głębokości 
	glClear(GL_DEPTH_BUFFER_BIT);
	//ustawianie programu
	glUseProgram(programDepth);
	glm::mat4 viewProjection = lightVP;
	
	//drawObjectDepth(sphereContext, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::scale(glm::vec3(0.3f)), glm::mat4());
	//drawObjectDepth(sphereContext, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)), glm::mat4());
	
	

	drawObjectDepth(models::wallsContext, viewProjection, glm::mat4());
	drawObjectDepth(models::planeContext, viewProjection, glm::mat4());
	drawObjectDepth(models::floorContext, viewProjection, glm::mat4());
	drawObjectDepth(models::roofContext, viewProjection, glm::mat4());
	drawObjectDepth(models::window1Context, viewProjection, glm::mat4());
	drawObjectDepth(models::window2Context, viewProjection, glm::mat4());
	drawObjectDepth(models::doorContext, viewProjection, glm::mat4());
	drawObjectDepth(models::bedLegsContext, viewProjection, glm::mat4());
	drawObjectDepth(models::bedMainContext, viewProjection, glm::mat4());
	drawObjectDepth(models::bedBackContext, viewProjection, glm::mat4());
	drawObjectDepth(models::bedMateraceContext, viewProjection, glm::mat4());
	drawObjectDepth(models::bedSphereContext, viewProjection, glm::mat4());
	drawObjectDepth(models::nightstand1LegsContext, viewProjection, glm::mat4());
	drawObjectDepth(models::nightstand1MainContext, viewProjection, glm::mat4());
	drawObjectDepth(models::nightstand1SphereContext, viewProjection, glm::mat4());
	drawObjectDepth(models::bedlamp1MainContext, viewProjection, glm::mat4());
	drawObjectDepth(models::bedlamp1TopContext, viewProjection, glm::mat4());
	drawObjectDepth(models::nightstand2LegsContext, viewProjection, glm::mat4());
	drawObjectDepth(models::nightstand2MainContext, viewProjection, glm::mat4());
	drawObjectDepth(models::nightstand2SphereContext, viewProjection, glm::mat4());
	drawObjectDepth(models::bedlamp2MainContext, viewProjection, glm::mat4());
	drawObjectDepth(models::bedlamp2TopContext, viewProjection, glm::mat4());
	drawObjectDepth(models::wardrobeMainContext, viewProjection, glm::mat4());
	drawObjectDepth(models::wardrobeSphere1Context, viewProjection, glm::mat4());
	drawObjectDepth(models::wardrobeSphere2Context, viewProjection, glm::mat4());
	drawObjectDepth(models::bookshelfContext, viewProjection, glm::mat4());
	drawObjectDepth(models::deskLegsContext, viewProjection, glm::mat4());
	drawObjectDepth(models::deskMainContext, viewProjection, glm::mat4());
	drawObjectDepth(models::deskTopContext, viewProjection, glm::mat4());
	drawObjectDepth(models::deskDrawerContext, viewProjection, glm::mat4());
	drawObjectDepth(models::deskSphereContext, viewProjection, glm::mat4());
	drawObjectDepth(models::tvBottomContext, viewProjection, glm::mat4());
	drawObjectDepth(models::tvMiddleContext, viewProjection, glm::mat4());
	drawObjectDepth(models::tvTopContext, viewProjection, glm::mat4());
	drawObjectDepth(models::tvMainContext, viewProjection, glm::mat4());
	drawObjectDepth(models::chairContext, viewProjection, glm::mat4());
	drawObjectDepth(models::lampPart1Context, viewProjection, glm::mat4());
	drawObjectDepth(models::lampPart2Context, viewProjection, glm::mat4());
	drawObjectDepth(models::lampPart3Context, viewProjection, glm::mat4());
	drawObjectDepth(models::lampPart4Context, viewProjection, glm::mat4());
	drawObjectDepth(models::lampPart5Context, viewProjection, glm::mat4());
	drawObjectDepth(models::lampBulbContext, viewProjection, glm::mat4());

	/*
	drawObjectDepth(models::bedContext, viewProjection, glm::mat4());
	drawObjectDepth(models::chairContext, viewProjection, glm::mat4());
	drawObjectDepth(models::deskContext, viewProjection, glm::mat4());
	drawObjectDepth(models::doorContext, viewProjection, glm::mat4());
	drawObjectDepth(models::drawerContext, viewProjection, glm::mat4());
	drawObjectDepth(models::marbleBustContext, viewProjection, glm::mat4());
	drawObjectDepth(models::materaceContext, viewProjection, glm::mat4());
	drawObjectDepth(models::pencilsContext, viewProjection, glm::mat4());
	drawObjectDepth(models::planeContext, viewProjection, glm::mat4());
	drawObjectDepth(models::roomContext, viewProjection, glm::mat4());
	drawObjectDepth(models::windowContext, viewProjection, glm::mat4());
	*/

	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});


	//drawObjectColor(shipContext,
	//	glm::translate(cameraPos + 1.5 * cameraDir + cameraUp * -0.5f) * inveseCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
	//	glm::vec3(0.3, 0.3, 0.5)
	//	);
	drawObjectDepth(shipContext,
		viewProjection, glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.03f)));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void renderScene(GLFWwindow* window)
{
	glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float time = glfwGetTime();
	updateDeltaTime(time);
	renderShadowapSun();

	//space lamp
	glUseProgram(programSun);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1));
	glUniformMatrix4fv(glGetUniformLocation(programSun, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(programSun, "color"), sunColor.x / 2, sunColor.y / 2, sunColor.z / 2);
	glUniform1f(glGetUniformLocation(programSun, "exposition"), exposition);
	

	//Core::DrawContext(sphereContext);

	glUseProgram(program);

	//drawObjectPBR(sphereContext, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::scale(glm::vec3(0.3f)), glm::vec3(0.2, 0.7, 0.3), 0.3, 0.0);

	//drawObjectPBR(sphereContext,glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)),glm::vec3(0.5, 0.5, 0.5), 0.7, 0.0);


	drawObjectPBR(models::wallsContext, glm::mat4(), glm::vec3(0.0924f, 0.465f, 0.770f), 0.8f, 0.0f);
	drawObjectPBR(sphereContext, glm::translate(sunPos) * glm::mat4(), glm::vec3(0.0924f, 0.465f, 0.770f), 0.8f, 0.0f);
	drawObjectPBR(models::planeContext, glm::mat4(), glm::vec3(0.630f, 0.413f, 0.2378f), 0.2f, 0.0f);
	drawObjectPBR(models::floorContext, glm::mat4(), glm::vec3(0.630f, 0.413f, 0.2378f), 0.2f, 0.0f);
	drawObjectPBR(models::roofContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), 0.8f, 0.0f);
	drawObjectPBR(models::window1Context, glm::mat4(), glm::vec3(0.250f, 0.250f, 0.250f), 0.2f, 0.0f);
	drawObjectPBR(models::window2Context, glm::mat4(), glm::vec3(0.250f, 0.250f, 0.250f), 0.2f, 0.0f);
	drawObjectPBR(models::doorContext, glm::mat4(), glm::vec3(0.250f, 0.250f, 0.250f), 0.2f, 0.0f);
	drawObjectPBR(models::bedLegsContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::bedMainContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::bedBackContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::bedMateraceContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), 0.8f, 0.0f);
	drawObjectPBR(models::bedSphereContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), 0.8f, 0.0f);
	drawObjectPBR(models::nightstand1LegsContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::nightstand1MainContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::nightstand1SphereContext, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::bedlamp1MainContext, glm::mat4(), glm::vec3(0.0138f, 0.690f, 0.149f), 0.2f, 0.0f);
	drawObjectPBR(models::bedlamp1TopContext, glm::mat4(), glm::vec3(1.00f, 0.957f, 0.140f), 0.2f, 0.0f);
	drawObjectPBR(models::nightstand2LegsContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::nightstand2MainContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::nightstand2SphereContext, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::bedlamp2MainContext, glm::mat4(), glm::vec3(0.0138f, 0.690f, 0.149f), 0.2f, 0.0f);
	drawObjectPBR(models::bedlamp2TopContext, glm::mat4(), glm::vec3(1.00f, 0.957f, 0.140f), 0.2f, 0.0f);
	drawObjectPBR(models::wardrobeMainContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::wardrobeSphere1Context, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::wardrobeSphere2Context, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::bookshelfContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::deskLegsContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::deskMainContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::deskTopContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::deskDrawerContext, glm::mat4(), glm::vec3(0.228691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::deskSphereContext, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::tvBottomContext, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 1.0f);
	drawObjectPBR(models::tvMiddleContext, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 1.0f);
	drawObjectPBR(models::tvTopContext, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 1.0f);
	drawObjectPBR(models::tvMainContext, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 1.0f);
	drawObjectPBR(models::chairContext, glm::mat4(), glm::vec3(0.830f, 0.0830f, 0.158f), 0.4f, 0.0f);
	drawObjectPBR(models::lampPart1Context, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::lampPart2Context, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::lampPart3Context, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::lampPart4Context, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::lampPart5Context, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::lampBulbContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), 0.2f, 0.0f);

	/*
	drawObjectPBR(models::bedContext, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), 0.2f, 0.0f);
	drawObjectPBR(models::chairContext, glm::mat4(), glm::vec3(0.195239f, 0.37728f, 0.8f), 0.4f, 0.0f);
	drawObjectPBR(models::deskContext, glm::mat4(), glm::vec3(0.428691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::doorContext, glm::mat4(), glm::vec3(0.402978f, 0.120509f, 0.057729f), 0.2f, 0.0f);
	drawObjectPBR(models::drawerContext, glm::mat4(), glm::vec3(0.428691f, 0.08022f, 0.036889f), 0.2f, 0.0f);
	drawObjectPBR(models::marbleBustContext, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), 0.5f, 1.0f);
	drawObjectPBR(models::materaceContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), 0.8f, 0.0f);
	drawObjectPBR(models::pencilsContext, glm::mat4(), glm::vec3(0.10039f, 0.018356f, 0.001935f), 0.1f, 0.0f);
	drawObjectPBR(models::planeContext, glm::mat4(), glm::vec3(0.402978f, 0.120509f, 0.057729f), 0.2f, 0.0f);
	drawObjectPBR(models::roomContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), 0.8f, 0.0f);
	drawObjectPBR(models::windowContext, glm::mat4(), glm::vec3(0.402978f, 0.120509f, 0.057729f), 0.2f, 0.0f);
	*/

	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});


	//drawObjectColor(shipContext,
	//	glm::translate(cameraPos + 1.5 * cameraDir + cameraUp * -0.5f) * inveseCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
	//	glm::vec3(0.3, 0.3, 0.5)
	//	);
	drawObjectPBR(shipContext,
		glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.03f)),
		glm::vec3(0.3, 0.3, 0.5),
		0.2, 1.0
	);

	spotlightPos = spaceshipPos + 0.2 * spaceshipDir;
	spotlightConeDir = spaceshipDir;

	renderSkybox();
	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram(programTest);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//Core::DrawContext(models::testContext);
	
	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}
void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

unsigned int loadCubemap()
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	std::vector<std::string> faces = getCubemapFaces();

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			printf("Loaded cubemap texture for %s\n", faces[i].c_str());
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void generateSkybox() {
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	cubemapTexture = loadCubemap();
	glUseProgram(skyboxProgram);
	glUniform1i(glGetUniformLocation(skyboxProgram, "skybox"), 0);
}

void renderSkybox() {

	glm::mat4 projectionMatrix = createPerspectiveMatrix();
 	glm::mat4 viewMatrix =  createCameraMatrix();

	glDepthFunc(GL_LEQUAL);
	glUseProgram(skyboxProgram);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void loadModels() {
	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/spaceship.obj", shipContext);


	loadModelToContext("./models/walls.obj", models::wallsContext);
	loadModelToContext("./models/plane.obj", models::planeContext);
	loadModelToContext("./models/floor.obj", models::floorContext);
	loadModelToContext("./models/roof.obj", models::roofContext);
	loadModelToContext("./models/window1.obj", models::window1Context);
	loadModelToContext("./models/window2.obj", models::window2Context);
	loadModelToContext("./models/door.obj", models::doorContext);
	loadModelToContext("./models/bed_legs.obj", models::bedLegsContext);
	loadModelToContext("./models/bed_main.obj", models::bedMainContext);
	loadModelToContext("./models/bed_back.obj", models::bedBackContext);
	loadModelToContext("./models/bed_materace.obj", models::bedMateraceContext);
	loadModelToContext("./models/bed_sphere.obj", models::bedSphereContext);
	loadModelToContext("./models/nightstand1_legs.obj", models::nightstand1LegsContext);
	loadModelToContext("./models/nightstand1_main.obj", models::nightstand1MainContext);
	loadModelToContext("./models/nightstand1_sphere.obj", models::nightstand1SphereContext);
	loadModelToContext("./models/bedlamp1_main.obj", models::bedlamp1MainContext);
	loadModelToContext("./models/bedlamp1_top.obj", models::bedlamp1TopContext);
	loadModelToContext("./models/nightstand2_legs.obj", models::nightstand2LegsContext);
	loadModelToContext("./models/nightstand2_main.obj", models::nightstand2MainContext);
	loadModelToContext("./models/nightstand2_sphere.obj", models::nightstand2SphereContext);
	loadModelToContext("./models/bedlamp2_main.obj", models::bedlamp2MainContext);
	loadModelToContext("./models/bedlamp2_top.obj", models::bedlamp2TopContext);
	loadModelToContext("./models/wardrobe_main.obj", models::wardrobeMainContext);
	loadModelToContext("./models/wardrobe_sphere1.obj", models::wardrobeSphere1Context);
	loadModelToContext("./models/wardrobe_sphere2.obj", models::wardrobeSphere2Context);
	loadModelToContext("./models/bookshelf.obj", models::bookshelfContext);
	loadModelToContext("./models/desk_legs.obj", models::deskLegsContext);
	loadModelToContext("./models/desk_main.obj", models::deskMainContext);
	loadModelToContext("./models/desk_top.obj", models::deskTopContext);
	loadModelToContext("./models/desk_drawer.obj", models::deskDrawerContext);
	loadModelToContext("./models/desk_sphere.obj", models::deskSphereContext);
	loadModelToContext("./models/tv_bottom.obj", models::tvBottomContext);
	loadModelToContext("./models/tv_middle.obj", models::tvMiddleContext);
	loadModelToContext("./models/tv_top.obj", models::tvTopContext);
	loadModelToContext("./models/tv_main.obj", models::tvMainContext);
	loadModelToContext("./models/chair.obj", models::chairContext);
	loadModelToContext("./models/lamp_part1.obj", models::lampPart1Context);
	loadModelToContext("./models/lamp_part2.obj", models::lampPart2Context);
	loadModelToContext("./models/lamp_part3.obj", models::lampPart3Context);
	loadModelToContext("./models/lamp_part4.obj", models::lampPart4Context);
	loadModelToContext("./models/lamp_part5.obj", models::lampPart5Context);
	loadModelToContext("./models/lamp_bulb.obj", models::lampBulbContext);

	/*
	loadModelToContext("./models/bed.obj", models::bedContext);
	loadModelToContext("./models/chair.obj", models::chairContext);
	loadModelToContext("./models/desk.obj", models::deskContext);
	loadModelToContext("./models/door.obj", models::doorContext);
	loadModelToContext("./models/drawer.obj", models::drawerContext);
	loadModelToContext("./models/marbleBust.obj", models::marbleBustContext);
	loadModelToContext("./models/materace.obj", models::materaceContext);
	loadModelToContext("./models/pencils.obj", models::pencilsContext);
	loadModelToContext("./models/plane.obj", models::planeContext);
	loadModelToContext("./models/room.obj", models::roomContext);
	loadModelToContext("./models/spaceship.obj", models::spaceshipContext);
	loadModelToContext("./models/sphere.obj", models::sphereContext);
	loadModelToContext("./models/window.obj", models::windowContext);
	loadModelToContext("./models/test.obj", models::testContext);
	*/
}

void loadShaders() {
	program = shaderLoader.CreateProgram("shaders/shader_9_1.vert", "shaders/shader_9_1.frag");
	programDepth = shaderLoader.CreateProgram("shaders/shader_shadow.vert", "shaders/shader_shadow.frag");
	programTest = shaderLoader.CreateProgram("shaders/test.vert", "shaders/test.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_8_sun.vert", "shaders/shader_8_sun.frag");
	skyboxProgram = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	initDepthMap();

	glEnable(GL_DEPTH_TEST);
	
	loadShaders();
	loadModels();
	generateSkybox();

}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 0.05f * deltaTime * 60;
	float moveSpeed = 0.05f * deltaTime * 60;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		spaceshipPos += spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		spaceshipPos -= spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		spaceshipPos += spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		spaceshipPos -= spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		spaceshipPos += spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		spaceshipPos -= spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));

	cameraPos = spaceshipPos - 0.5 * spaceshipDir + glm::vec3(0, 1, 0) * 0.2f;
	cameraDir = spaceshipDir;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.05;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		printf("spaceshipPos = glm::vec3(%ff, %ff, %ff);\n", spaceshipPos.x, spaceshipPos.y, spaceshipPos.z);
		printf("spaceshipDir = glm::vec3(%ff, %ff, %ff);\n", spaceshipDir.x, spaceshipDir.y, spaceshipDir.z);
	}

	//cameraDir = glm::normalize(-cameraPos);

}

// funkcja jest glowna petla
void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}
//}