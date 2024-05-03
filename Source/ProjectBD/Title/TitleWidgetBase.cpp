// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "TitlePC.h"
#include "../BDGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UTitleWidgetBase::NativeConstruct()
{
	UserID = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("UserID")));
	Password = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Password")));
	ServerIP = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("ServerIP")));
	ConnectButton = Cast<UButton>(GetWidgetFromName(TEXT("ConnectButton")));
	StartButton = Cast<UButton>(GetWidgetFromName(TEXT("StartButton")));

	if (ConnectButton)
	{
		ConnectButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnConnectButton);
	}

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnStartButton);
	}
}

void UTitleWidgetBase::OnConnectButton()
{
	//������ ����
	ATitlePC* PC = GetOwningPlayer<ATitlePC>();
	if (PC)
	{
		if (ServerIP)
		{
			SaveUserID();

			FString ServerIPAddress = ServerIP->GetText().ToString();

			PC->ConnectServer(ServerIPAddress);
		}
	}
}

void UTitleWidgetBase::OnStartButton()
{
	//������ ����
	ATitlePC* PC = GetOwningPlayer<ATitlePC>();
	if (PC)
	{
		SaveUserID();

		PC->StartServer();
	}
}

void UTitleWidgetBase::SaveUserID()
{
	UBDGameInstance* GI = Cast<UBDGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		if (UserID)
		{
			GI->SetUserID(UserID->GetText().ToString());
		}
	}
}

