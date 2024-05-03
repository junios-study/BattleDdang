// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameframeWork/CharacterMovementComponent.h"
#include "WeaponComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BulletDamageType.h"
#include "Components/DecalComponent.h"
#include "MyCameraShake.h"
#include "Net/UnrealNetwork.h"
#include "../Battle/BattleWidgetBase.h"
#include "../Battle/BattlePC.h"
#include "../Battle/BattleGM.h"
#include "../Item/MasterItem.h"
#include "Engine/DamageEvents.h"


// Sets default values
ABasicPlayer::ABasicPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	SpringArm->SocketOffset = FVector(0, 40.0f, 88.f);
	SpringArm->TargetArmLength = 120.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocation(FVector(0, 0, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
	GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0));


	Weapon = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCharacterMovement()->SetCrouchedHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());

	NormalSpringArmPosition = SpringArm->GetRelativeLocation();
	CrouchedSpringArmPosition = NormalSpringArmPosition + FVector(0, 0, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() / 2);

	Tags.Add(TEXT("Player"));
}

// Called when the game starts or when spawned
void ABasicPlayer::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;

	OnRep_CurrentHP();

}

// Called every frame
void ABasicPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABasicPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABasicPlayer::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABasicPlayer::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABasicPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABasicPlayer::LookUp);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ABasicPlayer::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ABasicPlayer::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ABasicPlayer::Sprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ABasicPlayer::StopSprint);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ABasicPlayer::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ABasicPlayer::StopFire);

	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Pressed, this, &ABasicPlayer::StartIronsight);
	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Released, this, &ABasicPlayer::StopIronsight);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ABasicPlayer::StartCrouch);


	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ABasicPlayer::Reload);


	PlayerInputComponent->BindAction(TEXT("LeftLean"), IE_Pressed, this, &ABasicPlayer::StartLeftLean);
	PlayerInputComponent->BindAction(TEXT("LeftLean"), IE_Released, this, &ABasicPlayer::StopLeftLean);

	PlayerInputComponent->BindAction(TEXT("RightLean"), IE_Pressed, this, &ABasicPlayer::StartRightLean);
	PlayerInputComponent->BindAction(TEXT("RightLean"), IE_Released, this, &ABasicPlayer::StopRightLean);


	PlayerInputComponent->BindAction(TEXT("Pickup"), IE_Pressed, this, &ABasicPlayer::Pickup);
}

void ABasicPlayer::MoveForward(float AxisValue)
{
	//절대회전, 카메라 같게 설정
	//GetControlRotation().Vector()
	//수학의 나라 (엔진)
	FVector CameraLocation;
	FRotator CameraRotation;
	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FRotator YawBaseRotation = FRotator(0, GetControlRotation().Yaw, 0);
	//FRotator YawBaseRotation = FRotator(0, CameraRotation.Yaw, 0);

	FVector CameraForward = UKismetMathLibrary::GetForwardVector(YawBaseRotation);

	AddMovementInput(CameraForward, AxisValue);
}

void ABasicPlayer::MoveRight(float AxisValue)
{
	FVector CameraLocation;
	FRotator CameraRotation;
	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	//FRotator YawBaseRotation = FRotator(0, CameraRotation.Yaw, 0);
	FRotator YawBaseRotation = FRotator(0, GetControlRotation().Yaw, 0);

	FVector CameraRight = UKismetMathLibrary::GetRightVector(YawBaseRotation);

	AddMovementInput(CameraRight, AxisValue);
}

void ABasicPlayer::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void ABasicPlayer::Turn(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void ABasicPlayer::Sprint()
{
	//Client
	bIsSprint = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	//Server
	C2S_SetSprint(true);

}

void ABasicPlayer::StopSprint()
{
	//Client
	bIsSprint = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	//Server
	C2S_SetSprint(false);
}

void ABasicPlayer::C2S_SetFire_Implementation(bool State)
{
	bIsFire = State;
}

void ABasicPlayer::C2S_SetSprint_Implementation(bool State)
{
	bIsSprint = State;
	GetCharacterMovement()->MaxWalkSpeed = State ? SprintSpeed : WalkSpeed;
}

void ABasicPlayer::C2S_SetIronsight_Implementation(bool State)
{
	bIsIronsight = State;
}

void ABasicPlayer::StartFire()
{
	bIsFire = true;
	C2S_SetFire(true);
	OnFire();
}

void ABasicPlayer::StopFire()
{
	bIsFire = false;
	C2S_SetFire(false);
}

void ABasicPlayer::OnFire()
{
	if (!bIsFire)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		int32 ScreenSizeX;
		int32 ScreenSizeY;
		FVector CrosshairWorldPosition; //3D
		FVector CrosshairWorldDirection; //3D 

		FVector CameraLocation;
		FRotator CameraRotation;

		//사람 반동
		int RandX = FMath::RandRange(-20, 20);
		int RandY = FMath::RandRange(3, 20);

		PC->GetViewportSize(ScreenSizeX, ScreenSizeY);
		PC->DeprojectScreenPositionToWorld(ScreenSizeX / 2 + RandX, ScreenSizeY / 2 + RandY, CrosshairWorldPosition, CrosshairWorldDirection);

		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		//총구 들리기(Tick에서 총 쏜 후에 애니메이션)
		FRotator PlayerRotation = GetControlRotation();
		PlayerRotation.Pitch += FMath::FRandRange(0.2f, 1.0f);
		GetController()->SetControlRotation(PlayerRotation);

		FVector TraceStart = CameraLocation;
		FVector TraceEnd = TraceStart + (CrosshairWorldDirection * 99999.f);

		C2S_ProcessFire(TraceStart, TraceEnd);
	}

	GetWorldTimerManager().SetTimer(FireTimerHandle,
		this,
		&ABasicPlayer::OnFire,
		0.12f,
		false);
}

void ABasicPlayer::StartIronsight()
{
	bIsIronsight = true;
	C2S_SetIronsight(true);
}

void ABasicPlayer::StopIronsight()
{
	bIsIronsight = false;
	C2S_SetIronsight(false);
}

void ABasicPlayer::StartCrouch()
{
	if (CanCrouch())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}


//host only
//데미지 처리되는 모든 액터에 구현을 해야 됨
float ABasicPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP <= 0)
	{
		return 0.0f;
	}

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) //PointDamage 처리
	{
		//다형성
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);

		UE_LOG(LogClass, Warning, TEXT("Point Damage %s"), *(PointDamageEvent->HitInfo.BoneName.ToString()));

		if (PointDamageEvent->HitInfo.BoneName.Compare(TEXT("head")) == 0)
		{
			CurrentHP = 0;
		}
		else
		{
			CurrentHP -= DamageAmount;
		}

		//Host
		OnRep_CurrentHP();


		//Hit Action all client
		S2A_HitAction(FMath::RandRange(1, 4));


		CurrentHP = FMath::Clamp(CurrentHP, 0.0f, 100.0f);

		if (CurrentHP <= 0)
		{
			//죽는거
			//애니메이션
			S2A_DeadAction(FMath::RandRange(1, 3));

			ABattleGM* GM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
			if (GM)
			{
				GM->CountAlivePlayer();
			}
		}

	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) //RadialDamage 처리
	{
		FRadialDamageEvent* RadialDamageEvent = (FRadialDamageEvent*)(&DamageEvent);

		UE_LOG(LogClass, Warning, TEXT("Radial Damage %f"), DamageAmount);
	}
	else //모든 데미지 처리
	{
		CurrentHP -= DamageAmount;

	}



	UE_LOG(LogClass, Warning, TEXT("%f"), DamageAmount);

	return 0.0f;
}

void ABasicPlayer::OnRep_CurrentHP()
{
	ABattlePC* PC = Cast<ABattlePC>(GetController());
	if (PC && PC->IsLocalController())
	{
		if (PC->BattleWidgetObject)
		{
			PC->BattleWidgetObject->UpdateHPBar(CurrentHP / MaxHP);
		}
	}
}

void ABasicPlayer::Reload()
{
	bIsReload = true;
	C2S_SetReload(true);
}

void ABasicPlayer::StartLeftLean()
{
	bLeftLean = true;
	C2S_SetLeftLean(true);
}

void ABasicPlayer::StopLeftLean()
{
	bLeftLean = false;
	C2S_SetLeftLean(false);
}

void ABasicPlayer::StartRightLean()
{
	bRightLean = true;
	C2S_SetRightLean(true);
}

void ABasicPlayer::StopRightLean()
{
	bRightLean = false;
	C2S_SetRightLean(false);
}

void ABasicPlayer::C2S_SetLeftLean_Implementation(bool State)
{
	bLeftLean = State;
}

void ABasicPlayer::C2S_SetRightLean_Implementation(bool State)
{
	bRightLean = State;
}

FRotator ABasicPlayer::GetAimOffset() const
{
	//WorldSpace Rotation -> LocalSpace Rotation, Rotation World -> Local
	//WorldSpace Rotation -> WorldSpace Direction Vector -> LocalSpace Direction Vector
	//LocalSpace Direction Vector -> LocalSpace Rotation

	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
	//return ActorToWorld().InverseTransformVectorNoScale(GetBaseAimRotation().Vector()).Rotation();
}

void ABasicPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasicPlayer, bIsFire);
	DOREPLIFETIME(ABasicPlayer, bIsSprint);
	DOREPLIFETIME(ABasicPlayer, bIsIronsight);
	DOREPLIFETIME(ABasicPlayer, CurrentHP);
	DOREPLIFETIME(ABasicPlayer, MaxHP);
	DOREPLIFETIME(ABasicPlayer, bLeftLean);
	DOREPLIFETIME(ABasicPlayer, bRightLean);
	DOREPLIFETIME(ABasicPlayer, bIsReload);
}

void ABasicPlayer::C2S_ProcessFire_Implementation(FVector TraceStart, FVector TraceEnd)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> Objects;

	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	TArray<AActor*> ActorToIgnore;

	FHitResult OutHit;

	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		TraceStart,
		TraceEnd,
		Objects,
		true,
		ActorToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	if (Result)
	{
		//all client spawn Hiteffect and Decal
		S2A_SpawnHitEffectAndDecal(OutHit);

		//Point Damage
		UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), //맞은놈
			1.0f,	//데미지
			-OutHit.ImpactNormal,	//데미지 방향
			OutHit,	//데미지 충돌 정보
			GetController(),	//때린 플레이어
			this,	//때린놈
			UBulletDamageType::StaticClass() //데미지 타입
		);

		MakeNoise(1.0f, this, OutHit.ImpactPoint);
	}

	//All Client Spawn Muzzleflash and Sound
	S2A_SpawnMuzzleFlashAndSound();
}

void ABasicPlayer::S2A_SpawnMuzzleFlashAndSound_Implementation()
{
	//WeaponSound and MuzzleFlash
	if (WeaponSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(),
			WeaponSound,
			Weapon->GetComponentLocation()
		);
	}

	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			MuzzleFlash,
			Weapon->GetSocketTransform(TEXT("Muzzle"))
		);
	}
}

void ABasicPlayer::S2A_SpawnHitEffectAndDecal_Implementation(FHitResult OutHit)
{
	//HitEffect(Blood) and Decal
	if (Cast<ACharacter>(OutHit.GetActor()))
	{
		//캐릭터
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			BloodHitEffect,
			OutHit.ImpactPoint + (OutHit.ImpactNormal * 10)
		);
	}
	else
	{
		//지형
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			HitEffect,
			OutHit.ImpactPoint + (OutHit.ImpactNormal * 10)
		);

		UDecalComponent* NewDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
			NormalDecal,
			FVector(5, 5, 5),
			OutHit.ImpactPoint,
			OutHit.ImpactNormal.Rotation(),
			10.0f
		);

		NewDecal->SetFadeScreenSize(0.005f);

	}
}

void ABasicPlayer::S2A_HitAction_Implementation(int Number)
{
	if (HitActionMontage)
	{
		FString SectionName = FString::Printf(TEXT("Hit%d"), Number);
		PlayAnimMontage(HitActionMontage, 1.0f, FName(SectionName));
	}
}

void ABasicPlayer::S2A_DeadAction_Implementation(int Number)
{
	if (DeadMontage)
	{
		FString SectionName = FString::Printf(TEXT("Death_%d"), Number);
		PlayAnimMontage(DeadMontage, 1.0f, FName(SectionName));


		//물리 처리
		//GetMesh()->SetSimulatePhysics(true);
		//GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GetMesh()->AddImpulse(-PointDamageEvent->HitInfo.ImpactNormal * 30000.0f, PointDamageEvent->HitInfo.BoneName);
	}

	DisableInput(Cast<APlayerController>(GetController()));
}

void ABasicPlayer::C2S_SetReload_Implementation(bool newState)
{
	bIsReload = newState;

}

void ABasicPlayer::AddPickItem(AMasterItem* AddItem)
{
	PickItemList.Add(AddItem);

	ABattlePC* PC = Cast<ABattlePC>(GetController());
	if (PC)
	{
		PC->ShowItemTooltip(AddItem->ItemData.ItemName);
	}
}

void ABasicPlayer::RemovePickItem(AMasterItem* RemoveItem)
{
	PickItemList.Remove(RemoveItem);

	ABattlePC* PC = Cast<ABattlePC>(GetController());
	if (PC)
	{
		if (PickItemList.Num() > 0)
		{
			PC->ShowItemTooltip(PickItemList[PickItemList.Num() - 1]->ItemData.ItemName);
		}
		else
		{
			PC->HideItemTooltip();
		}
	}

}

void ABasicPlayer::Pickup()
{
	if (PickItemList.Num() > 0)
	{
		//Server Pickup check
		C2S_CheckPickupItem(PickItemList[PickItemList.Num() - 1]);
	}
}

void ABasicPlayer::C2S_CheckPickupItem_Implementation(AMasterItem* PickupItem)
{
	if (IsValid(PickupItem))
	{
		S2C_InsertItem(PickupItem);
		PickupItem->Destroy();
	}
}

void ABasicPlayer::S2C_InsertItem_Implementation(AMasterItem* PickupItem)
{
	AddInventory(PickupItem);
}

void ABasicPlayer::AddInventory(AMasterItem* Item)
{
	Inventory.Add(Item);
}

void ABasicPlayer::RemoveInventory(AMasterItem* Item)
{
	Inventory.Remove(Item);
}

