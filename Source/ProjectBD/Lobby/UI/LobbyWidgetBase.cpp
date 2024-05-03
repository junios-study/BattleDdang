// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidgetBase.h"
#include "Components/EditableTextBox.h"
#include "../LobbyPC.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "../../BDGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../LobbyGS.h"
#include "../LobbyGM.h"
#include "Components/Button.h"

void ULobbyWidgetBase::PressStartGameButton()
{
	ALobbyGM* GM = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM) //Server Only
	{
		GM->StartGame();
	}
}

void ULobbyWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	ChatInput = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("ChatInput")));

	if (ChatInput)
	{
		ChatInput->OnTextCommitted.AddDynamic(this, &ULobbyWidgetBase::ProcessTextCommited);
	}

	ChatBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChatBox")));

	ConnectCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("ConnectCount")));
	PublicMessage = Cast<UTextBlock>(GetWidgetFromName(TEXT("PublicMessage")));

	StartGameButton = Cast<UButton>(GetWidgetFromName(TEXT("StartGameButton")));
	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &ULobbyWidgetBase::PressStartGameButton);
	}
}

void ULobbyWidgetBase::ProcessTextCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	switch (CommitMethod)
	{
		case ETextCommit::OnEnter:
		{
			//Local PC -> Host PC
			ALobbyPC* PC = GetOwningPlayer<ALobbyPC>();
			if (PC)
			{
				UBDGameInstance* GI = Cast<UBDGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
				if (GI)
				{
					FString Temp = FString::Printf(TEXT("%s : %s"), *GI->GetUserID(), *Text.ToString());

					PC->C2S_SendMessage(FText::FromString(Temp));
					ChatInput->SetText(FText::FromString(TEXT("")));
				}
			}
		}
		break;
		case ETextCommit::OnCleared:
		{
			ChatInput->SetUserFocus(GetOwningPlayer());
		}
		break;
	}
}

void ULobbyWidgetBase::AddMessage(FText Message)
{
	if (ChatBox)
	{
		UTextBlock* NewTextBlock = NewObject<UTextBlock>(ChatBox);
		if (NewTextBlock)
		{ 
			NewTextBlock->SetText(Message);

			FSlateFontInfo FontInfo = NewTextBlock->GetFont();
			FontInfo.Size = 18;
			NewTextBlock->SetFont(FontInfo);
			//NewTextBlock->Font.Size = 18;
			ChatBox->AddChild(NewTextBlock);
			ChatBox->ScrollToEnd();
		}
	}
}

//void ULobbyWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
//{
//	Super::NativeTick(MyGeometry, InDeltaTime);
//}

void ULobbyWidgetBase::SetConnectCount(int NewConnectCount)
{
	FString Temp = FString::Printf(TEXT("%d명 접속"), NewConnectCount);
	if (ConnectCount)
	{
		ConnectCount->SetText(FText::FromString(Temp));
	}
}

void ULobbyWidgetBase::SetPublicMessage(int LeftTime)
{
	FString Temp = FString::Printf(TEXT("%d초 남았습니다."), LeftTime);
	if (PublicMessage)
	{
		PublicMessage->SetText(FText::FromString(Temp));
	}
}

void ULobbyWidgetBase::HideStartGameButton()
{
	if (StartGameButton)
	{
		StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}


