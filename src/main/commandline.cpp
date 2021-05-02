#include "commandline.h"
#include "system/logger.h"
#include "tools/filename.h"
#include "tools/strings.h"

void printHelp()
{
	const std::vector<std::string> msg =
	{		
		"checkIncludes [options] projectfile",
		"",
		"options:",		
		"   -c:x    configuration for build process. x is Debug, Release, etc",
		"   -p:x    platform for build process. x is x64, x86, etc",		
		"   -i:x    ignore include. don't check if the specified header(s) x can be removed",
		"   -s:x    specify compile file. check the specified files(s) x only",
		"   -r:x    remove compile file. ignore the specified files(s) x",
		"   -l:f    logging, show all processed files",
		"   -l:i    logging, show checked includes for all processed files",
		"   -l:c    logging, show command lines for all processed files",
		"   -l:o    logging, show compiler output - this produces a lot of output",		
		"   -h      print help text",
		
		"example:",
		">checkIncludes -p:x64 -c:Release -r:tools/enumFiles.cpp -i:vector.h checkIncludes.vcxproj",
		""
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
	else if (a_parameters.getProjectType() == EProjectType::eUnknown)
	{
		logger::add(logger::EType::eError, PLATFORM_TOSTRING(a_parameters.getProject()) + " is not a valid project file.");
		return false;
	}

	if (a_parameters.getProjectType() == EProjectType::eMakeFile) // TODO
		logger::add(logger::EType::eWarning, "Checking projects with makefiles is under development for now.");

	return true;
}

	
std::string getArgumentValue(const std::string &a_sArgument)
{
	const size_t iValueStart = 3;
	return a_sArgument.substr(iValueStart);	
}

EProjectType getProjectTypeFromFileName(const std::string &a_sFileName)
{
	const std::string sExtension = std::filesystem::path(a_sFileName).extension().string();
	const std::string sFileName = std::filesystem::path(a_sFileName).stem().string();
	if (tools::strings::compareCaseInsensitive(sExtension, ".vcxproj"))
		return EProjectType::eMsBuild;
	else if (tools::strings::compareCaseInsensitive(sExtension, ".mak") || 
		tools::strings::compareCaseInsensitive(sFileName, "makefile"))
		return EProjectType::eMakeFile;
	return EProjectType::eUnknown;
}

bool parseArgument(const std::string &a_sArgument, CParameters &a_parameters)
{	
	bool bArgumentOK = true;
	if (a_sArgument.size() > 0 && a_sArgument.front() != '-')
	{
		if (a_parameters.getProject().empty())
		{			
			a_parameters.setProjectType(getProjectTypeFromFileName(a_sArgument));
			a_parameters.setProject(a_sArgument);
		}
		else
		{
			logger::add(logger::EType::eError, "More than one project file specified.");
			bArgumentOK = false;
		}
	}
	else if (a_sArgument.size() == 4 && tools::strings::beginsWithCaseInsensitive(a_sArgument, "-l:"))
	{
		// logging options
		const char cOption = a_sArgument[3];
		if (tools::strings::compareCaseInsensitive(cOption, 'f'))
			logger::allowType(logger::EType::eProcessedFiles);
		else if (tools::strings::compareCaseInsensitive(cOption, 'i'))
			logger::allowType(logger::EType::eCheckedIncludes);
		else if (tools::strings::compareCaseInsensitive(cOption, 'c'))
			logger::allowType(logger::EType::eCommandLines);
		else if (tools::strings::compareCaseInsensitive(cOption, 't'))
			logger::allowType(logger::EType::eDebugThreads); // this isn't documented.	
		else if (tools::strings::compareCaseInsensitive(cOption, 'o'))
			a_parameters.addOption(EOption::eCompileLog);
		else		
			bArgumentOK = false;
	}
	else if (tools::strings::beginsWithCaseInsensitive(a_sArgument, "-p:"))
	{
		// platform
		auto configuration = a_parameters.getProjectConfiguration();
		configuration.m_sPlatform = getArgumentValue(a_sArgument);
		a_parameters.setProjectConfiguration(configuration);
	}
	else if (tools::strings::beginsWithCaseInsensitive(a_sArgument, "-c:"))
	{			
		// configuration
		auto configuration = a_parameters.getProjectConfiguration();
		configuration.m_sConfiguration = getArgumentValue(a_sArgument);
		a_parameters.setProjectConfiguration(configuration);
	}
	else if (tools::strings::beginsWithCaseInsensitive(a_sArgument, "-s:"))
	{
		// specify compile file
		auto sValue = getArgumentValue(a_sArgument);
		if (!sValue.empty())
			a_parameters.addCompileFile(sValue);
	}
	else if (tools::strings::beginsWithCaseInsensitive(a_sArgument, "-r:"))
	{
		// remove compile file
		auto sValue = getArgumentValue(a_sArgument);
		if (!sValue.empty())		
			a_parameters.addIgnoreCompileFile(sValue);		
	}
	else if (tools::strings::beginsWithCaseInsensitive(a_sArgument, "-i:"))
	{
		// ignore include file
		auto sValue = getArgumentValue(a_sArgument);
		if (!sValue.empty())		
			a_parameters.addIgnoreInclude(sValue);			
	}
	else if (tools::strings::beginsWithCaseInsensitive(a_sArgument, "-h:"))
	{
		a_parameters.setPrintHelp();
	}
	else	
		bArgumentOK = false;
		

	if (!bArgumentOK)
		logger::add(logger::EType::eWarning, "Unknown command line option " + tools::strings::getQuoted(a_sArgument));

	return bArgumentOK;
}

std::unique_ptr<CParameters> parseCommandLine(const std::vector<std::string> &a_arguments)
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

