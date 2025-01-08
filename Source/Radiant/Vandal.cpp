// Fill out your copyright notice in the Description page of Project Settings.


#include "Vandal.h"
#include "Components/SphereComponent.h"

// Sets default values
AVandal::AVandal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VandalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Vandal Mesh"));
	
	PickUpSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Sphere"));

	PickUpSphere->SetupAttachment(VandalMesh);

	//PickUpSphere->OnComponentHit.AddDynamic(this, &AVandal::OnHit);

	bDisplayUI = false;

}

// Called when the game starts or when spawned
void AVandal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVandal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVandal::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) 
{
	if (OtherActor != NULL) 
	{

	}
}

