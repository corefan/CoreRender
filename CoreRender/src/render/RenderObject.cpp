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

#include "CoreRender/render/RenderObject.hpp"
#include "CoreRender/render/UploadManager.hpp"

namespace cr
{
namespace render
{
	RenderObject::RenderObject(UploadManager &uploadmgr)
		: uploadmgr(uploadmgr), uploading(false)
	{
	}
	RenderObject::~RenderObject()
	{
	}

	void *RenderObject::prepareForUpload()
	{
		tbb::mutex::scoped_lock lock(uploadmutex);
		void *data = getUploadData();
		uploading = false;
		return data;
	}
	bool RenderObject::isUploading()
	{
		tbb::mutex::scoped_lock lock(uploadmutex);
		return uploading;
	}

	void RenderObject::registerUpload()
	{
		tbb::mutex::scoped_lock lock(uploadmutex);
		uploading = true;
		uploadmgr.registerUpload(this);
	}
	void RenderObject::onDelete()
	{
		uploadmgr.registerDeletion(this);
	}
}
}
