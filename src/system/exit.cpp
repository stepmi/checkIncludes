#include "system/exit.h"
#include <set>
#include "tools/filesystem.h"
#include <mutex>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif

namespace exitHandler
{
	using FILES = std::set<platform::string>; 
	FILES global_pathsToDelete;
	std::mutex mutex_pathsToDelete;

	void deleteFiles()
	{
		FILES pathsToDeleteCopy;
		{
			// only lock the code, which accesses global_pathsToDelete
			// and keep it short - who knows, what situations may occur here
			std::lock_guard<std::mutex> guard(mutex_pathsToDelete);
			pathsToDeleteCopy = global_pathsToDelete;
			global_pathsToDelete.clear();
		}

		// restore files, that were already in use		
		for (auto &wsPath : pathsToDeleteCopy)
		{
			tools::filesystem::removeAll(wsPath);
		}		
	}

	void atexit_handler()
	{
		deleteFiles();
	}

	void at_quick_exit_handler()
	{
		deleteFiles();
	}

#ifdef _WIN32

	BOOL WINAPI CtrlHandler(const DWORD a_fdwCtrlType)
	{
		Beep(750, 300);
		if (a_fdwCtrlType == CTRL_C_EVENT)
		{
			atexit_handler();
			//Beep(750, 300);
			return TRUE;
		}
		else if (a_fdwCtrlType == CTRL_CLOSE_EVENT)
		{			
			atexit_handler();
			return TRUE;
		}
		else
		{
			// Pass other signals to the next handler.	
			return FALSE;
		}
	}

#endif


	void initExitHandler()
	{
		std::atexit(atexit_handler);
		std::at_quick_exit(at_quick_exit_handler);
#ifdef _WIN32
		SetConsoleCtrlHandler(CtrlHandler, TRUE);
#endif
	}

	void add(const platform::string &a_wsPath)
	{
		std::lock_guard<std::mutex> guard(mutex_pathsToDelete);
		global_pathsToDelete.insert(a_wsPath);
	}
	void remove(const platform::string &a_wsPath)
	{
		std::lock_guard<std::mutex> guard(mutex_pathsToDelete);
		auto it = global_pathsToDelete.find(a_wsPath);
		if (it != global_pathsToDelete.end())
			global_pathsToDelete.erase(it);
	}

}

