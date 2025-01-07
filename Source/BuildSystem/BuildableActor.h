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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = "Buildable")
	UStaticMeshComponent* StructureMesh;
	UPROPERTY(EditAnywhere, Category = "Buildable")
	bool bIsPlaced;
public:	
	UFUNCTION(BlueprintCallable, Category = "Buildable")
	void PlaceStructure();

	UFUNCTION(BlueprintCallable, Category = "Buildable")
	void DestroyStructure();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
