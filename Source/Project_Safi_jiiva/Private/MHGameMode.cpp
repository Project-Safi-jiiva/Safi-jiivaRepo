// Fill out your copyright notice in the Description page of Project Settings.


#include "MHGameMode.h"
#include "Hunter/Hunter.h"

void AMHGameMode::setQuestLife()
{
	QuestLife--;
}

int32 AMHGameMode::GetQuestLife()
{
	return QuestLife;
}
