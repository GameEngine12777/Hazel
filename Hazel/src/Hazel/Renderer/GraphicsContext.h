#pragma once

namespace Hazel {

	class GraphicsContext
	{
	public:
		virtual void Init() = 0;

		/** 交换缓存 */
		virtual void SwapBuffers() = 0;
	};

}