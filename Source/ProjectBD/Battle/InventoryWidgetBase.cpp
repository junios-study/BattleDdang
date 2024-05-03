// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidgetBase.h"
#include "Components/ScrollBox.h"
#include "ItemSlotBase.h"

void UInventoryWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	ItemSlots = Cast<UScrollBox>(GetWidgetFromName(TEXT("ItemSlots")));
}

void UInventoryWidgetBase::UpdateInventory(TArray<class AMasterItem*> Inventory)
{
	for (int i = 0; i < ItemSlots->GetChildrenCount(); ++i)
	{
		UItemSlotBase* InventorySlot = Cast<UItemSlotBase>(ItemSlots->GetChildAt(i)); 
		if (InventorySlot)
		{
			InventorySlot->bUse = false;
			InventorySlot->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	for (int i = 0; i < Inventory.Num(); ++i)
	{
		int FindIndex = GetEmptySlot();
		if (FindIndex > -1) 
		{
			SetSlot(FindIndex, Inventory[i]);
		}
	}

}

int UInventoryWidgetBase::GetEmptySlot()
{
	for (int i = 0; i < ItemSlots->GetChildrenCount(); ++i)
	{
		UItemSlotBase* InventorySlot = Cast<UItemSlotBase>(ItemSlots->GetChildAt(i));
		if (InventorySlot && InventorySlot->bUse == false)
		{
			return i;
		}
	}

	return -1; //full
}

void UInventoryWidgetBase::SetSlot(int Index, AMasterItem* Item)
{
	UItemSlotBase* EmptySlot = Cast<UItemSlotBase>(ItemSlots->GetChildAt(Index));
	if (EmptySlot && EmptySlot->bUse == false)
	{
		EmptySlot->UpdateItemSlot(Item);
		EmptySlot->bUse = true;
		EmptySlot->SetVisibility(ESlateVisibility::Visible);
	}
}
