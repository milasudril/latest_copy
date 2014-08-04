#ifdef __WAND__
target[name[direntry.o] type[object]]
#endif

#include "direntry.h"
#include <herbs/stringsys/stringsys.h>
#include <herbs/stack/stack.h>
#include <herbs/fileutils/fileutils.h>

#include <herbs/stringformat/stringformat.h>
#include <herbs/logmessage/logmessage.h>
#include <herbs/stringsys/stringsys.h>
#include <herbs/intformat/intformat.h>
#include <herbs/logwriter/logwriter.h>

LatestCopy::Direntry::Direntry(const Herbs::FilesEnumerator::FileInfo& fileinfo
	,const Herbs::Path& in_dir)
	:m_in_dir(in_dir),filename(Herbs::stringloc(fileinfo.name))
	{
	valuesCompute(fileinfo);
	}

bool LatestCopy::Direntry::copy(const Herbs::Path& dest,Herbs::LogWriter& log)
	{
	Herbs::Stack<Herbs::Path> stack(64);
	stack.push(m_in_dir>>filename);
	while(stack.depth())
		{
		auto in_dir=stack.pop();

		auto fullpath=dest>>(in_dir.makeRelative(m_in_dir));
		Herbs::FilesEnumerator::FileInfo info;
		if(!Herbs::FilesEnumerator::fileInfoGet(fullpath,info))
			{
			log.write(
				{
				 Herbs::format(STR("Skapar katalog \"%0\"")
					,{
					Herbs::String(fullpath).begin()
					}).begin()
				,Herbs::LogMessage::Type::INFORMATION
				});

			if(!Herbs::Fileutils::mkdir(fullpath))
				{return 0;}
			}

		Herbs::FilesEnumerator files(in_dir);
		while(files.nextGet(info))
			{
			Herbs::Path name(Herbs::stringloc(info.name));

			if(info.flags&Herbs::FilesEnumerator::FileInfo::DIRECTORY)
				{
				if(!info.backref())
					{stack.push(in_dir>>name);}
				}
			else
				{
				auto filename_remote=fullpath>>name;
				auto filename_local=in_dir>>name;

				const Herbs::Path* src=nullptr;
				const Herbs::Path* dest=nullptr;

				Herbs::FilesEnumerator::FileInfo info_new;
				if(Herbs::FilesEnumerator::fileInfoGet(filename_remote,info_new))
					{
					if(info.time_modified < info_new.time_modified)
						{
						src=&filename_local;
						dest=&filename_remote;
						}
					if(info.time_modified > info_new.time_modified)
						{
						src=&filename_remote;
						dest=&filename_local;
						}
					}
				else
					{
					src=&filename_local;
					dest=&filename_remote;
					}

				if(src!=nullptr)
					{
					log.write(
						{
						Herbs::format(STR("Kopierar \"%0\" till \"%1\"")
							,{
							Herbs::String(*src).begin()
							,Herbs::String(*dest).begin()
							}).begin()
						,Herbs::LogMessage::Type::INFORMATION
						});
					if(!Herbs::Fileutils::copy(*src,*dest))
						{return 0;}
					}
				else
					{
					log.write(
						{
						Herbs::format(STR("Hoppar över \"%0\" ")
							,{
							Herbs::String(filename_local).begin()
							}).begin()
						,Herbs::LogMessage::Type::INFORMATION
						});

					}
				}
			}
		}
	return 1;
	}

void LatestCopy::Direntry::valuesCompute(const Herbs::FilesEnumerator::FileInfo& fileinfo)
	{
	time_modified=0;
	size=fileinfo.size;

	Herbs::Stack<Herbs::Path> stack(64);
	stack.push(m_in_dir>>filename);
	while(stack.depth())
		{
		auto in_dir=stack.pop();
		Herbs::FilesEnumerator files(in_dir);
		Herbs::FilesEnumerator::FileInfo info;
		while(files.nextGet(info))
			{
			Herbs::Path name(Herbs::stringloc(info.name));

			if(info.flags&Herbs::FilesEnumerator::FileInfo::DIRECTORY)
				{
				if(!info.backref())
					{
					stack.push(in_dir>>name);
					size+=info.size;
					}
				}
			else
				{
				time_modified=std::max(info.time_modified,time_modified);
				size+=info.size;
				}
			}
		}
	}