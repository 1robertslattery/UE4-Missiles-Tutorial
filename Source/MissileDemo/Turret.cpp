// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"
#include "MissileDemo.h"
#include "Missile.h"

// Sets default values
ATurret::ATurret(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Construct Collision Component
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CollisionComp"));
	RootComponent = CollisionComp;

	// Construct Static Mesh Component
	TurretMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("TurretMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Game/StarterContent/Shapes/Shape_Tube"));
	TurretMesh->SetStaticMesh(MeshObj.Object);
	TurretMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	InitMissileLaunch();
}

#pragma region Missile Logic
// Spawn a new missile every 3 seconds
void ATurret::InitMissileLaunch()
{
	GetWorldTimerManager().SetTimer(MissileHandle, this, &ATurret::SpawnMissile, 3.f, true);
}

// Spawn missile from the turret position
void ATurret::SpawnMissile()
{
	if (MissileClass != NULL)
	{
		class UWorld* const world = GetWorld();

		if (world != NULL)
		{
			FVector currentPos = GetActorLocation();
			FRotator currentRot = FRotator(0, 0, 0);

			FActorSpawnParameters spawnParams;
			spawnParams.Owner = this;
			spawnParams.Instigator = Instigator;

			class AMissile* FiredMissile = world->SpawnActor<AMissile>(MissileClass, currentPos, currentRot, spawnParams);

			if (FiredMissile != nullptr)
			{
				// Set Mesh Rotation Offset. This rotation is based upon how your missile FBX was modeled.
				FRotator meshRot = FiredMissile->MissileMesh->GetComponentRotation();
				meshRot.Roll = 0.f;
				meshRot.Pitch = -90.f;
				meshRot.Yaw = 0.f;
				FiredMissile->MissileMesh->SetRelativeRotation(meshRot);
			}
		}
	}
}
#pragma endregion

