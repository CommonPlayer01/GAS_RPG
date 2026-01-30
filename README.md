# 实现角色描边效果

在接口类EnemyInterface.h中，我们增加了两个虚函数，继承此接口的类需要实现高亮函数和取消高亮函数

```
class AURA_API IEnemyInterface
{
	GENERATED_BODY()
	// 向此类添加接口函数（虚函数），继承此接口的类都将需要继承并实现这些函数。
public:
	virtual void HighlightActor() = 0; //高亮
	virtual void UnHighlightActor() = 0; //取消高亮
};


```

敌人类继承接口类

```
UCLASS()
class AURA_API AEnemy : public ACharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	virtual void HighlightActor() override; //高亮
	virtual void UnHighlightActor() override; //取消高亮

	UPROPERTY(BlueprintReadOnly) //蓝图可读
	bool bHighlighted = false; //是否高亮
};

//cpp文件
void AEnemy::HighlightActor()
{
	bHighlighted = true;
}

void AEnemy::UnHighlightActor()
{
	bHighlighted = false;
}
```

在playercontroller中覆盖帧更新回调，鼠标位置追踪拾取函数，以及两个变量去记录当前鼠标拾取到的内容。

```
//帧更新
void APlayerControllerBase::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}
//鼠标位置追踪
void APlayerControllerBase::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit); //获取可视的鼠标命中结果
	if(!CursorHit.bBlockingHit) return; //如果未命中直接返回

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if(ThisActor != LastActor)
	{
		if(ThisActor) ThisActor->HighlightActor();
		if(LastActor) LastActor->UnHighlightActor();
	}
	
}

```

为了能够被鼠标拾取到，我们需要将敌人模型的检测相应可视设置为阻挡。

<figure><img src="./images/image (25).png" alt=""><figcaption></figcaption></figure>

使用后处理描边，在代码中实现，对自定义深度通道修改以及自定义深度模板值的修改，就可以实现当前的高亮效果。

```
AAuraEnemy::AEnemyBase()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); //设置可视为阻挡
}

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}
```

# ClassInfo

<figure><img src="./images/image (20).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (21).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (22).png" alt=""><figcaption></figcaption></figure>

# ClassInfo

<figure><img src="./images/image (20).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (21).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (22).png" alt=""><figcaption></figcaption></figure>

# 添加ASC和AS

带有ASC的Actor也被称为ASC的OwnerActor。ASC实际作用的Actor叫作AvatarActor。

OwnerActor和AvatarActor可以是同一个Actor，比如游戏中的野怪。

它们也可以是不同的 Actors，比如MOBA游戏中玩家和AI控制的英雄角色，OwnerActor是PlayerState、AvatarActor是HeroCharacter。大部分情况下OwnerActor和AvatarActor可以是角色Actor。不过想像一下你控制的英雄角色死亡然后重生的过程，如果此时要保留死亡前的Attributes或者GameplayEffects，那么最理想的做法是将ASC交给PlayerState。

如果OwnerActor和AvatarActor是不同的Actors，那么两者都需要实现IAbilitySystemInterface， 方便用于获取GAS的数据。这个接口只有一个方法需要被重载UAbilitySystemComponent\* GetAbilitySystemComponent() const，此方法将返回ASC。


首先实现在角色类里面添加，我们在类里面创建两个属性，分别用来定义ASC和AS，记得声明它们的类。主角类和敌人基础的类都会继承角色基础类，那么，它们都会创建相关的属性。

敌人的OwnerActor和AvatarActor是相同的，我们直接在敌人的基类的构造函数里面对ASC和AS实例化。

角色的直接在PlayerState构造函数里实例化

在playerState里，继承接口，实现接口的虚函数，用于获取ASC，并实现获取AS的函数。

<figure><img src="./images/image (26).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (27).png" alt=""><figcaption></figcaption></figure>

玩家的playerState和character里面存储的ASC和AS需要相同，我们可以在初始化玩家Character的时候，从PlayerState里获取ASC和AS

<figure><img src="./images/image (28).png" alt=""><figcaption></figcaption></figure>

### 初始化AbilitySysytemComponent

ASC需要有OwnerActor和AvatarActor进行初始化，而且必须在服务器和客户端都要完成初始化。

对于玩家控制的角色，ASC存在于Pawn(AvatarActor)中，我通常在Pawn的 PossessedBy()方法中完成ASC在服务器端的初始化，在PlayerController的AcknowledgePawn()方法中完成ASC在客户端的初始化。

对于玩家控制的角色，ASC存在于PlayerState中，我通常在Pawn 的PossessedBy() 方法中完成ASC在服务器端的初始化（这一点与上述相同），在 Pawn的 OnRep\_PlayerState()方法中完成ASC在客户端的初始化（这将确保PlayerState在客户端已存在）。

#### 初始化AI角色的GAS

对于AI控制的角色(即Enemy)，ASC存在于Pawn中，通常在Pawn的 BeginPlay()方法中完成ASC在服务器端和客户端的初始化。

既然根据文档知道了如何初始化，那么，我们按照相应的方式初始化。首先初始化敌人身上的ASC，敌人作为ai怪物，只需要在BeginPlay里面调用重写的InitAbilityActorInfo()函数即可。

<figure><img src="./images/image (34).png" alt=""><figcaption></figcaption></figure>

#### **初始化玩家角色GAS**



<figure><img src="./images/image (31).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (36).png" alt=""><figcaption></figcaption></figure>

# AttributeSet设置

创建一个继承自UAttributeSet的类

<figure><img src="./images/image (37).png" alt=""><figcaption></figcaption></figure>

宏的设置，编译时会默认给变量生成相应的Getter以及Setter函数，当前设置会生成四个函数，获取属性，获取值，设置值，以及初始化值。

<figure><img src="./images/image (38).png" alt=""><figcaption></figcaption></figure>

变量的OnRep 函数调用GAMEPLAYATTRIBUTE\_REPNOTIFY 宏才能使用预测系统

<figure><img src="./images/image (39).png" alt=""><figcaption></figcaption></figure>

Attribute 需要被添加到GetLifetimeReplicatedProps中，COND\_None 为触发没有条件限制，REPTNOTIFY\_Always 告诉 OnRep 方法在本地值和服务器下发的值即使已经相同也会触发（为了预测），默认情况下OnRep不会触发

# GE应用流程

<figure><img src="./images/image (16).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (15).png" alt=""><figcaption></figcaption></figure>

```cpp
void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
    //添加效果
    if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
       ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }

    if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
       ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }

    if(InfinityEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
       ApplyEffectToTarget(TargetActor, InfinityGameplayEffectClass);
    }

    //删除效果
    if(InfinityEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
    {
       UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
       if(!IsValid(TargetASC)) return;

       //创建存储需要移除的效果句柄存储Key，用于遍历完成后移除效果
       TArray<FActiveGameplayEffectHandle> HandlesToRemove;

       //循环map内存的数据
       for(TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
       {
          //判断是否ASC相同
          if(TargetASC == HandlePair.Value)
          {
             //通过句柄将效果移除，注意，有可能有多层效果，不能将其它层的效果也移除掉，所以只移除一层
             TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
             //添加到移除列表
             HandlesToRemove.Add(HandlePair.Key);
          }
       }
       //遍历完成后，在Map中将移除效果的KeyValue删除
       for(auto& Handle : HandlesToRemove)
       {
          ActiveEffectHandles.FindAndRemoveChecked(Handle);
       }
    }
}
```

#### 使用曲线表格设置回复血量值 <a href="#articlecontentid" id="articlecontentid"></a>

<figure><img src="./images/image (54).png" alt=""><figcaption></figcaption></figure>

接着打开GE，在Modifiers这里选择使用曲线表格

<figure><img src="./images/image (55).png" alt=""><figcaption></figcaption></figure>

选择使用哪一行数据

<figure><img src="./images/image (56).png" alt=""><figcaption></figcaption></figure>

### 修改应用的数值

创建的曲线表格对应着10个等级，现在还没有实现通过等级修改回血量。要实现这个功能，我们需要在之前类里面增加一个设置等级的属性

```
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
float ActorLevel = 1.f;
```

然后在创建GE实例这里，将等级设置上去

```
const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
```

<figure><img src="./images/image (57).png" alt=""><figcaption></figcaption></figure>

AbilityActorInfoSet()函数，这个函数用于初始化委托的注册。
\
然后添加一个委托触发的回调函数EffectApplied()这个函数将在GE被添加的时候触发。

<figure><img src="./images/image (58).png" alt=""><figcaption></figcaption></figure>

AbilityActorInfoSet()中先绑定了一个GE添加到自身的委托，事件触发时将调用EffectApplied()函数，EffectApplied()函数内，现在将获取到GE身上的AssetTags，并通过遍历的形式打印出来。

<figure><img src="./images/image (59).png" alt=""><figcaption></figcaption></figure>

ASC的基础类添加了，我们还需要对其进行调用，事件委托的绑定需要在ASC初始化完成后调用。

在所有的角色基类上面我们添加一个virtual void InitAbilityActorInfo();这个函数用于初始化ASC的委托注册，下面为初始化的代码

在角色基类上只声明一个虚函数，实现在英雄类和敌人类里面去实现。

在英雄类里面，在原来的基础上，初始化ASC后进行调用

<figure><img src="./images/image (60).png" alt=""><figcaption></figcaption></figure>

在敌人类里面，初始化委托方式一致

<figure><img src="./images/image (61).png" alt=""><figcaption></figcaption></figure>

在初始化完成后，我们在UE里面的GE添加对应的Tags

#### 使用委托将ASC和Widget Controller绑定

<figure><img src="./images/image (62).png" alt=""><figcaption></figcaption></figure>

EffectApplied是添加GE后的事件回调，之前是直接在这里打印的tags，现在修改成获取到TagContainer后，将TagContainer进行委托广播出去。

<figure><img src="./images/image (2).png" alt=""><figcaption></figcaption></figure>

Widget Controller的初始化委托监听的函数，前面的是之前监听AS属性值变化的代码，后面我们通过AddLambda添加了一个匿名函数，用于监听EffectAssetTags

```cpp
void UOverlayWidgetController::BindCallbacksToDependencies()
{
    Super::BindCallbacksToDependencies();
    const UAuraAttributeSet* AttributeSetBase = CastChecked<UAuraAttributeSet>(AttributeSet);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
       AttributeSetBase->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
       AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
       AttributeSetBase->GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
       AttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);

    //AddLambda 绑定匿名函数
    Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
       [this](const FGameplayTagContainer& AssetTags) //中括号添加this是为了保证内部能够获取类的对象
       {
          for(const FGameplayTag& Tag : AssetTags)
          {
             //对标签进行检测，如果不是信息标签，将无法进行广播
             FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
             // "A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
             if(Tag.MatchesTag(MessageTag))
             {
                FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
                MessageWidgetRowDelegate.Broadcast(*Row); //前面加*取消指针引用
             }
          }
       }
    );
}
```

#### 创建标签消息委托

实现在Widget里面通过委托获取GE的应用标签。

在WidgetController里面，创建一个结构体，这个结构体可以被蓝图使用

```cpp
USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
    GENERATED_BODY();
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag MessageTag = FGameplayTag();
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Message = FText();
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class UMyUserWidget> MessageWidget;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Image = nullptr;
};
```

编译后，创建一个Data Table，类就选择我们创建的结构体。

<figure><img src="./images/image (3).png" alt=""><figcaption></figcaption></figure>

在GameplayTag标签管理器里面，添加信息标签。

<figure><img src="./images/image (4).png" alt=""><figcaption></figcaption></figure>

在DataTable添加相应内容

<figure><img src="./images/image (5).png" alt=""><figcaption></figcaption></figure>

在WidgetController上面增加一个配置项，用于配置数据

```cpp
//EditDefaultsOnly 说明此属性可以通过属性窗口编辑，但只能在原型上进行。
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget Data")
TObjectPtr<UDataTable> MessageWidgetDataTable;
```

创建一个可以通过Tag在数据表格中寻找对应的数据的函数，为了兼容不同的数据表格，这里我们使用不确定的T类型，在调用时指定返回类型。

```cpp
template T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag) 
{ 
    T* Row = DataTable->FindRow(Tag.GetTagName(), TEXT("")); 
    return Row; 
}
```

在之前我们实现的匿名函数里面，首先添加的GE内是否包含Message的Tag，如果包含，则广播出去。

```
//AddLambda 绑定匿名函数
Cast<UAbilitySystemComponentBase>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
	[this](const FGameplayTagContainer& AssetTags) //中括号添加this是为了保证内部能够获取类的对象
	{
		for(const FGameplayTag& Tag : AssetTags)
		{
			//对标签进行检测，如果不是信息标签，将无法进行广播
			FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
			// "A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
			if(Tag.MatchesTag(MessageTag))
			{
				FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
				MessageWidgetRowDelegate.Broadcast(*Row); //前面加*取消指针引用
			}
		}
	}
);
```

在GE里面添加Tag

### 创建Message Widget

创建完成，我们可以将数据表格的数据填充上去对于的Widget

<figure><img src="./images/image (6).png" alt=""><figcaption></figcaption></figure>

使用数据创建一个widget，并设置显示在屏幕中间

给widget制作了一个新的动画，并添加到了设置函数上面，在设置完成属性后，播放动画

<figure><img src="./images/image (7).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image
# MVC UI架构

<figure><img src="./images/image (17).png" alt=""><figcaption></figcaption></figure>

#### UAuraUserWidget

在用户控件中，我们需要添加一个一个参数，用于获取存储控制器层对象。然后添加一个设置控制层对象的方法以及设置成功后的回调，方便后续操作。

添加一个可以在蓝图里面使用的回调，这个回调函数会在控制器对象被设置后调用。

增加一个设置控制器层的方法，可以在蓝图调用去修改控制器。

<figure><img src="./images/image (18).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (19).png" alt=""><figcaption></figcaption></figure>

EventPreConstruct相当于构造函数，在构造时，我们只需要通过变量修改尺寸框的高和宽，实现对每个添加的用户控件进行尺寸修改。

<figure><img src="./images/image (41).png" alt=""><figcaption></figcaption></figure>

### 添加到屏幕

创建一个新的用户控件，控件基类为创建的UAuraUserWidget 基类，这个控件会覆盖整个屏幕，作为ui的根。

### 创建UI控制器层

在UI控制器类里，增加了一个结构体，我们后续通过结构体设置控制器层的相关引用。结构体构造函数小括号后面加：可以直接对属性进行初始化。

<figure><img src="./images/image (42).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (43).png" alt=""><figcaption></figcaption></figure>

然后创建基于此控制层类创建一个子类，后续作为HUD的控制器层，它可以作为一个单例使用。

<figure><img src="./images/image (44).png" alt=""><figcaption></figcaption></figure>

### 创建HUD

<figure><img src="./images/image (45).png" alt=""><figcaption></figcaption></figure>

创建一个函数，可以通过结构体去获取控制器层实例。控制器层的实例化，我们需要在HUD里面做实例。实现就是OverlayWidgetController没有就创建，存在就直接返回。

<figure><img src="./images/image (46).png" alt=""><figcaption></figcaption></figure>

接着创建一个能够HUD初始化用户控件和控制器层对象的函数，它们的初始化以及创建我们在HUD里面去创建。如果没有设置对应的类，将抛出错误并给予提示，然后根据类创建对应的实例最后将用户控件添加到视口。

<figure><img src="./images/image (47).png" alt=""><figcaption></figcaption></figure>

由于这个函数需要PC,PS,ASC,AS这四个参数，所以我们需要在这四个值已经创建完成并初始化完成后的地方并且还能够获取HUD的一个对象里面调用。\
而ASC初始化完成，需要在客户端和服务器都要初始化结束，我们是在对应的Character里面实现的，所以，我们也需要在对应的Character里面调用即可。\
最终，我们选择了在玩家的Character里面，初始化完成ASC后，对HUD进行初始化。\
在玩家角色里我们书写了InitAbilityActorInfo()函数，这里面可以获得ASC和AS还有PlayerState，从角色类身上我们也可以获得玩家控制器，那么参数就齐了。

<figure><img src="./images/image (11).png" alt=""><figcaption></figcaption></figure>

#### &#xD;应用HUD

在UE里创建一个蓝图类，基类基于创建的c++的HUD

设置Overlay用户控件类和Overlay控制器层类

<figure><img src="./images/image (12).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (49).png" alt=""><figcaption></figcaption></figure>

打开基础游戏模式蓝图，设置HUD类为当前创建的蓝图HUD。

<figure><img src="./images/image (13).png" alt=""><figcaption></figcaption></figure>

### 使用委托设置初始值

由void AAuraCharacter::InitAbilityActorInfo()

调用void AAuraHUD::InitOverlay(APlayerController\* PC, APlayerState\* PS, UAbilitySystemComponent\* ASC, UAttributeSet\* AS)

再调用

<figure><img src="./images/image (14).png" alt=""><figcaption></figcaption></figure>

在进度条蓝图中绑定血条、蓝条变化事件的回调函数

### 监听数值的变化



使用AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate()来注册一个监听一个值的变化，如果监听的数值变化了接着将变化后的数值进行广播。

<figure><img src="./images/image (52).png" alt=""><figcaption></figcaption></figure>

在数值变化时，就会触发去调用Changed函数，我们只需要在changed函数内，将数值广播给蓝图即可。

<figure><img src="./images/image (53).png" alt=""><figcaption></figcaption></figure>



# 属性初始化方法

#### 使用DataTable初始化角色属性

这种方法主要用于调试，使用的是ASC内置的方法。

首先将ASC显示出来，在PlayerState里面设置在任何地方显示：

```cpp
UPROPERTY(VisibleAnywhere)
TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
```

选中ASC，在右侧会发现Attribute Test选项，我们可以添加多个配置项进行设置初始属性。

<figure><img src="./images/image (10).png" alt=""><figcaption></figcaption></figure>

### 使用GE进行初始化

在所有角色基类上面添加一个新的配置项，用于设置初始化属性使用的GE

```cpp
UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
```

创建一个基于GameplayEffect的蓝图,

属性设置指定对应属性，并将修改方式修改为覆盖

<figure><img src="./images/image (8).png" alt=""><figcaption></figcaption></figure>

设置完成，打开角色蓝图，找到属性面板，设置制作好的GE

<figure><img src="./images/image (9).png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image.png" alt=""><figcaption></figcaption></figure>

<figure><img src="./images/image (1).png" alt=""><figcaption></figcaption></figure>

# 基于MMC设置

建C++类，选择所有类，在里面找到GameplayModeMagnitudeCalculation类作为基类

覆盖父类的CalculateBaseMagnitude\_Implementation(const FGameplayEffectSpec& Spec)，这个函数返回了一个应用此MMC的GE的实例，我们可以通过Spec去获取需要的内容，然后计算数值。

创建一个FGameplayEffectAttributeCaptureDefinition 属性，我们会在cpp文件内，使用它去获取AS内的体力的值。

```cpp
UCLASS()
class AURA_API UMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()
public:
    UMMC_MaxHealth();
    
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
    
private:
    FGameplayEffectAttributeCaptureDefinition VigorDef;

};
```



```cpp
UMMC_MaxHealth::UMMC_MaxHealth()
{
    VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute(); //设置需要获取的属性对象
    VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target; //设置拾取对象为GE的应用目标
    VigorDef.bSnapshot = false;

    RelevantAttributesToCapture.Add(VigorDef); //添加到捕获属性数值，只有添加到列表，才会去获取属性值

}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    //  从 source 和 target 获取 Tag
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;

    //获取体力值
    float Vigor = 0.f;
    GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);
    Vigor = FMath::Max<float>(Vigor, 0.f);

    //获取等级
    ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
    const int32 Level = CombatInterface->GetPlayerLevel();

    //计算最大血量
    return 80.f + Vigor * 2.5f + Level * 10.f;
}
```

# 经验获取

>~~~cpp
>void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data){
>    ```
>    SendXPEvent(Props);
>    ```
>}
>~~~
>
>>```c
>>void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props){
>>    ```
>>    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, 
>>                                                         GameplayTags.Attributes_Meta_IncomingXP, 
>>                                                         Payload);
>>    ```
>>}
>>
>>```



# 

>~~~cpp
>void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data){
>    ```
>    IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
>    ```
>}
>~~~
>
>>```c
>>void AAuraPlayerState::SetLevel(int32 InLevel)
>>{
>>	Level = InLevel;
>>	OnLevelChangedDelegate.Broadcast(Level);
>>}
>>
>>```
>>
>>>
>>>
>>>

