// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class STARBREEZE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

		
public:
	
	bool isDead;


	UPROPERTY(EditAnywhere, Category = Mesh)
		USkeletalMeshComponent* SK_Mesh;

	UPROPERTY(EditAnywhere, Category = CapsuleComp)
		UCapsuleComponent* SK_CapsuleComp;
	// Sets default values for this character's properties

	UPROPERTY(EditAnywhere, Category = AISystem)
		class UBehaviorTree *EnemyBehaviour;

	AEnemy();

protected:
	UPROPERTY(BlueprintReadOnly, Category = EnemyConfig)
		float Health;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void KillEnemy();

	UFUNCTION()
	void HitPlayer(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult &SweepResult);

public:	
	// Called every frame
	void ApplDamageToEnemy(float);
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
