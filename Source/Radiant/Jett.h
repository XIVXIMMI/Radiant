// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RadiantCharacter.h"
#include "Engine.h"
#include "CloudBurst.h"
#include "Components/ActorComponent.h"
#include "Jett.generated.h"

class ACloudBurst;
/**
 * 
 */
UCLASS()
class RADIANT_API AJett : public ARadiantCharacter
{
	GENERATED_BODY()
	
public:
	//constructor
	AJett();

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


protected:
	/*Jett Dash*/
	void Dash();

	/*Jett Updraft*/
	void Updraft();

	/*Jett Cloudburst*/
	AActor* CloudBurst();

	/*Interaction*/
	void Interact(FHitResult* OtherActor);

	void Tick(float DeltaTime) override;
};
