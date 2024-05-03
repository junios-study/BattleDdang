// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	AZombie* Zombie = Cast<AZombie>(TryGetPawnOwner());
	if (Zombie && Zombie->IsValidLowLevel())
	{
		CurrentState = Zombie->CurrentState;
		Speed = Zombie->GetCharacterMovement()->Velocity.Size();
	}

}

void UZombieAnimInstance::AnimNotify_CheckAttack2(UAnimNotify* Notify)
{
	UE_LOG(LogClass, Warning, TEXT("Attack2"));
}
