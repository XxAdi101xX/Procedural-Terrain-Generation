// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terrain.h"

#include "MapDisplay.generated.h"

UCLASS()
class PROCEDURALTERRIANGEN_API AMapDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapDisplay();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Static mesh component for the plane
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *StaticMeshComponent;

	// Material instance dynamic
	UMaterialInstanceDynamic *DynamicMaterial;

	// Method to create and apply a dynamic texture
	void CreateAndApplyDynamicTexture();

	// Method to initialize the texture with pixel data
	void InitializeTexture(UTexture2D *Texture);

	// Get the terrain that we want to display; we assume there is only one
	ATerrain *GetTerrainActor();
};
