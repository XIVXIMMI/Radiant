// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemySpawn::AEnemySpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Bounds"));
	EnemiesSpawned = 0;
}

// Called when the game starts or when spawned
void AEnemySpawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemySpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (EnemiesSpawned < 3)
	{
		AEnemySpawn::SpawnEnemy();
	}
}


void AEnemySpawn::SpawnEnemy() 
{
	// Spawn params --- spawn location is a random coordinate within a box
	UWorld* const World = GetWorld();
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FVector RandomSpawnPoint = SpawnBounds->GetRelativeLocation();
	RandomSpawnPoint.X += FMath::RandRange(-600, 600);
	RandomSpawnPoint.Y += FMath::RandRange(-600, 600);

	// Bot spawning manager
	BotInstance = World->SpawnActor<AEnemy>(BotToSpawn,RandomSpawnPoint, GetActorRotation(), ActorSpawnParams);
	BotInstance->OnEnemyDeath.AddDynamic(this, &AEnemySpawn::SpawnEnemy);
	EnemiesSpawned++;
}
