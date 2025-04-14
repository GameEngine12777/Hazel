#pragma once

namespace Hazel {

	/**
	* 图形上下文
	* 通过多态的行为后期封装不同平台的图形上下文
	*/
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		virtual void Init() = 0;

		/** 交换缓存 */
		virtual void SwapBuffers() = 0;

		static Scope<GraphicsContext> Create(void* window);
	};

}