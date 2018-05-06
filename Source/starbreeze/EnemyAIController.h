// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class STARBREEZE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

		UPROPERTY(transient)
		class UBlackboardComponent *BlackBoardComp;

	UPROPERTY(transient)
		class UBehaviorTreeComponent *BehaviorTreeComp;

public:
	AEnemyAIController();

	virtual void Possess(APawn *InPawn) override;

	uint8 EnemyKeyID;
	
	
	
};
