#pragma once

#include <string>

namespace msvc
{

	// it seems there are multiple ways, in which a vcxproj-file can be structured:
	// - Visual Studio creates "PropertyGroup" with a condition that describes platform and configuration
	// - cmake creates "PropertyGroup" without a condition, but adds the condition to each child entry (like "IntDir")
	// we prefer the Visual Studio structure and remove all other entries

	static const std::string sTagProject = "Project";
		static const std::string sTagPropertyGroup = "PropertyGroup";
			static const std::string sTagIntDir = "IntDir";
			static const std::string sAttCondition = "Condition";
	static const std::string sTagItemDefinitionGroup = "ItemDefinitionGroup";
		static const std::string sTagClCompile = "ClCompile";
			static const std::string sTagAdditionalOptions = "AdditionalOptions";
			static const std::string sTagPrecompiledHeader = "PrecompiledHeader";
			static const std::string sTagProgramDataBaseFileName = "ProgramDataBaseFileName";
	static const std::string sTagItemGroup = "ItemGroup";
		//static const std::string sTagClCompile = "ClCompile";
			//static const std::string sTagPrecompiledHeader = "PrecompiledHeader";
			static const std::string sAttInclude = "Include";
		static const std::string sTagProjectConfiguration = "ProjectConfiguration";
			static const std::string sTagConfiguration = "Configuration";
			static const std::string sTagPlatform = "Platform";

}