// Fill out your copyright notice in the Description page of Project Settings.


#include "JettKnife.h"

// Sets default values
AJettKnife::AJettKnife()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Knife = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KNIFE"));

}

// Called when the game starts or when spawned
void AJettKnife::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJettKnife::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

