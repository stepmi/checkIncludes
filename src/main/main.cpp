#include <string>
#include "system/exit.h"
#include "compiler/createCompiler.h"
#include "system/logger.h"
#include <chrono>
#include "tools/strings.h"
#include "main/CParameters.h"
#include "tools/platforms.h"
#include "commandline.h"
#include "projectFile/projectFile.h"

// TODO: 
// cmake add filters

// better logging options
// check with qt project
// idea for precompile to handle #ifdef: add fake lines after each include to see if they are in the precompiled result file


#ifdef _WIN32
int wmain(int argc, wchar_t* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	using TIME_POINT = std::chrono::time_point<std::chrono::system_clock>;
	const TIME_POINT tpStart = std::chrono::system_clock::now();

	// read parameters
	std::vector<platform::string> params;
	for (int i = 1; i < argc; i++)
		params.push_back(argv[i]);
	auto upParameter = parseCommandLine(params);
	if (upParameter)
	{
		// all logging is be done in function code	
		auto upCompiler = compiler::createCompiler(upParameter->getCompilerType());
		if (upCompiler)
		{
			exitHandler::initExitHandler();
			projectFile::checkProject(*upCompiler, *upParameter);

			const TIME_POINT tpEnd = std::chrono::system_clock::now();
			auto sDuration = tools::strings::secondsToHours(std::chrono::duration_cast<std::chrono::seconds>(tpEnd - tpStart).count());
			logger::add(logger::EType::eMessage, "Time taken: " + sDuration + " h");
		}		
	}

	return 0;
}
