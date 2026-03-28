#include "CoreSaveModule.h"

IMPLEMENT_MODULE(FCoreSaveModule, CoreSave)

void FCoreSaveModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("CoreSave module started"));
}

void FCoreSaveModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("CoreSave module shut down"));
}