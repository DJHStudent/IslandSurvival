// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class State : uint8
{
	IDLE,
	CHASE
};
/**
 * 
 */
class ISLANDSURVIVAL_API ZombieState
{
public:
	ZombieState();
	~ZombieState();
};
