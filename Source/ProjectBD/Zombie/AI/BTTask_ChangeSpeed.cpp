// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeSpeed.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "../Zombie.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_ChangeSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = Cast<AAIController>(OwnerComp.GetAIOwner());

    if (AIC)
    {
        AZombie* Zombie = AIC->GetPawn<AZombie>();
        if (Zombie)
        {
            Zombie->SetSpeed(Speed);   

            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}
