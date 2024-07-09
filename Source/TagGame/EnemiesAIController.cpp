// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemiesAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "TagGameGameMode.h"

void AEnemiesAIController::BeginPlay()
{
	Super::BeginPlay();

	GoToPlayer = MakeShared<FAivState>(
		[](AAIController* AIController) {
			AIController->MoveToActor(AIController->GetWorld()->GetFirstPlayerController()->GetPawn(), 100.0f);
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {
			EPathFollowingStatus::Type Status = AIController->GetMoveStatus();

			if (Status == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}

			if (BestBall)
			{
				BestBall->AttachToActor(AIController->GetWorld()->GetFirstPlayerController()->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
				BestBall->SetActorRelativeLocation(FVector::Zero());

				AGameModeBase* GameModeBase = AIController->GetWorld()->GetAuthGameMode();
				ATagGameGameMode* TagGameMode = Cast<ATagGameGameMode>(GameModeBase);
				TagGameMode->CheckForEndGame();

				BestBall = nullptr;
			}

			return SearchFreeBall;
		}
	);

	SearchFreeBall = MakeShared<FAivState>(
		[this](AAIController* AIController) {
			AGameModeBase* GameModeBase = AIController->GetWorld()->GetAuthGameMode();
			ATagGameGameMode* TagGameMode = Cast<ATagGameGameMode>(GameModeBase);
			const TArray<AGrabbableObject*>& BallsList = TagGameMode->GetBalls();

			AGrabbableObject* NearestBall = nullptr;

			for (int32 i = 0; i < BallsList.Num(); i++)
			{
				if (!BallsList[i]->GetAttachParentActor() &&
					(!NearestBall || 
						FVector::Distance(AIController->GetPawn()->GetActorLocation(), BallsList[i]->GetActorLocation()) < 
						FVector::Distance(AIController->GetPawn()->GetActorLocation(), NearestBall->GetActorLocation())))
				{
					NearestBall = BallsList[i];
				}
			}

			BestBall = NearestBall;
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> {

			if (BestBall)
			{
				return GoToBall;
			}

			SearchFreeBall->CallEnter(this);
			return SearchFreeBall;
		}
	);

	GoToBall = MakeShared<FAivState>(
		[this](AAIController* AIController) 
		{
			AIController->MoveToActor(BestBall, 100.0f);
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> 
		{
			EPathFollowingStatus::Type Status = AIController->GetMoveStatus();

			if (Status == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}

			return TryToGrabBall;
		}
	);

	TryToGrabBall = MakeShared<FAivState>(
		[this](AAIController* AIController) 
		{
			if (BestBall->GetAttachParentActor())
			{
				BestBall = nullptr;
			}
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAivState> 
		{
			if (!BestBall)
			{
				return SearchFreeBall;
			}

			BestBall->AttachToActor(AIController->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
			BestBall->SetActorRelativeLocation(FVector::Zero());

			return GoToPlayer;
		}
	);

	CurrentState = SearchFreeBall;
	CurrentState->CallEnter(this);
}

void AEnemiesAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState)
	{
		CurrentState = CurrentState->CallTick(this, DeltaTime);
	}
}