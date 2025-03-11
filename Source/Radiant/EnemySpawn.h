// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "EnemySpawn.generated.h"

class AEnemy;
class UBoxComponent;

UCLASS()
class RADIANT_API AEnemySpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawn();

	UPROPERTY(EditDefaultsOnly, Category = Spawn)
	TSubclassOf<class AEnemy> BotToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = Spawn)
	AEnemy* BotInstance;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = Spawn)
	UBoxComponent* SpawnBounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Spawn)
	FBox SpawnArea;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Spawn)
	int32 EnemiesSpawned;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SpawnEnemy();

};
