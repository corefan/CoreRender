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

#include "CoreRender/render/RenderResource.hpp"
#include "CoreRender/render/Renderer.hpp"
#include "CoreRender/core/ConditionVariable.hpp"

namespace cr
{
namespace render
{
	RenderResource::RenderResource(Renderer *renderer)
		: renderer(renderer), uploading(false), waitvar(0)
	{
	}
	RenderResource::~RenderResource()
	{
	}

	bool RenderResource::create()
	{
		return true;
	}
	bool RenderResource::destroy()
	{
		return false;
	}
	bool RenderResource::upload()
	{
		uploadFinished();
		return true;
	}

	void RenderResource::registerUpload()
	{
		{
			tbb::spin_mutex::scoped_lock lock(uploadmutex);
			// Do not submit this several times
			if (uploading)
				return;
			uploading = true;
		}
		renderer->registerUpload(this);
	}
	void RenderResource::uploadFinished()
	{
		core::ConditionVariable *wait = 0;
		{
			tbb::spin_mutex::scoped_lock lock(uploadmutex);
			uploading = false;
			wait = waitvar;
		}
		if (wait)
		{
			wait->lock();
			wait->signal();
			wait->unlock();
		}
	}

	void RenderResource::waitForUpload()
	{
		if (!uploading)
			return;
		core::ConditionVariable waitvar;
		{
			tbb::spin_mutex::scoped_lock lock(uploadmutex);
			if (!uploading)
				return;
			waitvar.lock();
			this->waitvar = &waitvar;
		}
		waitvar.wait();
		waitvar.unlock();
	}

	void RenderResource::onDelete()
	{
		// We probably cannot free GPU resources here
		renderer->registerDelete(this);
	}
}
}
