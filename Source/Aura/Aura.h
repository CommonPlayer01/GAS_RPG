// CopyRight kang

#pragma once

#include "CoreMinimal.h"

#define CUSTOM_DEPTH_RED 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 251

#define ECC_PROJECTILE ECollisionChannel::ECC_GameTraceChannel1 //对投掷物响应的通道
#define ECC_TARGET_CHANNEL ECollisionChannel::ECC_GameTraceChannel2 //技能对攻击目标拾取的通道，只包含场景中的角色
#define ECC_EXCLUDEPLAYERS_CHANNEL ECollisionChannel::ECC_GameTraceChannel3 //技能范围选择时的通道，忽略场景中可动的角色



