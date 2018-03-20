// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

UCLASS()
class MISSILEDEMO_API AMissile : public AActor
{
	GENERATED_BODY()
	
private:

	// Target Variables
	bool hasTargetPosition;
	bool hasNoTarget;
	class AActor* target;

	// Delay Variables
	float delayTimer;
	bool hasFinishedDelay;

	// Actor Lifetime Variables
	float lifetimeCountdown;
	bool canBeDestroyed;
	void Explode();

	// Event to Detect When an Actor Overlaps the Missile Class
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &hitResult);
		
public:	

	// Sets default values for this actor's properties
	AMissile(const FObjectInitializer& ObjectInitializer);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Reference To Our Player in World
	UPROPERTY()
	class AMyCharacter* PlayerInWorld;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Allow a Short Delay
	void DelayLogic(float dTime);

	// Find Target In World
	void FindPlayer();
	void UpdateTarget();

	// Collision Component for Missile Class
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class UBoxComponent* CollisionComp;

	// Static Mesh Component for Missile Class
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* MissileMesh;
	
	// Projectile Movement Component for Missile Class
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovement;

	// Explosion
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class UParticleSystem* ExplosionSystem;

	class UParticleSystemComponent* PlayExplosion(class UParticleSystem* explosion);

	// Sounds
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundCue* ExplosionSound;

	class UAudioComponent* PlayExplosionSound(class USoundCue *sound);

};
