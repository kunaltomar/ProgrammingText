// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "starbreezeCharacter.h"
#include "starbreezeProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include "Enemy.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AstarbreezeCharacter

AstarbreezeCharacter::AstarbreezeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	
	//MyCharacterMovement = GetCharacterMovement();
// 	MyCharacterMovement->CrouchedHalfHeight = CrouchingHeight;

	// 	Setting Up Default Velues for Variables
	Health = 100;
	KillCount = 0;

	WeaponRange = 1000;
	ShellCount = 8;
	WeaponSpread = 0.5;
	ImpulseStrength = 50;
	DamageAmount = 0.f;
	isDead = false;
}


void AstarbreezeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	//MyCharacterMovement->MaxWalkSpeedCrouched = 50.f;
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void AstarbreezeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AstarbreezeCharacter::OnFire);
	
	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AstarbreezeCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AstarbreezeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AstarbreezeCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AstarbreezeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AstarbreezeCharacter::LookUpAtRate);
}



void AstarbreezeCharacter::OnFire()
{
	
	//DrawDebugLine(GetWorld(), FP_MuzzleLocation->GetComponentLocation(), (FP_MuzzleLocation->GetForwardVector() * 10000.f) + FP_MuzzleLocation->GetComponentLocation(), FColor(255, 0, 0), false, -1, 0, 5);

	for (int32 i = 0; i <= ShellCount; i++)
	{
		Instant_Fire();
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(DamageAmount));
	if (DamageAmount > 0) {
		DisplayDamage(DamageAmount);
	}
	DamageAmount = 0.f;
	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}




void AstarbreezeCharacter::Instant_Fire()
{
	
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = ShellCount;
	const float SpreadCone = FMath::DegreesToRadians(WeaponSpread *ShellCount);  // to take only half of the sphere we multiply it with 0.5
	const FVector AimDir = FP_MuzzleLocation->GetComponentRotation().Vector();
	const FVector StartTrace = FP_MuzzleLocation->GetComponentLocation();
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, SpreadCone);
	const FVector EndTrace = StartTrace + ShootDir * WeaponRange;
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
}


FHitResult AstarbreezeCharacter::WeaponTrace(const FVector & TraceFrom, const FVector & TraceTo) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);
	FHitResult Hit(ForceInit);  // To Reintialize every time 
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, TRACE_WEAPON, TraceParams);
	return Hit;

}

void AstarbreezeCharacter::ProcessInstantHit(const FHitResult & Impact, const FVector & Origin, const FVector & ShootDir, int32 RandomSeed, float ReticleSpread)
{
	
	const FVector EndTrace = Origin + ShootDir * WeaponRange;
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

	// Showing debug for Bullet Spread and Impact location

	DrawDebugLine(this->GetWorld(), Origin, Impact.TraceEnd, FColor::Black, true, 0.25, 0.25);			
	DrawDebugPoint(this->GetWorld(), Impact.ImpactPoint,5, FColor::Red, false, 0.25);
	
	FVector Impulse;
	if (Impact.bBlockingHit)
	{	
		
		class AEnemy *HitEnemy = Cast<AEnemy>(Impact.Actor);
		if (Impact.Component->IsSimulatingPhysics())
		{
			FVector dir = FVector(FP_MuzzleLocation->GetForwardVector());
			Impact.Component->AddImpulse(ImpulseStrength* dir * Impact.Component->GetMass());

		}
		if (HitEnemy != nullptr)
		{
			if(HitEnemy->isDead == false )
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("DamageThreshold: %f"),Impact.PhysMaterial->DestructibleDamageThresholdScale));
			DamageAmount = DamageAmount + Impact.PhysMaterial->DestructibleDamageThresholdScale;
			HitEnemy->ApplDamageToEnemy(DamageAmount);
			if (Impact.Component->IsSimulatingPhysics())
			{
				FVector dir = FVector(FP_MuzzleLocation->GetForwardVector());
				Impact.Component->AddImpulse(ImpulseStrength* dir * Impact.Component->GetMass()*0.5);
			}

		}
		
	}
}

void AstarbreezeCharacter::UpdateScore()
{
	KillCount += 1;
	DisplayScore(WaveCount);
}

void AstarbreezeCharacter::UpdateHealth(float value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Health Updated")));
	Health += value;
	UpdateHealthEvent(Health);
	if (Health <= 0)
	{
		KillPlayer();
	}

}

void AstarbreezeCharacter::KillPlayer()
{
	if(isDead ==false)
	{
		KillPlayerEvent();
	}
}




void AstarbreezeCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AstarbreezeCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AstarbreezeCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AstarbreezeCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AstarbreezeCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AstarbreezeCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AstarbreezeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AstarbreezeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AstarbreezeCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AstarbreezeCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AstarbreezeCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AstarbreezeCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}


