// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

UCLASS()
class MISSILEDEMO_API ATurret : public AActor
{
	GENERATED_BODY()

private:

	// Timer Handle for Repeating Missile Fire
	FTimerHandle MissileHandle;

public:	

	// Sets default values for this actor's properties
	ATurret(const FObjectInitializer& ObjectInitializer);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	// Spawn Missiles
	void InitMissileLaunch();
	void SpawnMissile();

	// Collision Component for Turret Class
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class UBoxComponent* CollisionComp;

	// Static Mesh Component for Turret Class
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* TurretMesh;

	// Missile Blueprint Class To Fire
	UPROPERTY(EditDefaultsOnly, Category = "Turret Projectiles")
	TSubclassOf<class AMissile> MissileClass;
	
};
