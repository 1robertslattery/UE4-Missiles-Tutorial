// Fill out your copyright notice in the Description page of Project Settings.

#include "Missile.h"
#include "MissileDemo.h"
#include "MyCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

// Sets default values
AMissile::AMissile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Construct Collision Component
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CollisionComp"));
	RootComponent = CollisionComp;

	// Construct Static Mesh Component
	MissileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MissileMesh"));
	//const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Game/Missile/Missile_01_Model"));
	//MissileMesh->SetStaticMesh(MeshObj.Object);
	MissileMesh->SetupAttachment(RootComponent);

	// Construct Projectile Movement Component
	ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 200.f;
	ProjectileMovement->MaxSpeed = 300.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bInitialVelocityInLocalSpace = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bIsHomingProjectile = false;
	ProjectileMovement->HomingAccelerationMagnitude = 0.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->Velocity = FVector(0, 0, 0);

	// Bind our OnOverlapBegin Event
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AMissile::OnOverlapBegin);

	// Set Default Values for Variables
	hasTargetPosition = false;
	hasNoTarget = false;
	target = NULL;
	delayTimer = 0.f;
	hasFinishedDelay = false;
	lifetimeCountdown = 15.f;
	canBeDestroyed = false;
	PlayerCharacter = NULL;
}

#pragma region Setup Target Logic
// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();
	FindPlayer();

	// Before we find our target, launch the Missile upwards
	if (!hasTargetPosition) 
	{
		ProjectileMovement->Velocity = GetActorUpVector() * 300.f;

		// BeginPlay with collision disabled so that the missile doesn't interact with other actors
		this->SetActorEnableCollision(false);
	}
}

// This will find our missile's target
void AMissile::FindPlayer()
{
	class UWorld* const world = GetWorld();

	if (world)
	{
		for (TActorIterator<AMyCharacter> ObstacleItr(world); ObstacleItr; ++ObstacleItr)
		{
			FName PlayerTagName = FName(TEXT("Player"));
			class AMyCharacter* FoundPlayer = *ObstacleItr;

			if (FoundPlayer != nullptr)
			{
				if (FoundPlayer->ActorHasTag(PlayerTagName))
				{
					if (PlayerCharacter != FoundPlayer)
					{
						PlayerCharacter = FoundPlayer;
					}
				}
			}
		}
	}
}

// Once our delay has finished, we search for possible targets
void AMissile::UpdateTarget()
{
	if (!hasTargetPosition)
	{
		if (target == NULL)
		{
			if (PlayerCharacter->IsValidLowLevel())
			{
				target = PlayerCharacter;
				hasTargetPosition = true;
				hasNoTarget = false;
				
				// Keep Our Mesh Rotation Offset
				FRotator rotVal = MissileMesh->GetComponentRotation();
				rotVal.Roll = 0.f;
				rotVal.Pitch = -90.f;
				rotVal.Yaw = 0.f;
				MissileMesh->SetRelativeRotation(rotVal);
			}
			else
			{
				target = nullptr;
				hasTargetPosition = true;
				hasNoTarget = true;
			}
		}
		else
		{
			target = nullptr;
			hasTargetPosition = true;
			hasNoTarget = true;
		}
	}
}

// Allow 1 second to pass before finding our target
void AMissile::DelayLogic(float dTime)
{
	if (!hasFinishedDelay)
	{
		delayTimer += 1 * dTime;

		if (delayTimer > 1.f)
		{
			UpdateTarget();
			this->SetActorEnableCollision(true);
			hasFinishedDelay = true;
		}
	}
}
#pragma endregion

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!hasFinishedDelay) 
	{
		DelayLogic(DeltaTime);
	}
	else 
	{
		// If a target is found, move the missile actor toward target
		if (hasTargetPosition)
		{
			if (target != NULL)
			{
				if (target->IsValidLowLevel())
				{
					FVector wantedDir = (target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
					wantedDir += target->GetVelocity() * wantedDir.Size() / 200.f;
					ProjectileMovement->Velocity += wantedDir * 200.f * DeltaTime;
					ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * 200.f;
				}
				else
				{
					if (!this->IsPendingKill())
						if (this->IsValidLowLevel())
							K2_DestroyActor();
				}
			}
			else
			{
				// If a target is NOT found, continue to move the missile actor upwards
				if (hasNoTarget)
				{
					ProjectileMovement->Velocity = GetActorUpVector() * 200.f;
					hasNoTarget = false;
				}
			}
		}

		// Destroy the missile actor after a 15 Second countdown
		lifetimeCountdown -= 1 * DeltaTime;

		if (lifetimeCountdown < 0.f)
		{
			if (!canBeDestroyed)
			{
				canBeDestroyed = true;
				target = nullptr;
				Explode();
			}
		}
	}
}

#pragma region Overlap Events
// If our missile overlaps the player or the ground, it will be destroyed
void AMissile::OnOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &hitResult)
{
	class AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(otherActor);
	class AStaticMeshActor* GroundActor = Cast<AStaticMeshActor>(otherActor);

	if (PlayerCharacter != nullptr)
	{
		PlayExplosion(ExplosionSystem);
		PlayExplosionSound(ExplosionSound);

		if (this->IsValidLowLevel())
			Destroy();
	}

	if (GroundActor != nullptr)
	{
		PlayExplosion(ExplosionSystem);
		PlayExplosionSound(ExplosionSound);

		if (this->IsValidLowLevel())
			Destroy();
	}
}
#pragma endregion

#pragma region End of Play Logic
// Our destroy method
void AMissile::Explode()
{
	PlayExplosion(ExplosionSystem);
	PlayExplosionSound(ExplosionSound);

	if (this->IsValidLowLevel())
		Destroy();
}

// Spawn our explosion particle system
class UParticleSystemComponent* AMissile::PlayExplosion(class UParticleSystem* explosion)
{
	class UParticleSystemComponent* retVal = NULL;

	if (explosion)
	{
		class UWorld* const world = GetWorld();

		if (world)
		{
			FVector myPos = GetActorLocation();
			FRotator myRot = GetActorRotation();

			retVal = UGameplayStatics::SpawnEmitterAtLocation(world, explosion, myPos, myRot, true);
		}
	}

	return retVal;
}

// Spawn our explosion sound
class UAudioComponent* AMissile::PlayExplosionSound(class USoundCue *sound)
{
	class UAudioComponent* retVal = NULL;

	if (sound)
		retVal = UGameplayStatics::SpawnSoundAttached(sound, this->GetRootComponent());

	return retVal;
}
#pragma endregion


