// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Enemy.h"


AEnemyAIController::AEnemyAIController()
{
	BlackBoardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
}

void AEnemyAIController::Possess(APawn *InPawn)
{
	Super::Possess(InPawn);

	AEnemy *EnemyChar = Cast <AEnemy>(InPawn);

	if (EnemyChar && EnemyChar->EnemyBehaviour)
	{
		BlackBoardComp->InitializeBlackboard(*EnemyChar->EnemyBehaviour->BlackboardAsset);
		EnemyKeyID = BlackBoardComp->GetKeyID("Target");

		BehaviorTreeComp->StartTree(*EnemyChar->EnemyBehaviour);

	}
}
