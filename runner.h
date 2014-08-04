#ifdef __WAND__
target[name[runner.h] type[include] ]
dependency[runner.o]
#endif

#ifndef LATESTCOPY_RUNNER_H
#define LATESTCOPY_RUNNER_H

#include "direntry.h"
#include <herbs/application/application.h>
#include <herbs/path/path.h>

namespace LatestCopy
	{
	class Runner:public Herbs::Application
		{
		public:
			struct Params
				{
				Herbs::Path dir_source;
				Herbs::Path dir_dest;
				uintmax_t size_max;
				};

			Runner(Herbs::LogWriter& log,Herbs::Directory&& options);

			int run();
			void filesCollect();
			void filesCopy();
			void destroy();

		private:
			Params params;
			Herbs::Array<Direntry> dirs;
		};
	}

#endif