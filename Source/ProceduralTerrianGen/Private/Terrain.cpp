// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetMathLibrary.h"

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

	// Important note: Terrain types should be added from lowest to highest heights
	TerrainTypes.Add(new TerrainType("Water", 120.0f, FColor(15.0f, 94.0f, 156.0f, 1.0f)));
	TerrainTypes.Add(new TerrainType("Grass", 300.0f, FColor(0.0f, 76.0f, 0.0f, 1.0f)));
	TerrainTypes.Add(new TerrainType("Snow", TNumericLimits<float>::Max(), FColor(255.0f, 255.0f, 255.0f, 1.0f))); // Default

	Vertices.Reset();
	Triangles.Reset();
	UV_0.Reset();
	VertexColors.Reset();

	CreateNoiseMap();
	CreateVertices();
	CreateTriangles();
	CreateVertexColors();

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV_0, Normals, Tangents);

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV_0, VertexColors, Tangents, true);
	//Material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/StarterContent/Materials/M_Basic_Floor.M_Basic_Floor"));
	//Material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/CustomBaseMaterial.CustomBaseMaterial"));
	ProceduralMesh->SetMaterial(0, Material);
	UE_LOG(LogTemp, Warning, TEXT("vertex and color size is %d %d"), Vertices.Num(), VertexColors.Num());
}

// Called when the game starts or when spawned
void ATerrain::BeginPlay()
{
	Super::BeginPlay();
	// GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Yellow, FString::Printf(TEXT("Z %f"), 3.0));
}

// Called every frame
void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATerrain::CreateNoiseMap()
{
	// Create a random stream object
	FRandomStream RandomStream;

	// Initialize the random stream with a seed value
	RandomStream.Initialize(Seed);

	TArray<FVector2D> LayerOffsets;
	LayerOffsets.Reserve(NoiseLayers);
	for (int i = 0; i < NoiseLayers; ++i)
	{
		float OffsetX = RandomStream.FRandRange(-100000.0f, 100000.0f);
		float OffsetY = RandomStream.FRandRange(-100000.0f, 100000.0f);
		LayerOffsets.Add(FVector2D(OffsetX, OffsetY));
	}

	float MaxNoiseHeight = TNumericLimits<float>::Min();
	float MinNoiseHeight = TNumericLimits<float>::Max();

	// If we don't init NoiseMap here
	NoiseMap.Init(TArray<float>(), MapX + 1);

	//UE_LOG(LogTemp, Warning, TEXT("The !!!noisemap size is %i"), NoiseMap.Num());

	for (int X = 0; X <= MapX; ++X)
	{
		for (int Y = 0; Y <= MapY; ++Y)
		{
			float Amplitude = 1.0f;
			float Frequency = 1.0f;
			float NoiseZ = 0.0f;

			for (int i = 0; i < NoiseLayers; ++i)
			{
				float SampleX = X * NoiseScale * Frequency + LayerOffsets[i].X;
				float SampleY = Y * NoiseScale * Frequency + LayerOffsets[i].Y;

				float PerlinSample = FMath::PerlinNoise2D(FVector2D(SampleX, SampleY));
				NoiseZ += PerlinSample * Amplitude;

				Amplitude *= Gain;
				Frequency *= Lacunarity;
			}

			if (NoiseZ > MaxNoiseHeight)
			{
				MaxNoiseHeight = NoiseZ;
			}
			else if (NoiseZ < MinNoiseHeight)
			{
				MinNoiseHeight = NoiseZ;
			}
			NoiseMap[X].Add(NoiseZ);
			//UE_LOG(LogTemp, Warning, TEXT("@@@The ZNoise is %f"), NoiseMap[X][Y]);
		}
	}

	for (int X = 0; X <= MapX; ++X)
	{
		for (int Y = 0; Y <= MapY; ++Y)
		{
			//UE_LOG(LogTemp, Warning, TEXT("The !!!noisemap value is %f %i %i"), NoiseMap[X][Y], NoiseMap.Num(), NoiseMap[0].Num());
			NoiseMap[X][Y] = UKismetMathLibrary::InverseLerp(MinNoiseHeight, MaxNoiseHeight, NoiseMap[X][Y]);
		}
	}
}

void ATerrain::CreateVertices()
{
	// For a number of boxes, we need one more vertex hence the <= check
	for (int X = 0; X <= MapX; ++X)
	{
		for (int Y = 0; Y <= MapY; ++Y)
		{
			// Perlin noise returns the same value for whole numbers so we add 0.1
			//float Offset = 0.1f;
			//float SampleX = X * NoiseScale + Offset;
			//float SampleY = Y * NoiseScale + Offset;
			//float Z = FMath::PerlinNoise2D(FVector2D(SampleX, SampleY)) * ZScale;
			float Z = NoiseMap[X][Y] * ZScale;

			Vertices.Add(FVector(X * Scale, Y * Scale, Z));
			UV_0.Add(FVector2D(X * UVScale, Y * UVScale));
		}
	}
}

void ATerrain::CreateTriangles()
{
	int Vertex = 0;
	for (int X = 0; X < MapX; ++X)
	{
		for (int Y = 0; Y < MapY; ++Y, ++Vertex)
		{
			// First triangle of box
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + MapY + 1);

			// Second triangle of box
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + MapY + 2);
			Triangles.Add(Vertex + MapY + 1);
		}
		// We do this to avoid creating a triangle from the final vertices in the previous row with ones in the row above
		++Vertex;
	}
}

void ATerrain::CreateVertexColors()
{
	FlushPersistentDebugLines(GetWorld()); // TODO remove this
	for (int VertexIndex = 0; VertexIndex < Vertices.Num(); ++VertexIndex)
	{
		float Z = Vertices[VertexIndex].Z;
		for (int TerrainIndex = 0; TerrainIndex < TerrainTypes.Num(); ++TerrainIndex)
		{
			if (Z < TerrainTypes[TerrainIndex]->Height)
			{
				VertexColors.Add(TerrainTypes[TerrainIndex]->Color);
				// Debugging
				//DrawDebugSphere(GetWorld(), FVector(Vertices[VertexIndex].X, Vertices[VertexIndex].Y, 0), 25.0f, 16, TerrainTypes[TerrainIndex]->Color, true, -1.0f, 0U, 0.0f);
				break;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Vertex color size %d"), VertexColors.Num());
}


TerrainType::TerrainType(FString Name, float Height, FColor Color): Name{Name}, Height{Height}, Color{Color}
{
}

