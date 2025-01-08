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

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ARadiantCharacter::Interact);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARadiantCharacter::Jumping);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARadiantCharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ARadiantCharacter::OnPrimaryAction);

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
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
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

		//Vertical Direction
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

void ARadiantCharacter::Interact() 
{
	bInteractApplied = true;
}