// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Terrain.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;

UCLASS()
class PROCEDURALTERRIANGEN_API ATerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrain();

	// Squares along the X axis
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int XSize = 50;
	// Squares along the y axis
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int YSize = 50;
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))

	// Controls vertices height scaling
	float ZMultiplier = 100.0f;

	// Perlin noise scaling
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	float NoiseScale = 0.1f;

	// Squares along the y axis
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.00001))
	float Scale = 100.0f;
	// Squares along the y axis
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.00001))
	float UVScale = 1.0f;
protected:
	// Called when an instance of this class is placed (in editor) or spawned.
	virtual void OnConstruction(const FTransform &Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UMaterialInterface *Material;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UProceduralMeshComponent *ProceduralMesh;
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector2D> UV_0;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;

	void CreateVertices();
	void CreateTriangles();
};
