// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealCoreFramework : ModuleRules
{
	public UnrealCoreFramework(ReadOnlyTargetRules Target) : base(Target)
	{
		//IWYUSupport = IWYUSupport.Full;
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);

		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"ApplicationCore",
				"Engine",
				"HTTP",
				"InputCore",
				"Slate",
				"UMG",
				"CommonUI",
				"Json",
				"JsonUtilities",
				"EnhancedInput",
				"ModelViewViewModel",
				"FieldNotification",
				"AsyncFlow",
				"CoreTween"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"DeveloperSettings",
				"Engine",
				"Slate",
				"SlateCore",
				"OnlineSubsystemUtils"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}