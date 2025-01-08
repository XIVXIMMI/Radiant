// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Smoke.h"
#include "CloudBurst.generated.h"

class UProjectileMovementComponent;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RADIANT_API ACloudBurst : public ASmoke
{
	GENERATED_BODY()


public:
	ACloudBurst();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	UProjectileMovementComponent* CloudBurstMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	bool DidItHit;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	float FullCloudTimer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	float FlyingTimer;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly)
	float LerpDigit; 
};
