#include "commandline.h"
#include "system/logger.h"
#include "tools/filename.h"
#include "tools/strings.h"

void printHelp()
{
	std::vector<std::string> msg =
	{		
		"checkIncludes [options] projectfile",
		"",
		"options:",
		"   -h      print help text",
		"   -p:X    platform for build process, X is x64, x86, ...",
		"   -c:X    configuration for build process, X is Debug, Release, ...",
		"   -l:f    show all processed files",
		"   -l:i    show checked includes for all processed files",
		"   -l:c    show command lines for all processed files",
		"   -l:r    show compiler output - this produces a lot of output",
		"   -o:X    check only this file, X is a c/cpp file",
		"   -i:X    ignore file, X may be a c/cpp file or a header",
		"           if X is a c/cpp file it is not checked",
		"           if X is a header, it isn't checked if this header can be removed",
		"example:",
		">checkIncludes -p:x64 -c:Release -i:tools/enumFiles.cpp -i:vector.h checkIncludes.vcxproj",
		""

		// TODO: implement new options (c+, c-).
		/*"checkIncludes [options] projectfile",
		"",
		"options:",		
		"   -c:x    configuration for build process, x is Debug, Release, etc",
		"   -p:x    platform for build process, x is x64, x86, etc",		
		"   -i:x    ignore include, it isn't checked if the specified header(s) x can be removed",
		"   -c+:x   compile file add, check the specified files(s) x only",
		"   -c-:x   compile file ignore, ignore the specified files(s) x",
		"   -l:f    logging, show all processed files",
		"   -l:i    logging, show checked includes for all processed files",
		"   -l:c    logging, show command lines for all processed files",
		"   -l:r    logging, show compiler output - this produces a lot of output",		
		"   -h      print help text",
		
		"example:",
		">checkIncludes -p:x64 -c:Release -i:tools/enumFiles.cpp -i:vector.h checkIncludes.vcxproj",
		""*/
	};
		
	logger::add(logger::EType::eMessage, msg);
}

bool checkParseResult(const CParameters &a_parameters)
{	
	if (a_parameters.getProject().empty())
	{
		logger::add(logger::EType::eError, "Missing project file.");
		return false;
	}
	else if (PLATFORM_TOSTRING(a_parameters.getProject()).find(".vcxproj") == std::string::npos)
	{
		logger::add(logger::EType::eError, PLATFORM_TOSTRING(a_parameters.getProject()) + " is not a valid project file.");
		return false;
	}
	return true;
}

	
std::string getArgumentValue(const std::string &a_sArgument)
{
	const size_t iValueStart = 3;
	return a_sArgument.substr(iValueStart);	
}

bool parseArgument(const platform::string &a_sArgument, CParameters &a_parameters)
{
	const std::string sArgument = PLATFORM_TOSTRING(a_sArgument);	
	bool bArgumentOK = true;
	if (sArgument.size() > 0 && sArgument.front() != '-')
	{
		if (a_parameters.getProject().empty())
			a_parameters.setProject(sArgument);
		else
		{
			logger::add(logger::EType::eError, "More than one project file specified.");
			bArgumentOK = false;
		}
	}
	else if (sArgument.size() == 4 && (sArgument.find("-l:") == 0 || sArgument.find("-L:") == 0))
	{
		if (sArgument[3] == 'f' || sArgument[3] == 'F')
			logger::allowType(logger::EType::eProcessedFiles);
		else if (sArgument[3] == 'i' || sArgument[3] == 'I')
			logger::allowType(logger::EType::eCheckedIncludes);
		else if (sArgument[3] == 'c' || sArgument[3] == 'C')
			logger::allowType(logger::EType::eCommandLines);
		else if (sArgument[3] == 't' || sArgument[3] == 'T')
			logger::allowType(logger::EType::eDebugThreads); // this isn't documented.	
		else if (sArgument[3] == 'r' || sArgument[3] == 'R')
			a_parameters.addOption(EOption::eCompileLog);
		else		
			bArgumentOK = false;
	}
	else if (sArgument.find("-p:") == 0 || sArgument.find("-P:") == 0)
	{
		auto configuration = a_parameters.getProjectConfiguration();
		configuration.m_sPlatform = getArgumentValue(sArgument);
		a_parameters.setProjectConfiguration(configuration);
	}
	else if (sArgument.find("-c:") == 0 || sArgument.find("-C:") == 0)
	{			
		auto configuration = a_parameters.getProjectConfiguration();
		configuration.m_sConfiguration = getArgumentValue(sArgument);
		a_parameters.setProjectConfiguration(configuration);
	}
	else if (sArgument.find("-o:") == 0 || sArgument.find("-O:") == 0)
	{
		auto sValue = getArgumentValue(sArgument);
		if (!sValue.empty())
			a_parameters.addCompileFile(sValue);
	}
	else if (sArgument.find("-i:") == 0 || sArgument.find("-I:") == 0)
	{
		auto sValue = getArgumentValue(sArgument);
		if (!sValue.empty())
		{
			if (tools::filename::isIncludeFile(sValue))
				a_parameters.addIgnoreInclude(sValue);
			else
				a_parameters.addIgnoreCompileFile(sValue);
		}
	}
	else if (sArgument.find("-h") == 0 || sArgument.find("-H") == 0)
	{
		a_parameters.setPrintHelp();
	}
	else	
		bArgumentOK = false;
		

	if (!bArgumentOK)
		logger::add(logger::EType::eWarning, "Unknown command line option " + tools::strings::getQuoted(sArgument));

	return bArgumentOK;
}

std::unique_ptr<CParameters> parseCommandLine(const std::vector<platform::string> &a_arguments)
{
	if (!a_arguments.empty())
	{		
		std::unique_ptr<CParameters> upResult = std::make_unique<CParameters>();
		bool bParseOk = true;
		for (const auto &sArgument : a_arguments)
		{			
			if (!parseArgument(sArgument, *upResult))
				bParseOk = false;			
		}

		if (!bParseOk || upResult->getPrintHelp())
			printHelp();
		else if (checkParseResult(*upResult))		
			return upResult;

		return std::unique_ptr<CParameters>(nullptr);
	}
	printHelp();
	return std::unique_ptr<CParameters>(nullptr);
}

