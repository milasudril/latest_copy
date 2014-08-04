#ifdef __WAND__
target[name[latest_copy] type[application] platform[;GNU/Linux]]
#endif

#include "runner.h"
#include <herbs/maindefault/maindefault.h>

Herbs::Application* Herbs::Application::instanceCreate(Herbs::LogWriter& log
	,Herbs::Directory&& options)
	{return new LatestCopy::Runner(log,std::move(options));}
