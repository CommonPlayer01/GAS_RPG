// CopyRight kang


#include "Actor/AuraEnemySpawnPoint.h"

#include "Character/AuraEnemy.h"

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	//延迟生成Actor，并设置其尝试调整位置但固定生成
	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(
		EnemyClass,
		GetActorTransform(),
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	
	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}
