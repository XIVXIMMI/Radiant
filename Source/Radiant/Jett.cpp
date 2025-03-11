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
	bCanDash = true;
	DashResetKills = 0;
	DashResetStart = 0;

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

	MaxUltOrbs = 8;
	KnifeCount = 5;
}

void AJett::BeginPlay() 
{
	Super::BeginPlay();
}

void AJett::Dash()
{
	if (bCanDash)
	{
		// The first button press activates the dash and the next press executes it.
		// 2 broadcasts for audio
		if (bDashApplied)
		{
			OnDashStarted.Broadcast();
			bIsDashing = true;
		}
		else
		{
			OnDashActivated.Broadcast();
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
		DashResetStart = Kills;
	}
}

void AJett::Updraft()
{
	if (bCanUpdraft)
	{
		OnUpdraftStarted.Broadcast();
		LaunchCharacter(FVector(0, 0, 1000), false, false);
		bCanUpdraft = false;
		bUpdraftUI = false;
	}
}

AActor* AJett::CloudBurst()
{
	// Spawning params
	UWorld* const World = GetWorld();
	FRotator SpawnRotation = GetFirstPersonCameraComponent()->GetComponentRotation();
	FVector SpawnAdjustment = GetFirstPersonCameraComponent()->GetForwardVector() * 50;
	FVector SpawnLocation = GetFirstPersonCameraComponent()->GetComponentLocation() + SpawnAdjustment;
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Instigator = this;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedCloud = World->SpawnActor<ACloudBurst>(Cloud, SpawnLocation, SpawnRotation, ActorSpawnParams);

	CloudBurstStorage -= 1;

	// return the spawned cloudburst that will be controlled in the tick function
	return SpawnedCloud;
}

void AJett::Knives()
{
	// first we distinguish if the ult has been activated or fired
	// we spawn 5 knives, set their locations, and control their visibility 
	if (!bUltFired)
	{
		OnUltStarted.Broadcast();
		KnifeCount = 5;
		EWeapon = EWeaponEquipped::EAbility;
		bUltActivated = false;
		bCanUlt = false;

		// Spawn params
		UWorld* const World = GetWorld();
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Instigator = this;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		Knife1Instance = World->SpawnActor<AJettKnife>(Knife, GetActorLocation(),
			GetFirstPersonCameraComponent()->GetComponentRotation(), ActorSpawnParams);
		Knife1Instance->AttachToComponent(GetMesh1P(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			FName(TEXT("Knife1")));

		Knife2Instance = World->SpawnActor<AJettKnife>(Knife, GetActorLocation(),
			GetFirstPersonCameraComponent()->GetComponentRotation(), ActorSpawnParams);
		Knife2Instance->AttachToComponent(GetMesh1P(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			FName(TEXT("Knife2")));

		Knife3Instance = World->SpawnActor<AJettKnife>(Knife, GetActorLocation(),
			GetFirstPersonCameraComponent()->GetComponentRotation(), ActorSpawnParams);
		Knife3Instance->AttachToComponent(GetMesh1P(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			FName(TEXT("Knife3")));

		Knife4Instance = World->SpawnActor<AJettKnife>(Knife, GetActorLocation(),
			GetFirstPersonCameraComponent()->GetComponentRotation(), ActorSpawnParams);
		Knife4Instance->AttachToComponent(GetMesh1P(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			FName(TEXT("Knife4")));

		Knife5Instance = World->SpawnActor<AJettKnife>(Knife, GetActorLocation(),
			GetFirstPersonCameraComponent()->GetComponentRotation(), ActorSpawnParams);
		Knife5Instance->AttachToComponent(GetMesh1P(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			FName(TEXT("Knife5")));
	}
	else
	{
		// this part is similar to the firing function in the parent class, but without the firing/movement errors
		if (KnifeCount > 0)
		{
			KnifeCount--;
			/*RayCast Parameters*/
			FHitResult* HitResult = new FHitResult();
			FVector ForwardVector = GetFirstPersonCameraComponent()->GetForwardVector();
			FVector StartTrace = GetFirstPersonCameraComponent()->GetComponentLocation() + (ForwardVector * 200.0f);
			FVector RightVector = GetFirstPersonCameraComponent()->GetRightVector();
			FVector UpVector = GetFirstPersonCameraComponent()->GetUpVector();
			FVector EndTrace = StartTrace + (ForwardVector * 10000.0f);
			FCollisionQueryParams* CQP = new FCollisionQueryParams();
			CQP->bReturnPhysicalMaterial = true;

			if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Pawn, *CQP))
			{
				BodyPartHit = *HitResult;
				ActorHit = BodyPartHit.GetActor();
				if (HitResult->GetActor()->ActorHasTag("Enemy"))
				{
					EnemyHit = Cast<AEnemy>(ActorHit);
					if (EnemyHit != nullptr)
					{
						OnEnemyHit.Broadcast();
						EnemyHit->ApplyDamage(DamageDealt);
						if (EnemyHit->bShouldDie)
						{
							Kills += 1;
							KnifeCount = 5;
							EnemyHit->Death();
						}
					}
				}
			}
			OnFireWeapon.Broadcast();
			bUltFired = false;
			if (KnifeCount <= 0)
			{
				EWeapon = EWeaponEquipped::EPrimary;
				VandalInstance->SetActorHiddenInGame(false);
			}
		}
	}

	switch (KnifeCount)
	{
	default:
		break;

	case 5:
		Knife1Instance->SetActorHiddenInGame(false);
		Knife2Instance->SetActorHiddenInGame(false);
		Knife3Instance->SetActorHiddenInGame(false);
		Knife4Instance->SetActorHiddenInGame(false);
		Knife5Instance->SetActorHiddenInGame(false);
		break;
	case 4:
		Knife1Instance->SetActorHiddenInGame(true);
		break;
	case 3:
		Knife2Instance->SetActorHiddenInGame(true);
		break;
	case 2:
		Knife3Instance->SetActorHiddenInGame(true);
		break;
	case 1:
		Knife4Instance->SetActorHiddenInGame(true);
		break; 
	case 0:
		Knife5Instance->SetActorHiddenInGame(true);
		UltOrbs = 0;
		break;
	}
}

void AJett::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*Jett Drift*/
	if (bIsFloating && GetVelocity().Z < 0)
	{
		MovementError = 5;
		GetCharacterMovement()->GravityScale = 0.05;
	}
	else {
		GetCharacterMovement()->GravityScale = 1.2;
	}
	
	/*Jett Dash*/
	if (!bCanDash)
	{
		DashResetKills = Kills - DashResetStart;
		if (DashResetKills > 1)
		{
			bCanDash = true;
			DashResetKills = 0;
		}
	}
	else
	{
		bDashUI = true;
	}

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

	if (bUltActivated)
	{
		AJett::Knives();
	}

	if (bUltFired)
	{
		AJett::Knives();
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
							if (bIsDashing)
							{
								bCanDash = false;
								bDashApplied = false;
								bAbilityTimerStart = true;
							}
							bIsDashing = false;
						}
					}
				}
			}
		}

		if (bIsDashing)
		{
			bTimerUI = false;
			bDashUI = false;
			bCanDash = false;

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
}
