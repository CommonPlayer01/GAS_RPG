// CopyRight kang


#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//设置Pt_0作为根节点
	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmutablePts.Add(Pt_0);
	SetRootComponent(Pt_0);

	//将其它的场景组件作为根组件的子节点添加，会在蓝图中去修改它的位置
	CreateSceneComponent(Pt_1, "Pt_1");
	CreateSceneComponent(Pt_2, "Pt_2");
	CreateSceneComponent(Pt_3, "Pt_3");
	CreateSceneComponent(Pt_4, "Pt_4");
	CreateSceneComponent(Pt_5, "Pt_5");
	CreateSceneComponent(Pt_6, "Pt_6");
	CreateSceneComponent(Pt_7, "Pt_7");
	CreateSceneComponent(Pt_8, "Pt_8");
	CreateSceneComponent(Pt_9, "Pt_9");
	CreateSceneComponent(Pt_10, "Pt_10");
}

/**
 * 获取并计算贴合地面的组件点位
 * @param GroundLocation 参考的基础位置（但在当前函数体中主要作为逻辑参考，实际计算多基于 Pt_0）
 * @param NumPoints 需要处理并返回的点位数量
 * @param YawOverride 额外的偏航角（Yaw）偏移量，用于旋转点阵布局
 */
TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints, float YawOverride)
{
    // 断言检查：确保预存的原始点数组（ImmutablePts）长度足够，防止索引溢出
    checkf(ImmutablePts.Num() >= NumPoints, TEXT("访问索引超过了数组的范围"));

    TArray<USceneComponent*> ArrayCopy;

    for(USceneComponent* Pt : ImmutablePts)
    {
       // 如果已经收集到了足够数量的点，直接返回结果
       if(ArrayCopy.Num() >= NumPoints) return ArrayCopy;

       // 处理非根节点（Pt_0 通常是集合的中心或起始参考点）
       if(Pt != Pt_0)
       {
          // 1. 计算当前点相对于根组件（Pt_0）的世界坐标偏移向量
          FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation(); 
          
          // 2. 绕 Z 轴（UpVector）旋转该偏移向量。这通常用于让整个点阵根据角色的朝向或输入进行整体旋转
          ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector); 
          
          // 3. 将旋转后的偏移应用回当前点，更新其世界位置
          Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint); 
       }

       // --- 准备射线检测（Line Trace）以寻找地面 ---

       // 创建射线检测的起点（当前点上方 500 单位）和终点（当前点下方 500 单位）
       const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z + 500.f);
       const FVector LoweredLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z - 500.f);

       // 4. 过滤干扰对象：获取半径 1500 内的所有存活玩家，防止射线检测撞到角色身上
       TArray<AActor*> IgnoreActors;
       UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(this, IgnoreActors, TArray<AActor*>(), 1500.f, GetActorLocation());

       // 5. 执行单线射线检测（Line Trace）
       FHitResult HitResult;
       FCollisionQueryParams QueryParams;
       QueryParams.AddIgnoredActors(IgnoreActors); // 忽略上面搜寻到的玩家
       
       // 使用 "BlockAll" 预设进行检测，寻找任何能阻挡射线的物理表面（通常是地面）
       GetWorld()->LineTraceSingleByChannel(HitResult, RaisedLocation, LoweredLocation, ECC_Visibility, QueryParams);

       // 6. 根据射线检测结果调整点的位置和旋转
       // 保持 X, Y 不变，将 Z 轴坐标修改为射线击中点（地面）的高度
       const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
       Pt->SetWorldLocation(AdjustedLocation);
       
       // 使组件的 Z 轴朝向地面的法线方向（让物体垂直于地面坡度）
       Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

       // 将处理好的点添加到返回数组中
       ArrayCopy.Add(Pt);
    }

    return ArrayCopy;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}

void APointCollection::CreateSceneComponent(TObjectPtr<USceneComponent>& Pt, FName Name)
{
	Pt = CreateDefaultSubobject<USceneComponent>(Name);
	ImmutablePts.Add(Pt);
	Pt->SetupAttachment(GetRootComponent());
}

