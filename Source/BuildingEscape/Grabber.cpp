// Copyright Thomas Tran 2017

#include "BuildingEscape.h"
#include "Grabber.h"

#define OUT


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();
	SetupInputComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveObjectToGrabLocation();
}

void UGrabber::Grab()
{
	if (!PhysicsHandle)
		return;

	GetPlayerLocationRotation();
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();
	if (ActorHit)
		PhysicsHandle->GrabComponent(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), true);
}

void UGrabber::Release()
{
	if (!PhysicsHandle)
		return;

	PhysicsHandle->ReleaseComponent();
}

void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicsHandleComponent not found for %s!"), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (!InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InputComponent not found for %s"), *GetOwner()->GetName());
		return;
	}

	InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
	InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	FHitResult HitResult;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(OUT HitResult, PlayerViewPointLocation, GetEndReach(), FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParameters);
	AActor* ActorHit = HitResult.GetActor();

	return HitResult;
}


void UGrabber::MoveObjectToGrabLocation()
{
	if (!PhysicsHandle)
		return;

	if (PhysicsHandle->GrabbedComponent)
	{
		GetPlayerLocationRotation();
		PhysicsHandle->SetTargetLocation(GetEndReach());
	}
}

FVector UGrabber::GetEndReach() const
{
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	return LineTraceEnd;
}

void UGrabber::GetPlayerLocationRotation()
{
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);
}