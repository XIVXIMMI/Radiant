// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	EnemyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Enemy Mesh"));

	Health = 100;
	Armor = 50;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	bShouldDie = false;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::ApplyDamage(int32 Damage) 
{
	OnEnemyHit.Broadcast();
	if (Health > Damage) 
	{
		if (Armor > 0)
		{
			if (Armor < Damage)
			{
				Damage -= Armor;
				Armor = 0;
				Health -= Damage;
			}
			else
			{
				Armor -= Damage;
			}
		}
		else
		{
			Health -= Damage;
		}
	}
	else
	{
		bShouldDie = true;
	}
}

void AEnemy::Death() 
{
	OnEnemyDeath.Broadcast();
	Destroy();
}
