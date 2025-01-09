// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildableActor.h"

// Sets default values
ABuildableActor::ABuildableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create the static mesh component
	StructureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StructureMesh"));
	RootComponent = StructureMesh;

	bIsPlaced = false;
}

// Called when the game starts or when spawned
void ABuildableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildableActor::PlaceStructure()
{
	if (StructureMesh)
	{
		bIsPlaced = true;
		StructureMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		UE_LOG(LogTemp, Log, TEXT("Structure has been placed."));
	}
}

void ABuildableActor::DestroyStructure()
{
	Destroy();
	UE_LOG(LogTemp, Log, TEXT("Structure has been destroyed."));
}

UStaticMesh* ABuildableActor::GetMesh()
{
	return StructureMesh->GetStaticMesh();
}

// Called every frame
void ABuildableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

