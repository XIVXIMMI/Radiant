// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Vandal.generated.h"

class USphereComponent;

UCLASS()
class RADIANT_API AVandal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVandal();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* VandalMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USphereComponent* PickUpSphere;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
