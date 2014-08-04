#ifdef __WAND__
target[name[direntry.h] type[include]]
dependency[direntry.o]
#endif

#ifndef LATESTCOPY_DIRENTRY_H
#define LATESTCOPY_DIRENTRY_H

#include <herbs/filesenumerator/filesenumerator.h>

namespace Herbs
	{
	class LogWriter;
	}

namespace LatestCopy
	{
	class Direntry
		{
		public:
			Direntry(){}

			Direntry(const Herbs::FilesEnumerator::FileInfo& fileinfo
				,const Herbs::Path& in_dir);

			bool copy(const Herbs::Path& new_dir,Herbs::LogWriter& log);

			bool operator<(const Direntry& dir) const
				{return time_modified>dir.time_modified;}

			uintmax_t sizeGet() const
				{return size;}

			const Herbs::Path& nameGet() const
				{return filename;}

			timefile_t timeModifiedGet() const
				{return time_modified;}

		private:
			Herbs::Path m_in_dir;
			Herbs::Path filename;
			timefile_t time_modified;
			uintmax_t size;

			void valuesCompute(const Herbs::FilesEnumerator::FileInfo& fileinfo);
		};
	}

#endif


