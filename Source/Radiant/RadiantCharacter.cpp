// Copyright Epic Games, Inc. All Rights Reserved.

#include "RadiantCharacter.h"
#include "RadiantProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"


//////////////////////////////////////////////////////////////////////////
// ARadiantCharacter

ARadiantCharacter::ARadiantCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Player Abilities
	bFirstAbilityApplied = false;
	bSecondAbilityApplied = false;
	bThirdAbilityApplied = false;
	bInteractApplied = false;

	bIsFloating = false;
	bIsFiring = false;
	bApplyRecoil = false;
	bRecoilReset = false;
	FiringError = 1;
	MovementError = 1;
	Kills = 0;
	
	UltOrbs = 0;
	bUltUi = false;

	// Enum Default Values
	EDirection = EMovementDirection::EStationary;
	EOrientation = EMovementDirection::EStationary;
	EWeapon = EWeaponEquipped::EKnife;
}

void ARadiantCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

//////////////////////////////////////////////////////////////////////////// Input

void ARadiantCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	//Bind Unique Abilities
	PlayerInputComponent->BindAction("FirstAbility", IE_Pressed, this, &ARadiantCharacter::FirstAbility);
	PlayerInputComponent->BindAction("FirstAbility", IE_Released, this, &ARadiantCharacter::StopFirstAbility);

	PlayerInputComponent->BindAction("SecondAbility", IE_Pressed, this, &ARadiantCharacter::SecondAbility);
	PlayerInputComponent->BindAction("SecondAbility", IE_Released, this, &ARadiantCharacter::StopSecondAbility);

	PlayerInputComponent->BindAction("ThirdAbility", IE_Pressed, this, &ARadiantCharacter::ThirdAbility);
	PlayerInputComponent->BindAction("ThirdAbility", IE_Released, this, &ARadiantCharacter::StopThirdAbility);

	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &ARadiantCharacter::Ultimate);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ARadiantCharacter::InteractPressed);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARadiantCharacter::Jumping);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARadiantCharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ARadiantCharacter::OnPrimaryAction);	
	PlayerInputComponent->BindAction("PrimaryAction", IE_Released, this, &ARadiantCharacter::StopPrimaryAction);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ARadiantCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ARadiantCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ARadiantCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ARadiantCharacter::LookUpAtRate);
}

void ARadiantCharacter::OnPrimaryAction()
{
	bIsFiring = true;

	if (EWeapon == EWeaponEquipped::EPrimary)
	{
		if (bCanFire)
		{
			/*Bullet RayCast Parameters*/
			FHitResult* HitResult = new FHitResult();
			FVector ForwardVector = GetFirstPersonCameraComponent()->GetForwardVector();
			FVector StartTrace = GetFirstPersonCameraComponent()->GetComponentLocation() + (ForwardVector * 200.0f);
			FVector RightVector = GetFirstPersonCameraComponent()->GetRightVector();
			FVector UpVector = GetFirstPersonCameraComponent()->GetUpVector();
			FVector EndTrace = StartTrace + (ForwardVector * 10000.0f);

			// incorporating movement error into the calculations for the raycast
			if (MovementError > 1)
			{
				EndTrace += (UpVector * MovementError * FMath::RandRange(-200, 200) + (UpVector * MovementError * FMath::RandRange(-50, 50)));
			}
			if (FiringError > 1)
			{
				EndTrace += (RightVector * MovementError * FiringError * FMath::RandRange(-50, 50)) + 
					(UpVector * MovementError * FMath::RandRange(-100, 100));
			}

			FCollisionQueryParams* CQP = new FCollisionQueryParams();
			CQP->bReturnPhysicalMaterial = true;

			// If the hit actor is not an enemy, just simply spawn a bullet impact decal,
			// else we figure out which bodypart is hit and deal damage accordingly
			// If an enemy is killed we calculate the ult orbs and current kills
			if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Pawn, *CQP))
			{
				BodyPartHit = *HitResult;
				ActorHit = BodyPartHit.GetActor();
				if (!HitResult->GetActor()->ActorHasTag("Enemy"))
				{
					bSpawnDecal = true;
					DecalLocation = HitResult->Location;
					DecalNormal = HitResult->ImpactNormal;
				}
				else
				{
					EnemyHit = Cast<AEnemy>(ActorHit);
					if (EnemyHit != nullptr)
					{
						OnEnemyHit.Broadcast();
						EnemyHit->ApplyDamage(DamageDealt);
						if (EnemyHit->bShouldDie)
						{
							Kills += 1;
							EnemyHit->Death();
							if (UltOrbs < MaxUltOrbs)
							{
								UltOrbs++;
								if (UltOrbs == MaxUltOrbs)
								{
									bUltUi = true;
									bCanUlt = true;
								}
							}
						}
					}
					bSpawnDecal = false;
				}
			}

			// broadcast that the weapon has been fired to cue audio, recoil, and fire rate timers.
			OnFireWeapon.Broadcast();
			RecoilAmount = 1.00f;
			bApplyRecoil = true;
			FireCount += 1;
			bCanFire = false;
			FireRate = 1.00f;
			RecoilDuration = 1.00f;
		}
	}
	
	// If the player is using an ultimate weapon, then bullet calculations will be done in that class
	if (EWeapon == EWeaponEquipped::EAbility) 
	{
		bIsFiring = false;
		bUltFired = true;
	}
}

void ARadiantCharacter::StopPrimaryAction()
{
	bIsFiring = false;
}

void ARadiantCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ARadiantCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void ARadiantCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);

		// Vertical Direction
		if (Value > 0.0f)
		{
			EOrientation = EMovementDirection::EForward;
		}
		if (Value < 0.0f)
		{
			EOrientation = EMovementDirection::EBackward;
		}
	}
	else {
		EOrientation = EMovementDirection::EStationary;
	}

}

void ARadiantCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);

		// Horizontal Direction
		if (Value > 0.0f)
		{
			EDirection = EMovementDirection::ERight;
		}
		if (Value < 0.0f)
		{
			EDirection = EMovementDirection::ELeft;
		}
	}
	else {
		EDirection = EMovementDirection::EStationary;
	}
}

void ARadiantCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ARadiantCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool ARadiantCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ARadiantCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ARadiantCharacter::EndTouch);

		return true;
	}
	
	return false;
}

void ARadiantCharacter::FirstAbility()
{
	bFirstAbilityApplied = true;
	bControllingFirstAbility = true;
}

void ARadiantCharacter::StopFirstAbility()
{
	bControllingFirstAbility = false;
}

void ARadiantCharacter::SecondAbility()
{
	bSecondAbilityApplied = true;
	bControllingSecondAbility = true;
}

void ARadiantCharacter::StopSecondAbility()
{
	bControllingSecondAbility = false;
}

void ARadiantCharacter::ThirdAbility()
{
	bThirdAbilityApplied = true;
	bControllingThirdAbility = true;
}

void ARadiantCharacter::StopThirdAbility()
{
	bControllingThirdAbility = false;
}

void ARadiantCharacter::Jumping()
{
	bIsFloating = true;
	ACharacter::Jump();
}

void ARadiantCharacter::StopJumping()
{
	bIsFloating = false;
	ACharacter::StopJumping();
}

void ARadiantCharacter::InteractPressed()
{
	bInteractApplied = true;
}

void ARadiantCharacter::Interact(FHitResult* OtherActor)
{
	// Current code only supports Vandal pickups
	if (OtherActor->GetActor()->ActorHasTag("Vandal")) 
	{
		VandalInstance = Cast<AVandal>(OtherActor->GetActor());
		VandalInstance->AttachToComponent(GetMesh1P(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			FName(TEXT("GripPoint")));
	}
	bInteractApplied = false;
}

void ARadiantCharacter::Ultimate() 
{
	if (bCanUlt)
	{
		bUltActivated = true;
	}
}

void ARadiantCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// If the player is spamming, make sure to continously call the firing function
	if (bIsFiring)
	{
		ARadiantCharacter::OnPrimaryAction();
	}

	// Determining the bullet accuracy based on the player's movement,, (1 means full accuracy)
	if (EDirection == EMovementDirection::EStationary && EOrientation == EMovementDirection::EStationary)
	{
		MovementError = 1;
	}
	else
	{
		MovementError = 5;
	}
	if (!bIsFiring)
	{
		FireCount = 0;
		if (!bApplyRecoil && !bRecoilReset)
		{
			FiringError = 1;
		}
	}

	if (bIsFloating)
	{
		MovementError = 10;
	}

	// fire rate calculation
	if (!bCanFire)
	{
		FireRate -= DeltaTime * 6;
		if (FireRate < 0)
		{
			bCanFire = true;
		}
	}

	// firing error initiation
	if (FireCount > 4)
	{
		FiringError = FireCount;
	}

	// Recoil Application by adding rotational input
	// Recoil depends on the amount of bullets fired and movement 
	if (bApplyRecoil)
	{
		float RandomRecoil = FMath::RandRange(-1, 1);
		if (RecoilAmount > 0)
		{
			RecoilAmount -= DeltaTime * 9;
			if (bApplyHorizontalRecoil)
			{
				AddControllerYawInput(RecoilAmount * DeltaTime * RandomRecoil * 10);
				AddControllerPitchInput(RecoilAmount * DeltaTime * 2.5f * -1);
			}
			else
			{
				AddControllerPitchInput(RecoilAmount * DeltaTime * 5 * -1);
			}
		}
		else {
			bRecoilReset = true;
		}

		if (bRecoilReset)
		{
			if (RecoilDuration > 0)
			{
				if (bApplyHorizontalRecoil)
				{
					AddControllerYawInput(RecoilDuration * DeltaTime * RandomRecoil * 30 * -1);
					RecoilDuration -= DeltaTime * 7;
				}
				if (bIsFiring)
				{
					AddControllerPitchInput(RecoilDuration * DeltaTime);
					RecoilDuration -= DeltaTime * 9;
				}
				else {
					AddControllerPitchInput(RecoilDuration * DeltaTime * 5);
					RecoilDuration -= DeltaTime * 7;
				}
			}
			else {
				bApplyHorizontalRecoil = false;
				bRecoilReset = false;
				bApplyRecoil = false;
			}
		}
	}
	
	// raycast for interactbles detection
	FHitResult* HitResult = new FHitResult();
	FVector NewStartTrace = GetFirstPersonCameraComponent()->GetComponentLocation();
	FCollisionQueryParams* CQP = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, NewStartTrace, 
		((GetFirstPersonCameraComponent()->GetForwardVector() * 100.0f) + NewStartTrace), 
		ECC_Visibility, *CQP))
	{
		if (HitResult != NULL)
		{
			if (HitResult->GetActor()->ActorHasTag("Vandal"))
			{
				if (bInteractApplied)
				{
					EWeapon = EWeaponEquipped::EPrimary;
					bCanFire = true;
					ARadiantCharacter::Interact(HitResult);
				}
			}
			else
			{
				bInteractApplied = false;
			}
		}
	}
	else
	{
		bInteractApplied = false;
	}

	// If the vandal is not equipped currently, hide it until it is equipped
	if (EWeapon != EWeaponEquipped::EPrimary)
	{
		if (VandalInstance != nullptr)
		{
			VandalInstance->SetActorHiddenInGame(true);
		}
	}
}