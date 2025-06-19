#pragma once

#include "Engine/GameInstance.h"
#include "Subsystems/EngineSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Subsystems/WorldSubsystem.h"

#include "SubsystemHelper.generated.h"

/**
 * @class USubsystemHelper
 * @brief A utility class for retrieving various subsystem instances based on their type and the provided context.
 *
 * USubsystemHelper is a static helper class that simplifies the process of retrieving subsystems
 * associated with the given context. Subsystems can be of types derived from UGameInstanceSubsystem,
 * UWorldSubsystem, UEngineSubsystem, or ULocalPlayerSubsystem.
 *
 * This class is designed to be used within Unreal Engine projects and provides a single templated
 * method to retrieve subsystems dynamically while ensuring type safety.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API USubsystemHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template <typename T>
	static T* GetSubsystem(const UObject* Context)
	{
		static_assert(TIsDerivedFrom<T, USubsystem>::Value, "T must inherit from USubsystem");

		if (!Context)
		{
			return nullptr;
		}

		// Get the appropriate subsystem based on its type
		if constexpr (TIsDerivedFrom<T, UGameInstanceSubsystem>::Value)
		{
			if (const UGameInstance* GameInstance = Context->GetWorld()->GetGameInstance())
			{
				return GameInstance->GetSubsystem<T>();
			}
		}
		else if constexpr (TIsDerivedFrom<T, UWorldSubsystem>::Value)
		{
			if (const UWorld* World = Context->GetWorld())
			{
				return World->GetSubsystem<T>();
			}
		}
		else if constexpr (TIsDerivedFrom<T, UEngineSubsystem>::Value)
		{
			if (GEngine)
			{
				return GEngine->GetEngineSubsystem<T>();
			}
		}
		else if constexpr (TIsDerivedFrom<T, ULocalPlayerSubsystem>::Value)
		{
			if (const UWorld* World = Context->GetWorld())
			{
				if (const UGameInstance* GameInstance = World->GetGameInstance())
				{
					if (const APlayerController* PC = GameInstance->GetFirstLocalPlayerController())
					{
						if (const ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
						{
							return LocalPlayer->GetSubsystem<T>();
						}
					}
				}
			}
		}

		return nullptr;
	}
};