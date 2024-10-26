#include "Integrators/SequenceSdkBP.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Util/Log.h"

USequenceSdkBP::USequenceSdkBP() { }

USequenceSdkBP* USequenceSdkBP::GetSubSystem()
{
	if (GEngine)
	{
		const TIndirectArray<FWorldContext> Contexts = GEngine->GetWorldContexts();
		for (FWorldContext Context : Contexts)
		{
			if (const UWorld* World = Context.World())
			{
				if (const UGameInstance* GI = UGameplayStatics::GetGameInstance(World))
				{
					if (USequenceSdkBP* Subsystem = GI->GetSubsystem<USequenceSdkBP>())
					{
						return Subsystem;
					}
				}
			}
		}
	}
	else
	{
		SEQ_LOG(Error,TEXT("Error Accessing GEngine"));
	}
	
	SEQ_LOG(Error,TEXT("Error Accessing USequenceWallet GameInstanceSubSystem"));
	return nullptr;
}

void USequenceSdkBP::SetupAsync()
{
	// TODO: Integrate async setup process like querying remote chain configs
	this->CallSetupFinished();
}

void USequenceSdkBP::CallSetupFinished() const
{
	if (this->SetupFinished.IsBound())
		this->SetupFinished.Broadcast();
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: SetupFinished"));
}
