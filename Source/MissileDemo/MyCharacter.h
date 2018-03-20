// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class MISSILEDEMO_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMyCharacter();

protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	class UAnimInstance* animInstance;

public:	

	virtual void Tick(float DeltaTime) override;
	void MoveForward(float v);
	void MoveLeftRight(float h);

	UFUNCTION()
	virtual void StartJump();

	UFUNCTION()
	virtual void StopJump();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	class USkeletalMeshComponent* PlayerMesh;
	class UCapsuleComponent* PlayerCapsule;
	class UCharacterMovementComponent* PlayerMove;
	
	static const FName horizontalAnim;
	static const FName verticalAnim;
	static const FName jumpAnim;

};
