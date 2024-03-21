#pragma once
#include "SequencePlugin/Public/Authenticator.h"
#include "CoreMinimal.h"
#include "IOSBridge.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UIOSBridge : public UObject
{
	GENERATED_BODY()

public:
	UIOSBridge();
	static void InitiateIosSSO(const FString& providerUrl, void(*IOSCallback)(char *idToken));
};