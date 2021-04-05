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
	};
		
	logger::add(logger::EType::eMessage, msg);
}

bool checkParseResult(const CParameters &a_parameters)
{
	const auto eMsgType = logger::EType::eError;
	if (PLATFORM_TOSTRING(a_parameters.getProject()).find(".vcxproj") == std::string::npos)
	{
		logger::add(eMsgType, PLATFORM_TOSTRING(a_parameters.getProject()) + " is not a valid project file.");
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
	if (sArgument.size() == 4 && (sArgument.find("-l:") == 0 || sArgument.find("-L:") == 0))
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
		auto upResult = std::make_unique<CParameters>(a_arguments.back());
		bool bParseOk = true;
		for (size_t i = 0; i < a_arguments.size() - 1; i++)
		{
			const auto &sArgument = a_arguments[i];
			if (!parseArgument(sArgument, *upResult))			
				bParseOk = false;				
		}

		if (!bParseOk)
			printHelp();

		if (checkParseResult(*upResult))
		{
			return upResult;
		}
	}
	printHelp();
	return std::unique_ptr<CParameters>(nullptr);
}

