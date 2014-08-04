#ifdef __WAND__
target[name[runner.o] type[object]]
#endif

#include "runner.h"
#include <herbs/exceptionmissing/exceptionmissing.h>
#include <herbs/directory/directory.h>

#include <herbs/stringformat/stringformat.h>
#include <herbs/logwriter/logwriter.h>
#include <herbs/logmessage/logmessage.h>
#include <herbs/stringsys/stringsys.h>
#include <herbs/intformat/intformat.h>

LatestCopy::Runner::Runner(Herbs::LogWriter& log,Herbs::Directory&& options):
	Herbs::Application(log)
	{
	if(options.itemsCount()<3)
		{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
	auto items=options.itemsBegin();
	params.dir_source=Herbs::Path(items[0]);
	params.dir_dest=Herbs::Path(items[1]);
	params.size_max=Herbs::toInt<uintmax_t>(items[2].begin())*1024*1024;
	}


void LatestCopy::Runner::filesCollect()
	{
	dirs.clear();
	Herbs::FilesEnumerator files(params.dir_source);
	Herbs::FilesEnumerator::FileInfo info;
	while(files.nextGet(info))
		{
		if(info.flags&Herbs::FilesEnumerator::FileInfo::DIRECTORY
			&& !info.backref())
			{
			sysout().write(
				{
				Herbs::format(STR("Skannar \"%0\" "),
					{
					Herbs::bufferLocPtr(Herbs::stringloc(info.name))
					}).begin()
				,Herbs::LogMessage::Type::INFORMATION
				});
			dirs.append(Direntry(info,params.dir_source));
			}
		}
	std::sort(dirs.begin(),dirs.end());
	}

void LatestCopy::Runner::filesCopy()
	{
	auto ptr=dirs.begin();
	uintmax_t sum=0;
	while(ptr!=dirs.end())
		{
		sum+=ptr->sizeGet();
		if(sum>params.size_max)
			{break;}

		sysout().write(
			{
			 Herbs::format(STR("Kopierar \"%0\" (%1 %2 %3)"),
				{
				Herbs::String(ptr->nameGet()).begin()
				,Herbs::IntFormat<uintmax_t>(ptr->sizeGet())
				,Herbs::IntFormat<uintmax_t>(sum)
				,Herbs::IntFormat<uintmax_t>(params.size_max)
				}).begin()
			 ,Herbs::LogMessage::Type::INFORMATION
			});

		if(!ptr->copy(params.dir_dest,sysout()))
			{sysout().write(STR("Det gick inte at kopiera filen"));}
		++ptr;
		}
	}

int LatestCopy::Runner::run()
	{
	filesCollect();
	filesCopy();
	return STATUS_OK;
	}

void LatestCopy::Runner::destroy()
	{
	delete this;
	}