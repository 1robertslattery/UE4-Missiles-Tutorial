// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "MissileDemo.h"

const FName AMyCharacter::horizontalAnim("HorizontalVar");
const FName AMyCharacter::verticalAnim("VerticalVar");
const FName AMyCharacter::jumpAnim("Jumping");

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerMesh = GetMesh();
	PlayerCapsule = GetCapsuleComponent();
	PlayerMove = GetCharacterMovement();
	animInstance = NULL;
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (PlayerMesh)
		animInstance = Cast<UAnimInstance>(PlayerMesh->GetAnimInstance());
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerMove->IsMovingOnGround())
		StopJump();
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("Horizontal", this, &AMyCharacter::MoveForward);
	InputComponent->BindAxis("Vertical", this, &AMyCharacter::MoveLeftRight);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::StartJump);
}

void AMyCharacter::MoveForward(float v)
{
	UFloatProperty *forwardProp = FindField<UFloatProperty>(animInstance->GetClass(), horizontalAnim);

	if (forwardProp != NULL)
	{
		float val = forwardProp->GetPropertyValue_InContainer(animInstance);
		forwardProp->SetPropertyValue_InContainer(animInstance, v);
		val = forwardProp->GetPropertyValue_InContainer(animInstance);
	}

	if ((Controller != NULL) && (v != 0))
	{
		AddMovementInput(GetActorRightVector(), v);
	}
}

void AMyCharacter::MoveLeftRight(float h)
{
	UFloatProperty *verticalProp = FindField<UFloatProperty>(animInstance->GetClass(), verticalAnim);

	if (verticalProp != NULL)
	{
		float retVal = verticalProp->GetPropertyValue_InContainer(animInstance);
		verticalProp->SetPropertyValue_InContainer(animInstance, h);
		retVal = verticalProp->GetPropertyValue_InContainer(animInstance);
	}

	if ((Controller != NULL) && (h != 0))
		AddMovementInput(GetActorForwardVector(), h);
}

void AMyCharacter::StartJump()
{
	if (!animInstance)
		return;

	UBoolProperty *jumpProp = FindField<UBoolProperty>(animInstance->GetClass(), jumpAnim);

	if (jumpProp != NULL)
	{
		float retVal = jumpProp->GetPropertyValue_InContainer(animInstance);
		jumpProp->SetPropertyValue_InContainer(animInstance, true);
		retVal = jumpProp->GetPropertyValue_InContainer(animInstance);
	}

	if (PlayerMove->IsMovingOnGround())
		bPressedJump = true;
}

void AMyCharacter::StopJump()
{
	if (!animInstance)
		return;

	UBoolProperty *jumpProp = FindField<UBoolProperty>(animInstance->GetClass(), jumpAnim);

	if (jumpProp != NULL)
	{
		float retVal = jumpProp->GetPropertyValue_InContainer(animInstance);
		jumpProp->SetPropertyValue_InContainer(animInstance, false);
		retVal = jumpProp->GetPropertyValue_InContainer(animInstance);
	}

	if (bPressedJump) 
		bPressedJump = false;
}

