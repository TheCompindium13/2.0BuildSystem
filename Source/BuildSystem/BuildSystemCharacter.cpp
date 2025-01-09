// Copyright Epic Games, Inc. All Rights Reserved.

#include "BuildSystemCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ABuildSystemCharacter

ABuildSystemCharacter::ABuildSystemCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ABuildSystemCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABuildSystemCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABuildSystemCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABuildSystemCharacter::Look);
		// Building
		EnhancedInputComponent->BindAction(BuildAction, ETriggerEvent::Triggered, this, &ABuildSystemCharacter::StartBuildingMode);
		EnhancedInputComponent->BindAction(BuildAction, ETriggerEvent::Triggered, this, &ABuildSystemCharacter::StopBuildingMode);
		EnhancedInputComponent->BindAction(PlaceAction, ETriggerEvent::Triggered, this, &ABuildSystemCharacter::PlaceStructure);
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ABuildSystemCharacter::RotatePreview);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABuildSystemCharacter::StartBuildingMode()
{
	if (!SelectedStructure || bIsBuildingMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Building Mode could not start. Invalid SelectedStructure or already in building mode."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Start Building Mode Activated"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Start Building Mode Activated"));

	FVector SpawnLocation = Owner->GetActorLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	UE_LOG(LogTemp, Warning, TEXT("Attempting to spawn actor at %s"), *SpawnLocation.ToString());
	// Spawn a preview actor for building
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	PreviewActor = GetWorld()->SpawnActor<ABuildableActor>(SelectedStructure., SpawnLocation, SpawnRotation);
	UE_LOG(LogTemp, Warning, TEXT("PreviewActor spawned successfully at %s"), *SpawnLocation.ToString());
			if (UStaticMeshComponent* MeshComponent = PreviewActor->FindComponentByClass<UStaticMeshComponent>())
			{
				if (MeshComponent->GetStaticMesh())
				{
					UE_LOG(LogTemp, Warning, TEXT("PreviewActor Static Mesh assigned successfully."));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("PreviewActor spawned but Static Mesh is missing!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PreviewActor spawned but Static Mesh Component is missing!"));
			}
	
			bIsBuildingMode = true;

	//// Spawn a preview actor for building
	//FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = this;
//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
	//FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() *;
	//FRotator SpawnRotation = FRotator::ZeroRotator;

	//UE_LOG(LogTemp, Warning, TEXT("Attempting to spawn actor at %s"), *SpawnLocation.ToString());

	//PreviewActor = GetWorld()->SpawnActor<ABuildableActor>(SelectedStructure, SpawnLocation, SpawnRotation, SpawnParams);
//
//	if (PreviewActor)
//	{
//		PreviewActor->SetActorEnableCollision(false);
//
//		if (UStaticMeshComponent* MeshComponent = PreviewActor->FindComponentByClass<UStaticMeshComponent>())
//		{
//			if (MeshComponent->GetStaticMesh())
//			{
//				UE_LOG(LogTemp, Warning, TEXT("PreviewActor Static Mesh assigned successfully."));
//			}
//			else
//			{
//				UE_LOG(LogTemp, Error, TEXT("PreviewActor spawned but Static Mesh is missing!"));
//			}
//		}
//		else
//		{
//			UE_LOG(LogTemp, Error, TEXT("PreviewActor spawned but Static Mesh Component is missing!"));
//		}
//
//		bIsBuildingMode = true;
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("Failed to spawn PreviewActor!"));
//	}
}
void ABuildSystemCharacter::StopBuildingMode()
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}
	bIsBuildingMode = false;
}

void ABuildSystemCharacter::PlaceStructure()
{
	if (!PreviewActor || !bIsBuildingMode) return;

	UE_LOG(LogTemp, Warning, TEXT("Structure Placed"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Structure Placed"));

	// Finalize the structure placement
	FTransform SpawnTransform = PreviewActor->GetActorTransform();
	StopBuildingMode();

	GetWorld()->SpawnActor<ABuildableActor>(SelectedStructure, SpawnTransform);
}

void ABuildSystemCharacter::RotatePreview(const FInputActionValue& Value)
{
	if (PreviewActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ROTATING Building Mode Activated"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("ROTATING Building Mode Activated"));
		float RotationValue = Value.Get<float>();
		UE_LOG(LogTemp, Warning, TEXT("Rotating Preview: %f"), RotationValue);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Rotating Preview: %f"), RotationValue));
		FRotator NewRotation = PreviewActor->GetActorRotation();
		NewRotation.Yaw += RotationValue * 10.0f;
		PreviewActor->SetActorRotation(NewRotation);
	}
}

void ABuildSystemCharacter::UpdatePreviewLocation()
{
	UE_LOG(LogTemp, Warning, TEXT("UPDATBuilding Mode Activated"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UPDAT Building Mode Activated"));
	if (PreviewActor && bIsBuildingMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPDATING Building Mode Activated"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UPDATING Building Mode Activated"));
		FHitResult Hit;
		FVector Start = FollowCamera->GetComponentLocation();
		FVector End = Start + (FollowCamera->GetForwardVector());

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
		{
			FVector Location = Hit.Location;
			PreviewActor->SetActorLocation(Location);
		}
	}
}

void ABuildSystemCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABuildSystemCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}