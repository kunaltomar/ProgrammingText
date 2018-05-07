// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MoveToPlayer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyAIController.h"
#include "starbreezeCharacter.h"		
#include "Engine.h"
#include "Enemy.h"



EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIController* EnemyController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	AstarbreezeCharacter *Player = Cast<AstarbreezeCharacter>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(EnemyController->EnemyKeyID));
	
	if (Player)
	{
		EnemyController->MoveToActor(Player, 2.f, true, true, true, 0, true);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed"));
	}
}
