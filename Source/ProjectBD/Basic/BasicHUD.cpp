// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicHUD.h"
#include "Engine/Canvas.h"
#include "Engine/StreamableManager.h"
#include "Engine/Texture2D.h"

void ABasicHUD::DrawHUD()
{
	Super::DrawHUD();

	if (Crosshair)
	{
		DrawTextureSimple(Crosshair,
			(Canvas->SizeX / 2) - (Crosshair->GetSizeX() / 2),
			(Canvas->SizeY / 2) - (Crosshair->GetSizeY() / 2)
		);
	}
}

void ABasicHUD::BeginPlay()
{
	Super::BeginPlay();

	FStreamableManager loader;

	T_Crosshair.SetPath(TEXT("Texture2D'/Game/UI/Crossshair/crosshair.crosshair'"));

	//���� �ε�, �ε��� ȭ���� ����, 10�� �޽� �ε� ���������� ����
	//Crosshair = loader.LoadSynchronous<UTexture2D>(T_Crosshair);
	
	//�񵿱� �ε�
	loader.RequestAsyncLoad(T_Crosshair, FStreamableDelegate::CreateUObject(this, &ABasicHUD::LoadHUDTexture));

}

void ABasicHUD::LoadHUDTexture()
{
	Crosshair = Cast<UTexture2D>(T_Crosshair.TryLoad());
}