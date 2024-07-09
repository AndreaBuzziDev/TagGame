// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabbableObject.h"

// Sets default values
AGrabbableObject::AGrabbableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Create a static mesh in order to have a position ref in world
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjMesh"));
	RootComponent = BallMesh;
}

// Called when the game starts or when spawned
void AGrabbableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrabbableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}