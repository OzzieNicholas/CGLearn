#include "sketch/Util/HangUpJob.h"

#include "sketch/STCore.h"

using namespace Engine;

namespace sketch
{
	HangUpJob::HangUpJob()
	{
		
	}

	HangUpJob::~HangUpJob()
	{

	}

	bool HangUpJob::isDone() const {
		try {
			return m_task.is_done();
		}
		catch (...) {
			return false;
		}
		
	}
}