// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Print.h"

void UBTService_Print::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UE_LOG(LogClass, Warning, TEXT("BTService_Print Tick"));
}
