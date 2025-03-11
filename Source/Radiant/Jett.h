// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RadiantCharacter.h"
#include "Engine.h"
#include "CloudBurst.h"
#include "JettKnife.h"
#include "Components/ActorComponent.h"
#include "Jett.generated.h"

class ACloudBurst;
class AJettKnife;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilityAudio);
/**
 Jett Class
 */
UCLASS()
class RADIANT_API AJett : public ARadiantCharacter
{
	GENERATED_BODY()
	
public:
	//constructor
	AJett();

	/* Knife class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Ult)
	TSubclassOf<class AJettKnife> Knife;

	AJettKnife* Knife1Instance;
	AJettKnife* Knife2Instance;
	AJettKnife* Knife3Instance;
	AJettKnife* Knife4Instance;
	AJettKnife* Knife5Instance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ult")
	int32 KnifeCount;

	/* CloudBurst class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Smoke)
	TSubclassOf<class ASmoke> Cloud;
	
	ACloudBurst* CloudBurstInstance;

	
	//Dash Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dash)
	float DashSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dash)
	float DashDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash)
	float DashActivationTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash)
	float DashTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash)
	float FirstAbilityCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash)
	float DashInterval;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dash)
	bool bDashApplied;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dash)
	bool bCanDash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dash)
	bool bAbilityTimerStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dash)
	bool bDashUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dash)
	bool bTimerUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dash)
	bool bIsDashing;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dash)
	int32 DashResetStart;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dash)
	int32 DashResetKills;
	

	// Movement Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<EMovementDirection> EDashDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<EMovementDirection> EDashOrientation;

	FVector JettForwardVector;

	FVector JettRightVector;


	// Updraft Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Updraft)
	bool bCanUpdraft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUpdraftUI;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bInteractable;


	// CloudBurst Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CloudBurst)
	bool bCanCloudBurst;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CloudBurst)
	bool CloudBurstSpawned;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CloudBurst)
	int CloudBurstStorage;
	
	// Audio Variables
	UPROPERTY(BlueprintAssignable, Category = "Audio")
	FOnAbilityAudio OnDashActivated;
	
	UPROPERTY(BlueprintAssignable, Category = "Audio")
	FOnAbilityAudio OnDashStarted;
	
	UPROPERTY(BlueprintAssignable, Category = "Audio")
	FOnAbilityAudio OnUpdraftStarted;
	
	UPROPERTY(BlueprintAssignable, Category = "Audio")
	FOnAbilityAudio OnUltStarted;


protected:
	/*Jett Dash*/
	void Dash();

	/*Jett Updraft*/
	void Updraft();

	/*Jett Cloudburst*/
	AActor* CloudBurst();

	/*Jett Ult*/
	void Knives();

	virtual void BeginPlay();

	void Tick(float DeltaTime) override;
};
