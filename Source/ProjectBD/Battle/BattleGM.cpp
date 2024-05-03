// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGM.h"
#include "BattleGS.h"
#include "BattlePC.h"
#include "../Basic/BasicPlayer.h"
#include "BattlePS.h"
#include "Kismet/GameplayStatics.h"
#include "../Item/ItemPoint.h"
#include "../Item/MasterItem.h"

void ABattleGM::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	UE_LOG(LogClass, Warning, TEXT("PostSeamlessTravel"));
}

void ABattleGM::CountAlivePlayer()
{
	ABattleGS* GS = GetGameState<ABattleGS>();
	if (GS)
	{
		//STL Iterator, 
		//for(FConstPlayerControllerIterator Iter = GetWorld()->GetPlayerControllerIterator() )
		GS->AlivePlayerCount = 0;
		for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
		{
			ABattlePC* PC = Cast<ABattlePC>(*Iter);
			if (PC)
			{
				ABasicPlayer* Pawn = Cast<ABasicPlayer>(PC->GetPawn());
				if (Pawn)
				{
					if (Pawn->CurrentHP > 0)
					{
						GS->AlivePlayerCount++;
					}
				}
			}
		}

		//죽은 플레이어 랭킹 지정
		for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
		{
			ABattlePC* PC = Cast<ABattlePC>(*Iter);
			if (PC)
			{
				ABasicPlayer* Pawn = Cast<ABasicPlayer>(PC->GetPawn());
				if (Pawn)
				{
					if (Pawn->CurrentHP <= 0)
					{
						ABattlePS* PS = Pawn->GetPlayerState<ABattlePS>();
						if (PS)
						{
							if (PS->PlayerRank == -1)
							{
								PS->PlayerRank = GS->AlivePlayerCount + 1;
							}
						}
					}
				}
			}
		}

		if (GS->AlivePlayerCount == 1 && GetNumPlayers() > 1)
		{
			//GameOver;
			UE_LOG(LogClass, Warning, TEXT("GameOver"));

			for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
			{
				ABattlePC* PC = Cast<ABattlePC>(*Iter);
				if (PC)
				{
					ABasicPlayer* Pawn = Cast<ABasicPlayer>(PC->GetPawn());
					if (Pawn)
					{
						if (Pawn->CurrentHP > 0)
						{
							ABattlePS* PS = Pawn->GetPlayerState<ABattlePS>();
							PS->PlayerRank = 1;
						}
					}
				}
			}

			//GameOver
			//Network Result UI Show
			//Lobby Timer
			FTimerHandle EndingTimer;
			GetWorldTimerManager().SetTimer(
				EndingTimer,
				this,
				&ABattleGM::GoLobby,
				3.0f
			);
		}



		//Host only, UI Update
		GS->OnRep_AlivePlayerCount();
	}
}

void ABattleGM::GoLobby()
{
	GetWorld()->ServerTravel(TEXT("Lobby"));
}


void ABattleGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABattleGS* GS = GetGameState<ABattleGS>();
	if (GS)
	{
		//관중을 제외한 PlayerCount
		GS->TotalPlayerCount = GetNumPlayers();
	}

	
	CountAlivePlayer();
}

void ABattleGM::Logout(AController* Exiting)
{
	CountAlivePlayer();

	Super::Logout(Exiting);
}

void ABattleGM::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> ItemPoints;

	//get item spawn point
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemPoint::StaticClass(), ItemPoints);

	for (int i = 0; i < ItemPoints.Num(); ++i)
	{
		GetWorld()->SpawnActor<AMasterItem>(SpawnItemClass, ItemPoints[i]->GetActorTransform());
	}
}
