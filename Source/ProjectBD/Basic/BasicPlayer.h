// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BasicPlayer.generated.h"

UCLASS()
class PROJECTBD_API ABasicPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UCameraComponent* Camera;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUp(float AxisValue);
	void Turn(float AxisValue);

	void Sprint();
	void StopSprint();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float WalkSpeed = 300.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float SprintSpeed = 600.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UWeaponComponent* Weapon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="Status")
	uint64 bIsFire : 1;

	UFUNCTION(Server, Reliable)
	void C2S_SetFire(bool State);
	void C2S_SetFire_Implementation(bool State);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
	uint64 bIsSprint : 1;

	UFUNCTION(Server, Reliable)
	void C2S_SetSprint(bool State);
	void C2S_SetSprint_Implementation(bool State);


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
	uint64 bIsIronsight : 1;

	UFUNCTION(Server, Reliable)
	void C2S_SetIronsight(bool State);
	void C2S_SetIronsight_Implementation(bool State);

	void StartFire();
	void StopFire();

	void OnFire();

	void StartIronsight();
	void StopIronsight();

	void StartCrouch();


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="SpringArm")
	FVector NormalSpringArmPosition;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SpringArm")
	FVector CrouchedSpringArmPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
	TArray<TEnumAsByte<EObjectTypeQuery>> TestObjects;

	FTimerHandle FireTimerHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UParticleSystem* HitEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UParticleSystem* BloodHitEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class USoundBase* WeaponSound;


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing="OnRep_CurrentHP", Category = "Data")
	float CurrentHP = 100.0f;
	
	UFUNCTION()
	void OnRep_CurrentHP();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Data")
	float MaxHP = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UAnimMontage* DeadMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UAnimMontage* ReloadMontage;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UAnimMontage* HitActionMontage;


	void Reload();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UMaterialInstance* NormalDecal;

	void StartLeftLean();
	void StopLeftLean();

	void StartRightLean();
	void StopRightLean();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
	uint8 bLeftLean : 1;

	UFUNCTION(Server, Reliable)
	void C2S_SetLeftLean(bool State);
	void C2S_SetLeftLean_Implementation(bool State);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
	uint8 bRightLean : 1;

	UFUNCTION(Server, Reliable)
	void C2S_SetRightLean(bool State);
	void C2S_SetRightLean_Implementation(bool State);

	FRotator GetAimOffset() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	TSubclassOf<UCameraShakeBase> WorldCameraShake;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void C2S_ProcessFire(FVector TraceStart, FVector TraceEnd);
	void C2S_ProcessFire_Implementation(FVector TraceStart, FVector TraceEnd);

	UFUNCTION(NetMulticast, Unreliable)
	void S2A_SpawnMuzzleFlashAndSound();
	void S2A_SpawnMuzzleFlashAndSound_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void S2A_SpawnHitEffectAndDecal(FHitResult OutHit);
	void S2A_SpawnHitEffectAndDecal_Implementation(FHitResult OutHit);


	UFUNCTION(NetMulticast, Reliable)
	void S2A_HitAction(int Number);
	void S2A_HitAction_Implementation(int Number);

	UFUNCTION(NetMulticast, Reliable)
	void S2A_DeadAction(int Number);
	void S2A_DeadAction_Implementation(int Number);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Status")
	uint8 bIsReload : 1;

	UFUNCTION(Server, Reliable)
	void C2S_SetReload(bool newState);
	void C2S_SetReload_Implementation(bool newState);

	TArray<class AMasterItem*> PickItemList;

	void AddPickItem(class AMasterItem* AddItem);
	void RemovePickItem(class AMasterItem* RemoveItem);

	void Pickup();

	
	UFUNCTION(Server, Reliable)
	void C2S_CheckPickupItem(class AMasterItem* PickupItem);
	void C2S_CheckPickupItem_Implementation(class AMasterItem* PickupItem);

	UFUNCTION(Client, Reliable)
	void S2C_InsertItem(class AMasterItem* PickupItem);
	void S2C_InsertItem_Implementation(class AMasterItem* PickupItem);

	TArray<class AMasterItem*> Inventory;

	void AddInventory(class AMasterItem* Item);
	void RemoveInventory(class AMasterItem* Item);
};
