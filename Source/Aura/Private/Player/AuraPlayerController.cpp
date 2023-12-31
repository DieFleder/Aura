// Copyright Emergent Engineering LLC


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
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if(!CursorHit.bBlockingHit){return;}

	LastActor = CurrentActor;
	CurrentActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	
	/**
	 *Line trace from cursor. Several Scenarios:
	 *A. LastActor is null && CurrentActor is null
	 *	- Do nothing.
	 *B. LastActor is null && CurrentActor is valid
	 *	- Highlight CurrentActor
	 *C. LastActor is Valid && CurrentActor is null
	 *	- UnHighlight LastActor
	 *D. Both are valid but LastActor != CurrentActor
	 *	- UnHighlight LastActor and Highlight CurrentActor
	 *E. Both are valid and LastActor == CurrentActor
	 *	- do nothing
	 **/
	if(LastActor == nullptr)
	{
		if(CurrentActor != nullptr)
		{
			// Case B.
			CurrentActor->HighlightActor();
		}
		else
		{
			//Case A. Do nothing
		}
	}
	else // LastActor is valid
	{
		if(CurrentActor == nullptr)
		{
			// Case C
			LastActor->UnHighlightActor();
		}
		else //Both are valid
		{
			if(LastActor != CurrentActor)
			{
				// Case D.
				LastActor->UnHighlightActor();
				CurrentActor->HighlightActor();
			}
			else // both are valid but not the same actor
			{
				// Case E. do nothing
			}
		}
	}
	
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	
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
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

