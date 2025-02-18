#pragma once

#include "..\Core.h"
#include <ppl.h>
#include <ppltasks.h>

namespace sketch
{
	class HangUpJob
	{
	public:
		HangUpJob();
		~HangUpJob();

		virtual bool isDone() const;

		virtual void run() = 0;
		virtual void updateOnMainThread() = 0;
		virtual void onFinish() = 0;

	protected:
		concurrency::task<void> m_task;
	};

}


