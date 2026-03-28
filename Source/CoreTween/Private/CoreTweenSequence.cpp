#include "CoreTweenSequence.h"

FCoreTweenSequence FCoreTweenSequence::Create()
{
	return FCoreTweenSequence();
}

FCoreTweenSequence& FCoreTweenSequence::Then(FCoreTweenBuilder InBuilder)
{
	Steps.Add(MoveTemp(InBuilder));
	return *this;
}

AsyncFlow::TTask<void> FCoreTweenSequence::Run(UObject* WorldContext)
{
	TArray<FCoreTweenBuilder> LocalSteps = MoveTemp(Steps);
	for (FCoreTweenBuilder& Step : LocalSteps)
	{
		co_await Step.Run(WorldContext);
	}
}

