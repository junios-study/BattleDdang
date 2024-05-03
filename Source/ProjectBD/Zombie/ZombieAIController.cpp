// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Zombie.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AZombieAIController::AZombieAIController()
{
	BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTComponent"));
	BBComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBComponent"));

	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));

	SetPerceptionComponent(*Perception);

	SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	//SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));


	SightConfig->SightRadius = 500.0f;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + 50.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 900.0f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;




	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);



}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//AIController가 Zombie를 잡으면 그때 BT정보를 초기화 해서 사용
	AZombie* Zombie = Cast<AZombie>(InPawn);
	if (Zombie)
	{
		if (Zombie->ZombieBT && Zombie->ZombieBT->BlackboardAsset)
		{
			BBComponent->InitializeBlackboard(*(Zombie->ZombieBT->BlackboardAsset));
			BTComponent->StartTree(*(Zombie->ZombieBT));
		}

		GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AZombieAIController::ProcessPerceptionUpdate);
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AZombieAIController::ProcessTargetPerceptionUpdate);
	}

}

//그림 으로 AI 만드는 툴 제공, Behavior Tree
//void AZombieAIController::Tick()
//{
//	Tick();
//
//	1. 눈에 보이면 따라간다.
//	2. 가까우면 때린다.
//	3. 도망 가면 쫓아간다.
//}

void AZombieAIController::OnUnPossess()
{
	UE_LOG(LogClass, Warning, TEXT("OnUnPossess"));
	//BTComponent->StopTree();

	Super::OnUnPossess();
}

void AZombieAIController::SetCurrentState(EZombieState NewState)
{
	if (BBComponent)
	{
		BBComponent->SetValueAsEnum(TEXT("CurrentState"), (uint8)NewState);
	}
}

void AZombieAIController::SetTarget(FVector NewLocation)
{
	if (BBComponent)
	{
		BBComponent->SetValueAsVector(TEXT("Target"), NewLocation);
	}
}

void AZombieAIController::SetPlayer(AActor* Player)
{
	if (BBComponent)
	{
		BBComponent->SetValueAsObject(TEXT("Player"), Player);
	}
}


void AZombieAIController::ProcessPerceptionUpdate(const TArray<AActor*>& UpdatedActors)
{
	UE_LOG(LogTemp, Warning, TEXT("ProcessPerceptionUpdate"));
	for (auto Actor : UpdatedActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Actor->GetName());
	}
}

void AZombieAIController::ProcessTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	UE_LOG(LogTemp, Warning, TEXT("ProcessTargetPerceptionUpdate"));

	UE_LOG(LogTemp, Warning, TEXT("%s"), *Actor->GetName());

	AZombie* Zombie = Cast<AZombie>(GetPawn());

	if (Zombie)
	{
		if (Zombie->CurrentState == EZombieState::Normal)
		{
			UE_LOG(LogClass, Warning, TEXT("See %s"), *Actor->GetName());
			SetCurrentState(EZombieState::Chase);

			//Set Blackboard Value
			SetPlayer(Actor);
		}
	}
}