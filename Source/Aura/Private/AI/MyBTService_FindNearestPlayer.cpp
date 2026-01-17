// CopyRight kang


#include "AI/MyBTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UMyBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = AIOwner->GetPawn();
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);

	for(AActor* Actor : ActorsWithTag)
	{
		GEngine->AddOnScreenDebugMessage(2, .5f, FColor::Orange, *Actor->GetName());
	}


	//遍历所有的角色获取到最近的角色和距离
	float ClosestDistance = TNumericLimits<float>::Max(); //默认设置float最大值
	AActor* ClosestActor = nullptr;
	for(AActor* Actor : ActorsWithTag)
	{
		const float Distance = OwningPawn->GetDistanceTo(Actor);
		if(Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestActor = Actor;
		}
	}

	//设置黑板数据
	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);


}
