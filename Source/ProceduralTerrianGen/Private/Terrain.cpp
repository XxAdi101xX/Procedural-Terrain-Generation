// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
ATerrain::ATerrain()
{
 	// Disable the tick for every frame
	PrimaryActorTick.bCanEverTick = false;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
}

void ATerrain::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	Vertices.Reset();
	Triangles.Reset();
	UV_0.Reset();

	CreateVertices();
	CreateTriangles();

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV_0, Normals, Tangents);

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV_0, TArray<FColor>(), Tangents, true);
	ProceduralMesh->SetMaterial(0, Material);
}

// Called when the game starts or when spawned
void ATerrain::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATerrain::CreateVertices()
{
	// For a number of boxes, we need one more vertex hence the <= check
	for (int X = 0; X <= XSize; ++X)
	{
		for (int Y = 0; Y <= YSize; ++Y)
		{
			// Perlin noise returns the same value for whole numbers so we add 0.1
			float Z = FMath::PerlinNoise2D(FVector2D(X * NoiseScale + 0.1f, Y * NoiseScale + 0.1f)) * ZMultiplier;
			GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Yellow, FString::Printf(TEXT("Z %f"), Z));
			Vertices.Add(FVector(X * Scale, Y * Scale, Z));
			UV_0.Add(FVector2D(X * UVScale, Y * UVScale));

			// Debugging
			//DrawDebugSphere(GetWorld(), FVector(X * Scale, Y * Scale, 0 * Scale), 25.0f, 16, FColor::Red, true, -1.0f, 0U, 0.0f);
		}
	}
}

void ATerrain::CreateTriangles()
{
	int Vertex = 0;
	for (int X = 0; X < XSize; ++X)
	{
		for (int Y = 0; Y < YSize; ++Y, ++Vertex)
		{
			// First triangle of box
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);

			// Second triangle of box
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);
		}
		// We do this to avoid creating a triangle from the final vertices in the previous row with ones in the row above
		++Vertex;
	}
}

