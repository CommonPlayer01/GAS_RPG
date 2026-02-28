// CopyRight kang


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"

// Sets default values
AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(GetRootComponent());

}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
	
}


