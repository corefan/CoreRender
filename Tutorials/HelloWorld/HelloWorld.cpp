/*
Copyright (C) 2010, Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "CoreRender.hpp"

#include <GL/glfw.h>

using namespace cr;

int main(int argc, char **argv)
{
	// Create render window
	glfwInit();
	if (!glfwOpenWindow(1024,768, 8, 8, 8, 8, 24, 8, GLFW_WINDOW))
	{
		std::cerr << "Failed to open render window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Initialize file system
	GraphicsEngine graphics;
	{
		core::StandardFileSystem::Ptr filesystem;
		filesystem = new core::StandardFileSystem();
		filesystem->mount("Tutorials/media/", "/", core::FileAccess::Read);
		// Writes (e.g. log) shall go into the working directory
		filesystem->mount("", "/", core::FileAccess::Write);
		graphics.setFileSystem(filesystem);
	}
	// Initialize graphics engine
	if (!graphics.init())
	{
		std::cerr << "Graphics engine failed to initialize!" << std::endl;
		return -1;
	}
	graphics.getLog()->setConsoleLevel(core::LogLevel::Debug);
	// Create scene
	scene::Scene scene;
	// Add some models
	scene::Mesh::Ptr jeep = graphics.getMesh("/models/jeep.model.xml");
	scene::Mesh::Ptr dwarf = graphics.getMesh("/models/dwarf.model.xml");
	scene::AnimatedMesh::Ptr dwarf2 = new scene::AnimatedMesh(dwarf);
	scene::Animation::Ptr dwarfanim = graphics.getAnimation("/models/dwarf.anim");
	dwarf2->addAnimation(dwarfanim, 1.0f);
	math::Matrix4 dwarf2positions[1024];
	for (unsigned int i = 0; i < 1024; i++)
	{
		float x = (float)(i % 32) * 10.0f - 160.0f;
		float z = (float)(i / 32) * 10.0f - 160.0f;
		dwarf2positions[i] = math::Matrix4::TransMat(x, 0.0f, z)
		                   * math::Matrix4::ScaleMat(0.2f, 0.2f, 0.2f);
	}
	// Wait for loading
	dwarf->waitForLoading(true);
	dwarfanim->waitForLoading(false);
	// Add a terrain
	// TODO
	// Add some lights
	/*scene::SpotLight::Ptr spotlight = new scene::SpotLight(0, "SPOTLIGHT", "SHADOWMAP");
	spotlight->setRadius(10.0f);
	spotlight->setColor(core::Color(1.0f, 0.0f, 0.0f, 1.0f));
	scene::PointLight::Ptr pointlight = new scene::PointLight(0, "POINTLIGHT", "");
	spotlight->setRadius(10.0f);
	spotlight->setColor(core::Color(0.0f, 1.0f, 0.0f, 1.0f));*/
	// Add a camera
	scene::Camera::Ptr camera = new scene::Camera;
	//camera->setProjMat(math::Matrix4::Ortho(200.0f, 200.0f, -100.0f, 100.0f));
	camera->setProjMat(math::Matrix4::PerspectiveFOV(90.0f, 4.0f/3.0f, 1.0f, 1000.0f));
	math::Matrix4 viewmat = math::Matrix4::TransMat(0.0f, 0.0f, -200.0f)
	                      * math::Quaternion(math::Vector3F(30.0f, 0.0f, 0.0f)).toMatrix();
	camera->setViewMat(viewmat);
	camera->setPipeline(graphics.getPipeline("/pipelines/Forward.pipeline.xml"));
	camera->setViewport(0, 0, 1024, 768);
	// Add the camera and the lights to the scene
	scene.addCamera(camera);
	/*scene.addLight(spotlight);
	scene.addLight(pointlight);*/
	// Render loop
	bool stopping = false;
	float animtime = 0.0f;
	core::Time fpstime = core::Time::Now();
	int fps = 0;
	while (!stopping)
	{
		// Process input
		// TODO
		// Set animations
		animtime = animtime + 0.01f;
		dwarf2->setAnimation(0, animtime);
		if (animtime > 1.4f)
			animtime = 0.0f;
		viewmat = viewmat * math::Quaternion(math::Vector3F(0.0f, 1.0f, 0.0f)).toMatrix();
		camera->setViewMat(viewmat);
		// Render objects
		render::FrameData *frame = graphics.beginFrame();
		render::SceneFrameData *scenedata = scene.beginFrame(frame);
		render::RenderQueue *renderqueues = scenedata->getRenderQueues();
		unsigned int queuecount = scenedata->getRenderQueueCount();
		for (unsigned int i = 0; i < queuecount; i++)
		{
			jeep->render(renderqueues[i], math::Matrix4::TransMat(20.0f, 0.0f, 0.0f) * math::Matrix4::ScaleMat(5.0f, 5.0f, 5.0f));
			dwarf->render(renderqueues[i], math::Matrix4::TransMat(-20.0f, 0.0f, 0.0f));
			dwarf2->render(renderqueues[i], 1024, dwarf2positions);
		}
		graphics.endFrame(frame);
		// Render frame
		graphics.render(frame);
		// Swap buffers
		glfwSwapBuffers();
		if (glfwGetWindowParam(GLFW_OPENED) == GL_FALSE)
			stopping = true;
		fps++;
		core::Time currenttime = core::Time::Now();
		if (currenttime - fpstime >= core::Duration::Seconds(1))
		{
			std::cout << "FPS: " << fps << std::endl;
			fpstime = currenttime;
			fps = 0;
		}
	}
	// Destroy ressources
	scene.removeCamera(camera);
	/*scene.removeLight(spotlight);
	scene.removeLight(pointlight);*/
	camera = 0;
	/*spotlight = 0;
	pointlight = 0;*/
	jeep = 0;
	dwarf = 0;
	dwarf2 = 0;
	dwarfanim = 0;
	graphics.shutdown();
	// Close render window
	glfwTerminate();
	return 0;
}
