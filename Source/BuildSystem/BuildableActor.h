// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildableActor.generated.h"

UCLASS()
class BUILDSYSTEM_API ABuildableActor : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	ABuildableActor();
	// The static mesh for the structure, settable in a Blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buildable", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StructureMesh;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	// Whether the structure has been placed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildable")
	bool bIsPlaced;

public:
	// Places the structure
	UFUNCTION(BlueprintCallable, Category = "Buildable")
	void PlaceStructure();

	// Destroys the structure
	UFUNCTION(BlueprintCallable, Category = "Buildable")
	void DestroyStructure();
	UFUNCTION(BlueprintCallable, Category = "Buildable")
	UStaticMesh* GetMesh();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
