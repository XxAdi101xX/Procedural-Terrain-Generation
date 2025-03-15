// Fill out your copyright notice in the Description page of Project Settings.


#include "MapDisplay.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AMapDisplay::AMapDisplay()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // Create the static mesh component
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
    RootComponent = StaticMeshComponent;

    // Load a plane static mesh (you can use any other static mesh you have)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
    if (PlaneMesh.Succeeded())
    {
        StaticMeshComponent->SetStaticMesh(PlaneMesh.Object);
    }

    // Load a basic material (assuming you have a material with a texture parameter)
    static ConstructorHelpers::FObjectFinder<UMaterial> MapDisplayMaterial(TEXT("/Game/Materials/M_MapDisplayMaterial.M_MapDisplayMaterial"));
    UE_LOG(LogTemp, Error, TEXT("looking for material!!"));
    if (MapDisplayMaterial.Succeeded())
    {
        UE_LOG(LogTemp, Error, TEXT("Material found!!!"));
        StaticMeshComponent->SetMaterial(0, MapDisplayMaterial.Object);
    }
}

// Called when the game starts or when spawned
void AMapDisplay::BeginPlay()
{
	Super::BeginPlay();
    
    // Create and apply the dynamic texture when the game starts
    CreateAndApplyDynamicTexture();
}

// Called every frame
void AMapDisplay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapDisplay::CreateAndApplyDynamicTexture()
{
    // Create a new texture
    FVector meshSize = StaticMeshComponent->GetStaticMesh()->GetBounds().GetBox().GetSize();
    UTexture2D *DynamicTexture = UTexture2D::CreateTransient(256, 256, PF_B8G8R8A8);
    if (!DynamicTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create dynamic texture"));
        return;
    }
    DynamicTexture->UpdateResource();

    // Initialize the texture with pixel data
    InitializeTexture(DynamicTexture);

    // Create a dynamic material instance
    UMaterialInterface *Material = StaticMeshComponent->GetMaterial(0);
    if (Material)
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
        if (DynamicMaterial)
        {
            // Set the texture parameter (assuming the parameter name is "DynamicTexture")
            DynamicMaterial->SetTextureParameterValue(FName("DynamicTexture"), DynamicTexture);

            // Apply the dynamic material to the static mesh
            StaticMeshComponent->SetMaterial(0, DynamicMaterial);
            UE_LOG(LogTemp, Warning, TEXT("set material"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create dynamic material instance"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No material found on the static mesh component"));
    }
}

void AMapDisplay::InitializeTexture(UTexture2D *Texture)
{
    // Lock the texture so we can write to it
    FTexture2DMipMap &Mip = Texture->GetPlatformData()->Mips[0];
    int pixVal = static_cast<int>(Texture->GetPlatformData()->PixelFormat);
    void *Data = Mip.BulkData.Lock(LOCK_READ_WRITE);

    // Define the color data (e.g., a gradient)
    uint8 *PixelData = static_cast<uint8 *>(Data);
    for (int32 y = 0; y < Texture->GetSizeY(); ++y)
    {
        for (int32 x = 0; x < Texture->GetSizeX(); ++x)
        {
            //int32 PixelIndex = ((y * Texture->GetSizeX()) + x) * 4;
            //PixelData[PixelIndex + 0] = x % 256; // B
            //PixelData[PixelIndex + 1] = y % 256; // G
            //PixelData[PixelIndex + 2] = (x + y) % 256; // R
            //PixelData[PixelIndex + 3] = 255; // A

            //FLinearColor Black = FLinearColor::Black;
            //FLinearColor White = FLinearColor::White;

            //// Linearly interpolate between black and white
            //FLinearColor ResultColor = FLinearColor::LerpUsingHSV(Black, White, 1.0f);

            // Calculate the pixel index (4 bytes per pixel)
            int32 PixelIndex = (y * Texture->GetSizeX() + x) * 4;

            // Create a gradient from black to white (modify this for different effects)
            // Here we're creating a horizontal gradient where red channel increases from left (0) to right (255)
            uint8 RedValue = static_cast<uint8>((float(x) / Texture->GetSizeX()) * 255.0f);
            uint8 GreenValue = static_cast<uint8>((float(y) / Texture->GetSizeY()) * 255.0f); // Optional vertical gradient
            uint8 BlueValue = 128; // Constant blue value (modify as needed)
            uint8 AlphaValue = 255; // Fully opaque

            // Set the color of the texel (RGBA)
            PixelData[PixelIndex] = RedValue;     // Red
            PixelData[PixelIndex + 1] = GreenValue; // Green (optional vertical gradient)
            PixelData[PixelIndex + 2] = BlueValue;  // Blue
            PixelData[PixelIndex + 3] = AlphaValue; // Alpha
        }
    }

    ATerrain *terrainActor = GetTerrainActor();
    TArray<TArray<float>> NoiseMap = terrainActor->GetNoiseMap();

    if (NoiseMap.Num() > 0) {
        UE_LOG(LogTemp, Warning, TEXT("init texture mat material %i, %i"), NoiseMap.Num(), NoiseMap[0].Num());
    } else {
        UE_LOG(LogTemp, Warning, TEXT("noise map empty!!"));
    }

    FColor *FormattedData = static_cast<FColor *>(Data);
    for (int32 x = 0; x < NoiseMap.Num(); ++x)
    {
        for (int32 y = 0; y < NoiseMap[0].Num(); ++y)
        {
            FLinearColor Black = FLinearColor::Black;
            FLinearColor White = FLinearColor::White;

            // Linearly interpolate between black and white
            FLinearColor ResultColor = FLinearColor::LerpUsingHSV(Black, White, (NoiseMap)[x][y]);
            FormattedData[y * Texture->GetSizeX() + x] = ResultColor.ToFColor(false);
            //FColor fColor = ResultColor.ToFColor(false);
            //// Set the color of the texel (RGBA)
            //int32 PixelIndex = (y * NoiseMap.Num() + x) * 4;
            //PixelData[PixelIndex] = fColor.R;     // Red
            //PixelData[PixelIndex + 1] = fColor.G; // Green (optional vertical gradient)
            //PixelData[PixelIndex + 2] = fColor.B;  // Blue
            //PixelData[PixelIndex + 3] = fColor.A; // Alpha
        }
    }

    // Unlock the texture
    Mip.BulkData.Unlock();

    // Update the texture
    Texture->UpdateResource();
}

ATerrain *AMapDisplay::GetTerrainActor()
{
    TSubclassOf<AActor> ActorClass = ATerrain::StaticClass();
    TArray<AActor *> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ActorClass, FoundActors);

    if (FoundActors.Num() > 0)
    {
        ATerrain *FirstTerrainInstance = Cast<ATerrain>(FoundActors[0]);
        return FirstTerrainInstance; // Return the first instance
    }

    return nullptr; // Return nullptr if no actors are found
}

