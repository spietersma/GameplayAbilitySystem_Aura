// Copyright Scared and Confused


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	if(ThisActor != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(1, 3, FColor::Red, TEXT("Got something"));
	}

}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// Assert - Will crash if not set
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2d InputAxisVector = InputActionValue.Get<FVector2d>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation (0.f, Rotation.Yaw, 0.f );

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		// Forward (W/D Keys) is from the angle of top down -> ergo Y is forwards and backwards, X is left and right
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if(!CursorHit.bBlockingHit) return;
	
	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	/*
	 * Handling All Cases w/ Enemy Mouseoevers
	 *  Handles:
	 *		Hovering a new actor, was not hovering one before (A)
	 *		Hovering a new actor, is the same as the prior actor (B)
	 *		Hovering a new actor, was hovering a different actor before (C)
	 *		Hovering nothing, was hovering an actor before (D)
	 *		Hovering nothing (E)
	 *		
	 */


	
	if(LastActor == nullptr) 
	{
		if(ThisActor != nullptr)  
		{
			ThisActor->HighlightActor();
		}
	}
	else
	{
		if(ThisActor == nullptr)
		{
			LastActor->UnhighlightActor();
		}
		else
		{
			if(LastActor != ThisActor)
			{
				LastActor->UnhighlightActor();
				ThisActor->HighlightActor();
			}
		}
	}
	
}
