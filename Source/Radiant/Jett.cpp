// Fill out your copyright notice in the Description page of Project Settings.


#include "Jett.h"
#include "Kismet/GameplayStatics.h"

AJett::AJett() 
{
	/*Jett Dash Variables*/
	DashDistance = 10.0f;
	DashSpeed = 50.0f;
	DashTime = 0.0f;
	DashInterval = 0.0f;
	bDashApplied = false;
	DashActivationTimer = 3.0f;
	FirstAbilityCooldown = 10.0f;
	bAbilityTimerStart = false;
	bDashUI = true;
	bTimerUI = false;
	bIsDashing = false;

	//Cloudburst Variables
	CloudBurstSpawned = false;
	CloudBurstStorage = 2;

	//Jett Updraft Variables
	bCanUpdraft = true;
	bUpdraftUI = true;

	JettForwardVector;
	JettRightVector;

	bCanFire = false;
	FireRate = 1.00f;
	FireCount = 0;
}

void AJett::BeginPlay() 
{
	Super::BeginPlay();

	OnFireWeapon.AddDynamic(this, &AJett::Fire);
}

void AJett::Dash()
{
	if (!bIsDashing)
	{
		if (bDashApplied)
		{
			bCanDash = true;
		}
		DashActivationTimer = 3.0f;
		bDashApplied = true;
		bTimerUI = true;

		/*Dash Duration Calculation*/
		DashTime = DashDistance / DashSpeed;

		/*Orientation and Direction confirmation*/
		EDashDirection = EDirection;
		EDashOrientation = EOrientation;

		JettForwardVector = GetActorForwardVector();
		JettRightVector = GetActorRightVector();
	}
}

void AJett::Updraft()
{
	if (bCanUpdraft)
	{
		LaunchCharacter(FVector(0, 0, 1000), false, false);
		bCanUpdraft = false;
		bUpdraftUI = false;
	}
}

AActor* AJett::CloudBurst()
{
	UWorld* const World = GetWorld();

	FRotator SpawnRotation = GetFirstPersonCameraComponent()->GetComponentRotation();

	FVector SpawnAdjustment = GetFirstPersonCameraComponent()->GetForwardVector() * 50;

	FVector SpawnLocation = GetFirstPersonCameraComponent()->GetComponentLocation() + SpawnAdjustment;

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Instigator = this;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedCloud = World->SpawnActor<ACloudBurst>(Cloud, SpawnLocation, SpawnRotation, ActorSpawnParams);

	CloudBurstStorage -= 1;
	return SpawnedCloud;
}


void AJett::Interact(FHitResult* OtherActor)
{
	OtherActor->GetActor()->AttachToComponent(GetMesh1P(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("GripPoint")));
	bInteractApplied = false;
}

void AJett::Fire() 
{
	/*RayCast Parameters*/
	FHitResult* HitResult = new FHitResult();
	FVector StartTrace = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector ForwardVector = GetFirstPersonCameraComponent()->GetForwardVector();
	FCollisionQueryParams* CQP = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, ((ForwardVector * 1000.0f) + StartTrace), ECC_Visibility, *CQP))
	{
		DrawDebugLine(GetWorld(), StartTrace, (ForwardVector * 50000.0f + StartTrace), FColor(255, 255, 0), true);
	}
	FireCount += 1;
	bCanFire = false;
	FireRate = 1.00f;
}

void AJett::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*Jett Drift*/
	if (bIsFloating && GetVelocity().Z < 0)
	{
		GetCharacterMovement()->GravityScale = 0.05;
	}
	else {
		GetCharacterMovement()->GravityScale = 1.2;
	}
	
	/*Jett Dash*/
	if (bFirstAbilityApplied)
	{
		bFirstAbilityApplied = false;
		Dash();
	}

	/*Jett Updraft*/
	if (bSecondAbilityApplied)
	{
		bSecondAbilityApplied = false;
		if (bCanUpdraft)
		{
			Updraft();
		}
	}

	/*Jett CloudBurst*/
	if (bThirdAbilityApplied)
	{
		bThirdAbilityApplied = false;
		if (CloudBurstStorage > 0)
		{
			CloudBurstInstance = Cast<ACloudBurst>(CloudBurst());
		}
	}
	if (bControllingThirdAbility)
	{
		if (CloudBurstStorage > -1) 
		{
			CloudBurstInstance->CloudBurstMovement->Velocity = (GetFirstPersonCameraComponent()->GetForwardVector() * CloudBurstInstance->CloudBurstMovement->InitialSpeed);
		}
	}

	/*if the dash is completed, prevent the player from dashing again until the timer expires and control the UI*/
	if (bAbilityTimerStart)
	{
		bTimerUI = false;
		bDashUI = false;
		FirstAbilityCooldown -= DeltaTime;
		bFirstAbilityApplied = false;
		bDashApplied = false;
		bCanDash = false;

		if (FirstAbilityCooldown < 0.0f)
		{
			bDashUI = true;
			bAbilityTimerStart = false;
			FirstAbilityCooldown = 10.0f;
		}
	}

	if (!bCanFire) 
	{
		FireRate -= DeltaTime * 100;
		if (FireRate < 0) 
		{
			bCanFire = true;
		}
	}


	/*RayCast Parameters*/
	FHitResult* HitResult = new FHitResult();
	FVector StartTrace = GetMesh1P()->GetComponentLocation();
	StartTrace.Z = StartTrace.Z + 50;
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();
	FVector UpVector = GetActorUpVector();
	FCollisionQueryParams* CQP = new FCollisionQueryParams();

	if (bDashApplied)
	{
		/*Dash Collision Detection, stopping the player from dashing if there is an obstacle close*/
		if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, ((ForwardVector * 200.0f) + StartTrace), ECC_Visibility, *CQP))
		{
			if (HitResult != NULL)
			{
				/*Detection of smokes and other actors that should not interrupt the dash*/
				if (!HitResult->GetActor()->ActorHasTag("Weapon"))
				{
					if (!HitResult->GetActor()->ActorHasTag("Smoke"))
					{
						//DrawDebugLine(GetWorld(), StartTrace, ((ForwardVector * 200.0f) + StartTrace), FColor(255, 0, 0), true);
						if (EDashOrientation == EMovementDirection::EForward || EDashOrientation == EMovementDirection::EStationary)
						{
							if (bCanDash)
							{
								bIsDashing = false;
								bDashApplied = false;
								bAbilityTimerStart = true;
							}
							bCanDash = false;
						}
					}
				}
			}
		}

		if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, ((ForwardVector * -200.0f) + StartTrace), ECC_Visibility, *CQP))
		{
			if (HitResult != NULL)
			{
				/*Detection of smokes and other actors that should not interrupt the dash*/
				if (!HitResult->GetActor()->ActorHasTag("Weapon"))
				{
					if (!HitResult->GetActor()->ActorHasTag("Smoke"))
					{
						if (EDashOrientation == EMovementDirection::EBackward)
						{
							if (bCanDash)
							{
								bIsDashing = false;
								bDashApplied = false;
								bAbilityTimerStart = true;
							}
							bCanDash = false;
						}
					}
				}
			}
		}

		if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, ((RightVector * 200.0f) + StartTrace), ECC_Visibility, *CQP))
		{
			if (HitResult != NULL)
			{
				/*Detection of smokes and other actors that should not interrupt the dash*/
				if (!HitResult->GetActor()->ActorHasTag("Weapon"))
				{
					if (!HitResult->GetActor()->ActorHasTag("Smoke"))
					{
						if (EDashDirection == EMovementDirection::ERight)
						{
							if (bCanDash)
							{
								bIsDashing = false;
								bDashApplied = false;
								bAbilityTimerStart = true;
							}
							bCanDash = false;
						}
					}
				}
			}
		}

		if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, ((RightVector * -200.0f) + StartTrace), ECC_Visibility, *CQP))
		{
			if (HitResult != NULL)
			{
				/*Detection of smokes and other actors that should not interrupt the dash*/
				if (!HitResult->GetActor()->ActorHasTag("Weapon"))
				{
					if (!HitResult->GetActor()->ActorHasTag("Smoke"))
					{
						if (EDashDirection == EMovementDirection::ELeft)
						{
							if (bCanDash)
							{
								bIsDashing = false;
								bDashApplied = false;
								bAbilityTimerStart = true;
							}
							bCanDash = false;
						}
					}
				}
			}
		}

		if (bCanDash)
		{
			bTimerUI = false;
			bDashUI = false;
			bIsDashing = true;

			/*Distance to be travelled per tick*/
			DashInterval = DashSpeed * DeltaTime * 100.0f;
			FVector NewLocation = GetActorLocation();

			/*Dash Movement*/
			if (EDashDirection == EMovementDirection::ERight)
			{
				NewLocation += (JettRightVector * DashInterval);
				SetActorLocation(NewLocation);
			}
			if (EDashDirection == EMovementDirection::ELeft)
			{
				NewLocation += (JettRightVector * DashInterval * -1.0f);
				SetActorLocation(NewLocation);
			}
			if (EDashOrientation == EMovementDirection::EForward)
			{
				NewLocation += (JettForwardVector * DashInterval);
				SetActorLocation(NewLocation);
			}
			if (EDashOrientation == EMovementDirection::EStationary && EDashDirection == EMovementDirection::EStationary)
			{
				NewLocation += (JettForwardVector * DashInterval);
				SetActorLocation(NewLocation);
			}
			if (EDashOrientation == EMovementDirection::EBackward)
			{
				NewLocation += (JettForwardVector * DashInterval * -1.0f);
				SetActorLocation(NewLocation);
			}

			/*DashTimer countdown*/
			DashTime -= DeltaTime;
			if (DashTime < 0.0f)
			{
				bIsDashing = false;
				bDashApplied = false;
				bCanDash = false;
				bAbilityTimerStart = true;
			}
		}
		/*Timer set to limit the dash activation period*/
		DashActivationTimer -= DeltaTime;
		if (DashActivationTimer < 0.0f)
		{
			bTimerUI = false;
			bDashUI = false;
			bCanDash = false;
			bDashApplied = false;
			bAbilityTimerStart = true;
		}
	}

	FVector NewStartTrace = GetFirstPersonCameraComponent()->GetComponentLocation();
	if (GetWorld()->LineTraceSingleByChannel(*HitResult, NewStartTrace, ((GetFirstPersonCameraComponent()->GetForwardVector() * 100.0f) + NewStartTrace), ECC_Visibility, *CQP))
	{
		if (HitResult != NULL) 
		{
			if (HitResult->GetActor()->ActorHasTag("Vandal")) 
			{
				if (bInteractApplied)
				{
					EWeapon = EWeaponEquipped::EPrimary;
					bCanFire = true;
					AJett::Interact(HitResult);
					DrawDebugLine(GetWorld(), NewStartTrace, ((GetFirstPersonCameraComponent()->GetForwardVector() * 100.0f) + NewStartTrace), FColor(255, 0, 0), true);
				}
			}
			else
			{
				bInteractApplied = false;
			}
		}
	}
	else
	{
		bInteractApplied = false;
	}
}
