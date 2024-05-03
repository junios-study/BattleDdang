// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "ItemPoint.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API AItemPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
#if WITH_EDITORONLY_DATA
	//�����Ϳ����� ����
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UTexture2D* NewIcon;
#endif

	AItemPoint();
};
