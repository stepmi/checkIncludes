#include "setConfiguration.h"
#include "system/logger.h"
#include "tools/strings.h"
#ifdef  _WIN32
	#include "msvc/CMsProjectFileQuery.h"
#endif


namespace projectFile
{
	bool setConfiguration(CParameters &a_parameters)
	{
		if (a_parameters.getProjectType() == EProjectType::eMsBuild)
		{
#ifdef  _WIN32
			msvc::CMsProjectFileQuery msProjectFileQuery;
			if (!msProjectFileQuery.load(a_parameters.getProject()))
			{
				logger::add(logger::EType::eError, "Couldn't read " + PLATFORM_TOSTRING(a_parameters.getProject()));
				return false;
			}

			const std::vector<CProjectConfiguration> projectConfigurations = msProjectFileQuery.getProjectConfigurations();
			for (auto &projectConfiguration : projectConfigurations)
			{
				if (a_parameters.getProjectConfiguration().m_sConfiguration.empty() && a_parameters.getProjectConfiguration().m_sPlatform.empty())
				{
					a_parameters.setProjectConfiguration(projectConfiguration);
					return true;
				}
				else if (!a_parameters.getProjectConfiguration().m_sConfiguration.empty() && !a_parameters.getProjectConfiguration().m_sPlatform.empty())
				{
					if (tools::strings::compareCaseInsensitive(projectConfiguration.m_sConfiguration, a_parameters.getProjectConfiguration().m_sConfiguration) &&
						tools::strings::compareCaseInsensitive(projectConfiguration.m_sPlatform, a_parameters.getProjectConfiguration().m_sPlatform))
					{
						a_parameters.setProjectConfiguration(projectConfiguration);
						return true;
					}
				}
				else if (!a_parameters.getProjectConfiguration().m_sConfiguration.empty())
				{
					if (tools::strings::compareCaseInsensitive(projectConfiguration.m_sConfiguration, a_parameters.getProjectConfiguration().m_sConfiguration))
					{
						a_parameters.setProjectConfiguration(projectConfiguration);
						return true;
					}
				}
				else if (!a_parameters.getProjectConfiguration().m_sPlatform.empty())
				{
					if (tools::strings::compareCaseInsensitive(projectConfiguration.m_sPlatform, a_parameters.getProjectConfiguration().m_sPlatform))
					{
						a_parameters.setProjectConfiguration(projectConfiguration);
						return true;
					}
				}
			}

			logger::add(logger::EType::eError, "Couldn't find project configuration for " + PLATFORM_TOSTRING(a_parameters.getProject()));
			return false;
#endif
		}

		return false;
	}

}