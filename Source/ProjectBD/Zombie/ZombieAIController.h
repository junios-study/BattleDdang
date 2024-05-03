// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "Zombie.h"

#include "ZombieAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API AZombieAIController : public AAIController
{
	GENERATED_BODY()
public:

	AZombieAIController();
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<class UBehaviorTreeComponent> BTComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<class UBlackboardComponent> BBComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UAIPerceptionComponent> Perception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;


	UFUNCTION()
	void ProcessPerceptionUpdate(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void ProcessTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);


	void SetCurrentState(EZombieState NewState);

	void SetTarget(FVector NewLocation);

	void SetPlayer(AActor* Player);
};
