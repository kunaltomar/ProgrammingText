// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_CheckForPlayer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyAIController.h"
#include "starbreezeCharacter.h"		
#include "Engine.h"
#include "Enemy.h"


UBTService_CheckForPlayer::UBTService_CheckForPlayer()
{
	bCreateNodeInstance = true;
}

void UBTService_CheckForPlayer::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	AEnemyAIController *EnemyController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (EnemyController)
	{
		AstarbreezeCharacter *Player = Cast <AstarbreezeCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());			// Search For Player reference
		if (Player)
		{
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(EnemyController->EnemyKeyID, Player);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayerExist"));
		}

	}

}
