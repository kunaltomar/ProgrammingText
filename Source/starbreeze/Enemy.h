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


	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* SK_Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = CapsuleComp)
		UCapsuleComponent* SK_CapsuleComp;
	// Sets default values for this character's properties

	//  AI  //
	UPROPERTY(EditAnywhere, Category = AISystem)
		class UBehaviorTree *EnemyBehaviour;

	AEnemy();

	// GamePlay Variables
	UPROPERTY(BlueprintReadOnly, Category = EnemyConfig)
		float Health;

	// GamePlay Events
	UFUNCTION(BlueprintImplementableEvent, Category = "GamePlay")
		void BPEventGotKilled();
private:

	// Damaging Player
	bool bCanHitPlayer;
	void HitCheck();

	// DElay
	FTimerHandle _DelayHandle;

	UFUNCTION()
	void OnDelayEnd();

	// Check Collision with Player
	UFUNCTION()
	void OnHitPlayer(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void KillEnemy();
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
		
public:	
	// Called every frame
	void ApplDamageToEnemy(float);
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
