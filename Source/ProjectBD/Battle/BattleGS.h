// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleGS.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API ABattleGS : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = "OnRep_AlivePlayerCount")
	int AlivePlayerCount = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	int TotalPlayerCount = 0;

	UFUNCTION()
	void OnRep_AlivePlayerCount();

	virtual	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
};
