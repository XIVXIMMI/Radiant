// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "Vandal.h"
#include "DirectionEnum.h"
#include "WeaponEnum.h"
#include "RadiantCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class AVandal;
class AEnemy;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopUseItem);

UCLASS(config=Game)
class ARadiantCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	ARadiantCharacter();

	// First Ability Boolean
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ability)
	bool bFirstAbilityApplied;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ability)
	bool bControllingFirstAbility;

	// Second Ability Boolean
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ability)
	bool bSecondAbilityApplied;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ability)
	bool bControllingSecondAbility;

	// Third Ability Boolean
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ability)
	bool bThirdAbilityApplied;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ability)
	bool bControllingThirdAbility;
	
	//Firing Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bCanFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	float FireRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	int32 FireCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
	bool bSpawnDecal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	FVector DecalLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	FVector_NetQuantizeNormal DecalNormal;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	FHitResult BodyPartHit;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	AActor* ActorHit;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
	int32 DamageDealt;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	AEnemy* EnemyHit;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	int32 Kills;
	
	// Movement & Firing Error Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	bool bIsFloating;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsFiring;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementError;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float FiringError;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shooting)
	bool bApplyRecoil;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shooting)
	bool bApplyHorizontalRecoil;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shooting)
	bool bRecoilReset;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shooting)
	float RecoilAmount;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shooting)
	float RecoilDuration;

	// Interaction Boolean
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ability)
	bool bInteractApplied;

	// Ult Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ult")
	int32 UltOrbs;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ult")
	int32 MaxUltOrbs;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ult")
	bool bUltUi;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ult")
	bool bUltActivated;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ult")
	bool bCanUlt;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ult")
	bool bUltFired;

	AVandal* VandalInstance;

	// Enums to identify direction and orientation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	TEnumAsByte<EMovementDirection> EDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	TEnumAsByte<EMovementDirection> EOrientation;

	// Enum to identify weapon currently equipped
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	TEnumAsByte<EWeaponEquipped> EWeapon;

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;
	
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnFireWeapon;
	
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnEnemyHit;

protected:
	
	/** Fires a projectile. */
	UFUNCTION()
	void OnPrimaryAction();
	
	void StopPrimaryAction();

	/*Interaction Function*/
	void Interact(FHitResult* OtherActor);

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	// Ability Methods
	void FirstAbility();
	
	void Jumping();
	
	void StopJumping();

	void SecondAbility();

	void ThirdAbility();

	void StopFirstAbility();

	void StopSecondAbility();

	void StopThirdAbility();

	void InteractPressed();
	
	void Ultimate();

	void Tick(float DeltaTime) override;

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

