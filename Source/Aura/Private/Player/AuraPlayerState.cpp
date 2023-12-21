// Copyright Emergent Engineering LLC


#include "Player/AuraPlayerState.h"

AAuraPlayerState::AAuraPlayerState()
{
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
