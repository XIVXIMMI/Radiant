// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"

class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyHit);

UCLASS()
class RADIANT_API AEnemy : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* EnemyMesh;

public:	
	// Sets default values for this actor's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Health;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Armor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bShouldDie;

	UPROPERTY(BlueprintAssignable, Category = "Death")
	FOnEnemyDeath OnEnemyDeath;
	
	UPROPERTY(BlueprintAssignable, Category = "Death")
	FOnEnemyHit OnEnemyHit;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ApplyDamage(int32 Damage);

	UFUNCTION()
	void Death();

};
