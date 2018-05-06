// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine.h"
#include "starbreezeCharacter.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SK_Mesh = GetMesh();
	SK_CapsuleComp = GetCapsuleComponent();
	SK_CapsuleComp->BodyInstance.SetResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	SK_Mesh->SetRelativeLocation(FVector(0,0,-86.f));
	SK_Mesh->SetRelativeRotation(FRotator(0, -90.f, 0));

	Health = 100.f;
	isDead = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::KillEnemy()
{
	isDead = true;
	SK_CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SK_CapsuleComp->SetEnableGravity(false);
	SK_Mesh->SetSimulatePhysics(true);
	
	class AstarbreezeCharacter *Player = Cast<AstarbreezeCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Player != nullptr)
	{
		Player->UpdateScore();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UpdateScore")));
	}
	
}

void AEnemy::ApplDamageToEnemy(float value)
{
	if(Health>=0)
	Health -= value;
	else if(isDead == false)
	KillEnemy();
		
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

