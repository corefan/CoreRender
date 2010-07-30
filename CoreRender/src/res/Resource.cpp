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

#include "CoreRender/res/Resource.hpp"
#include "CoreRender/core/ConditionVariable.hpp"
#include "CoreRender/res/ResourceManager.hpp"

namespace cr
{
namespace res
{
	Resource::Resource()
		: loaded(false), rmgr(0)
	{
	}
	Resource::~Resource()
	{
	}
	
	void Resource::setName(const std::string &name)
	{
		this->name = name;
	}
	std::string Resource::getName()
	{
		return name;
	}

	void Resource::queueForLoading()
	{
		if (rmgr)
			rmgr->queueForLoading(this);
	}

	bool Resource::load()
	{
		finishLoading(false);
		return false;
	}
	bool Resource::unload()
	{
		return false;
	}
	void Resource::prioritizeLoading()
	{
		if (rmgr)
			rmgr->prioritize(this);
	}
	bool Resource::waitForLoading(bool recursive,
	                              bool highpriority)
	{
		// Shortcut
		if (isLoaded())
			return true;
		// Set priority
		prioritizeLoading();
		// Wait for resource to be loaded
		core::ConditionVariable waitvar;
		{
			tbb::spin_mutex::scoped_lock lock(statemutex);
			if (isLoaded())
				return true;
			waitvar.lock();
			waiting.push_back(&waitvar);
		}
		waitvar.wait();
		waitvar.unlock();
		return isLoaded();
	}

	void Resource::finishLoading(bool loaded)
	{
		tbb::spin_mutex::scoped_lock lock(statemutex);
		this->loaded = loaded;
		for (unsigned int i = 0; i < waiting.size(); i++)
		{
			waiting[i]->lock();
			waiting[i]->signal();
			waiting[i]->unlock();
		}
		waiting.clear();
	}
}
}
