// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPC.h"
#include "UI/LobbyWidgetBase.h"

void ALobbyPC::BeginPlay()
{
	Super::BeginPlay();

	//1. #include path
	FSoftClassPath LobbyWidgetClass(TEXT("WidgetBlueprint'/Game/Blueprints/Lobby/UI/LobbyWidget.LobbyWidget_C'"));

	//2. #include<path>
	UClass* WidgetClass = LobbyWidgetClass.TryLoadClass<ULobbyWidgetBase>();

	if (WidgetClass)
	{
		UE_LOG(LogClass, Warning, TEXT("BeginPlay"));
		if (IsLocalPlayerController())
		{
			UE_LOG(LogClass, Warning, TEXT("BeginPlay Local"));

			//3. CreateWidget
			LobbyWidgetObject = CreateWidget<ULobbyWidgetBase>(this, WidgetClass);
			if (LobbyWidgetObject)
			{
				LobbyWidgetObject->AddToViewport();
				if (!HasAuthority())
				{
					LobbyWidgetObject->HideStartGameButton();
				}

			}

			SetInputMode(FInputModeGameAndUI());
			bShowMouseCursor = true;

		}
	}

}

//Server에서 실행
bool ALobbyPC::C2S_SendMessage_Validate(const FText& Message)
{

	return true;
}

//Server에서 실행
void ALobbyPC::C2S_SendMessage_Implementation(const FText& Message)
{
	//c++ 11
	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; Iter++)
	{
		ALobbyPC* PC = Cast<ALobbyPC>(*Iter);
		if (PC)
		{
			//Server to Client
			PC->S2C_SendMessage(Message);
		}
	}
}

//Call Server, Execute Client
void ALobbyPC::S2C_SendMessage_Implementation(const FText& Message)
{
	if (LobbyWidgetObject)
	{
		LobbyWidgetObject->AddMessage(Message);
	}
}
