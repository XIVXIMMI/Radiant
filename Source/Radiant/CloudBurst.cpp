// Fill out your copyright notice in the Description page of Project Settings.


#include "CloudBurst.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

ACloudBurst::ACloudBurst()
{
	CloudBurstMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	RootComponent = OuterSmoke;
	//CloudBurst movement physics
	CloudBurstMovement->UpdatedComponent = OuterSmoke;
	CloudBurstMovement->InitialSpeed = 3000.0f;
	CloudBurstMovement->MaxSpeed = 3000.0f;

	OuterSmoke->OnComponentHit.AddDynamic(this, &ACloudBurst::OnHit);

	FullCloudTimer = 3.0f;
	FlyingTimer = 3.0f;
	DidItHit = false;

	LerpDigit = 2.0f;
}

void ACloudBurst::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//InnerSmoke->SetRelativeLocation(OuterSmoke->GetRelativeLocation());
	if (DidItHit)
	{
		LerpDigit += (DeltaTime * 10 * (7.5f - LerpDigit));
		FullCloudTimer -= DeltaTime;
		if (LerpDigit < 7.0f) 
		{
			OuterSmoke->SetWorldScale3D(FVector(LerpDigit, LerpDigit, LerpDigit));
			InnerSmoke->SetWorldScale3D(FVector(LerpDigit - 0.1f, LerpDigit - 0.1f, LerpDigit - 0.1f));
		}
		if (FullCloudTimer < 0)
		{
			Destroy();
		}
	}
	else {
		FlyingTimer -= DeltaTime;
		if (FlyingTimer < 0) 
		{ 
			CloudBurstMovement->Velocity = (FVector(0, 0, 0));
			CloudBurstMovement->ProjectileGravityScale = 0;
			DidItHit = true;
		}
	}
}

void ACloudBurst::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	DidItHit = true;
	CloudBurstMovement->Velocity = (FVector(0, 0, 0));
	CloudBurstMovement->ProjectileGravityScale = 0;
}

void ACloudBurst::BeginPlay() 
{
	Super::BeginPlay();
}
