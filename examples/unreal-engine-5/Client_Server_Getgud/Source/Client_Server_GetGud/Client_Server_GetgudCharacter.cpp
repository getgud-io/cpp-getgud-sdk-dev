// Copyright Epic Games, Inc. All Rights Reserved.

#include "Client_Server_GetgudCharacter.h"
#include "GetgudSDK.h"
#include "Client_Server_GetgudProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

extern std::string g_matchGuid;
std::string g_playerGuid;

//////////////////////////////////////////////////////////////////////////
// AClient_Server_GetgudCharacter

AClient_Server_GetgudCharacter::AClient_Server_GetgudCharacter()
{
	//bReplicates = true;
	//SetReplicatingMovement(true);
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	FString CharName = GetName();

	g_playerGuid = TCHAR_TO_UTF8(*CharName);

}

void AClient_Server_GetgudCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (IsLocallyControlled()) // Make sure this is the local client
	{
		// Call the server RPC function
		ServerRemoteSpawn(Mesh1P->GetPosition());
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AClient_Server_GetgudCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClient_Server_GetgudCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AClient_Server_GetgudCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AClient_Server_GetgudCharacter::Move(const FInputActionValue& Value)
{
	if (IsLocallyControlled()) // Make sure this is the local client
	{
		// Call the server RPC function
		ServerRemoteMove(Value);
	}

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AClient_Server_GetgudCharacter::Look(const FInputActionValue& Value)
{
	if (IsLocallyControlled()) // Make sure this is the local client
	{
		// Call the server RPC function
		ServerRemoteMove(Value);
	}

	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AClient_Server_GetgudCharacter::ServerRemoteMove_Implementation(const FInputActionValue& Value)
{
	// Handle the movement on the server
	//Move(Value); // Call the local move function

	FRotator CameraRotation = Controller->GetControlRotation();
	FVector position = GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("Character Position: %s"), *position.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Character Rotation: %s"), *CameraRotation.ToString());

	FDateTime Now = FDateTime::UtcNow();

	// Calculate the Unix timestamp in milliseconds
	int64 UnixTimestampMillis = (Now.GetTicks() - FDateTime(1970, 1, 1, 0, 0, 0, 0).GetTicks()) / ETimespan::TicksPerMillisecond;

	float pitch = 0;
	if (CameraRotation.Pitch >= 0 && CameraRotation.Pitch <= 90)
	{
		pitch = CameraRotation.Pitch * -1;
	}
	else if (CameraRotation.Pitch >= 270 && CameraRotation.Pitch <= 360)
	{
		pitch = 360 - CameraRotation.Pitch;
	}

	float yaw = 0;

	if (CameraRotation.Yaw >= 0 && CameraRotation.Yaw <= 180)
	{
		yaw = CameraRotation.Yaw * -1;
	}
	else if (CameraRotation.Yaw >= 180 && CameraRotation.Yaw <= 360)
	{
		yaw = 360 - CameraRotation.Yaw;
	}

	GetgudSDK::BaseActionData* outAction = nullptr;
	outAction = new GetgudSDK::PositionActionData(
		g_matchGuid, UnixTimestampMillis, g_playerGuid,
		GetgudSDK::PositionF{ (float)position.X, (float)position.Y, (float)position.Z },
		GetgudSDK::RotationF{pitch, yaw, 0});

	GetgudSDK::SendAction(outAction);

	delete outAction;
}

bool AClient_Server_GetgudCharacter::ServerRemoteMove_Validate(const FInputActionValue& Value)
{
	// Add any necessary validation here
	return true; // Assume validation passes
}

void AClient_Server_GetgudCharacter::ServerRemoteSpawn_Implementation(const FInputActionValue& Value)
{
	// Handle the movement on the server

	FVector position = GetActorLocation();
	FRotator CameraRotation = Controller->GetControlRotation();
	UE_LOG(LogTemp, Warning, TEXT("Character Position: %s"), *position.ToString());

	FDateTime Now = FDateTime::UtcNow();

	// Calculate the Unix timestamp in milliseconds
	int64 UnixTimestampMillis = (Now.GetTicks() - FDateTime(1970, 1, 1, 0, 0, 0, 0).GetTicks()) / ETimespan::TicksPerMillisecond;

	GetgudSDK::BaseActionData* outAction = nullptr;


	float pitch = 0;
	if (CameraRotation.Pitch >= 0 && CameraRotation.Pitch <= 90)
	{
		pitch = CameraRotation.Pitch * -1;
	}
	else if (CameraRotation.Pitch >= 270 && CameraRotation.Pitch <= 360)
	{
		pitch = 360 - CameraRotation.Pitch;
	}

	float yaw = 0;
	if (CameraRotation.Yaw >= 0 && CameraRotation.Yaw <= 180)
	{
		yaw = CameraRotation.Yaw * -1;
	}
	else if (CameraRotation.Yaw >= 180 && CameraRotation.Yaw <= 360)
	{
		yaw = 360 - CameraRotation.Yaw;
	}

	outAction = new GetgudSDK::SpawnActionData(
		g_matchGuid, UnixTimestampMillis, g_playerGuid, "halls_green", 0, 100.f,
		GetgudSDK::PositionF{(float)position.X, (float)position.Y, (float)position.Z},
		GetgudSDK::RotationF{pitch, yaw, 0});

	GetgudSDK::SendAction(outAction);

	delete outAction;
}

bool AClient_Server_GetgudCharacter::ServerRemoteSpawn_Validate(const FInputActionValue& Value)
{
	// Add any necessary validation here
	return true; // Assume validation passes
}