// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine.h"
#include "starbreezeGameMode.h"
#include "starbreezeCharacter.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SK_Mesh = GetMesh();
	SK_Mesh->bGenerateOverlapEvents = true;
	SK_CapsuleComp = GetCapsuleComponent();
	//SK_CapsuleComp->BodyInstance.SetResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	SK_CapsuleComp->SetCapsuleRadius(22.f,true);
	SK_Mesh->SetNotifyRigidBodyCollision(true);
	
	// Setting Up Collisions
	
	//SK_CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::HitPlayer);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	SK_Mesh->SetRelativeLocation(FVector(0,0,-86.f));
	SK_Mesh->SetRelativeRotation(FRotator(0, -90.f, 0));
	//SK_Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//SK_Mesh->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnHitPlayer);
	SK_CapsuleComp->OnComponentHit.AddDynamic(this, &AEnemy::OnHitPlayer);

	// Setting Default Values

	Health = 100.f;
	isDead = false;
	bCanHitPlayer = true;

	//EnemyAIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("EnemyAIPerception"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemy::ApplDamageToEnemy(float value)
{
	if (Health > 0)
		Health -= value;
	else if (isDead == false)
		KillEnemy();

}

void AEnemy::KillEnemy()
{
	isDead = true;
	//SK_CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//SK_CapsuleComp->SetEnableGravity(false);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Killed")));
	SK_CapsuleComp->SetCollisionProfileName(FName(TEXT("NoCollision")));
	SK_Mesh->SetCollisionProfileName(FName(TEXT("PhysicsActor")));
	SK_Mesh->SetSimulatePhysics(true);
	
	class AstarbreezeCharacter *Player = Cast<AstarbreezeCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Player != nullptr)
	{
		Player->UpdateScore();
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UpdateScore")));    
	}

	//OnDestroyed.AddDynamic(this, &AEnemy::OnDestroy);
	
	//delay
	GetWorld()->GetTimerManager().SetTimer(_DelayHandle, this, &AEnemy::OnDelayEnd, 2.f, false);
			
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

void AEnemy::OnHitPlayer(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player Hit")));
	AstarbreezeCharacter *Player = Cast<AstarbreezeCharacter>(OtherActor);   // Check if its player
	if(Player) 
	{
		
		if (bCanHitPlayer)
		{
			Player->UpdateHealth(-5.f);				/// Reduce Player Health
			bCanHitPlayer = false;
		}
		GetWorld()->GetTimerManager().SetTimer(_DelayHandle, this, &AEnemy::HitCheck, 1.6f, false);
		
	}
}

void AEnemy::OnDelayEnd()
{
	Destroy();			// Destroy actor after delay ends

}

void AEnemy::HitCheck()
{
	bCanHitPlayer = true;
}
