// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GrabbableObject.h"
#include "EnemiesAIController.generated.h"

struct FAivState : public TSharedFromThis<FAivState>
{
private:
	TFunction<void(AAIController*)> Enter;
	TFunction<void(AAIController*)> Exit;
	TFunction<TSharedPtr<FAivState>(AAIController*, const float)> Tick;

public:
	FAivState()
	{
		Enter = nullptr;
		Exit = nullptr;
		Tick = nullptr;
	}

	FAivState(TFunction<void(AAIController*)> InEnter = nullptr, TFunction<void(AAIController*)> InExit = nullptr, TFunction<TSharedPtr<FAivState>(AAIController*, const float)> InTick = nullptr)
	{
		Enter = InEnter;
		Exit = InExit;
		Tick = InTick;
	}

	FAivState(const FAivState& Other) = delete;
	FAivState& operator=(const FAivState& Other) = delete;
	FAivState(FAivState&& Other) = delete;
	FAivState& operator=(FAivState&& Other) = delete;

	void CallEnter(AAIController* AIController)
	{
		if (Enter)
		{
			Enter(AIController);
		}
	}

	void CallExit(AAIController* AIController) 
	{
		if (Exit)
		{
			Exit(AIController);
		}
	}

	TSharedPtr<FAivState> CallTick(AAIController* AIController, const float DeltaTime)
	{
		if (Tick)
		{
			TSharedPtr<FAivState> NewState = Tick(AIController, DeltaTime);

			if (NewState != nullptr && NewState != AsShared())
			{
				CallExit(AIController);
				NewState->CallEnter(AIController);
				return NewState;
			}
		}

		return AsShared();
	}
};

/**
 * 
 */
UCLASS()
class TAGGAME_API AEnemiesAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	TSharedPtr<FAivState> CurrentState;
	TSharedPtr<FAivState> GoToPlayer;
	TSharedPtr<FAivState> GoToBall;
	TSharedPtr<FAivState> TryToGrabBall;
	TSharedPtr<FAivState> SearchFreeBall;

	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	AGrabbableObject* BestBall;
};