// Copyright Thomas Tran 2017

#include "BuildingEscape.h"
#include "OpenDoor.h"

#define OUT


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	CheckPressurePlate();
	Owner = GetOwner();
}

void UOpenDoor::OpenDoor()
{
	Owner->SetActorRotation(FRotator(0.0f, OpenAngle, 0.0f));
}

void UOpenDoor::CloseDoor()
{
	FRotator NewRotation = FRotator(0.0f, 0.0f, 0.0f);

	Owner->SetActorRotation(NewRotation);
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TotalMass = 0.0f;

	if (GetTotalMassOfActorsOnPlate() >= 50.0f)
	{
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	if (GetWorld()->GetTimeSeconds() - LastDoorOpenTime >= DoorCloseDelay)
		CloseDoor();
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	TArray<AActor*> OverlappingActors;

	if (!PressurePlate)
		return 0.0f;

	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for (const auto* CurrentActor : OverlappingActors)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CurrentActor: %s"), *CurrentActor->GetName());
		TotalMass += CurrentActor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
}

void UOpenDoor::CheckPressurePlate()
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("Reference to PressurePlate not found on %s"), *GetOwner()->GetName());
	}
}