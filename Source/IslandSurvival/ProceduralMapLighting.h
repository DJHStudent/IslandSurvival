// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * //calculate the normals and tangents for a given vector
 */
class ISLANDSURVIVAL_API ProceduralMapLighting
{
public:
	ProceduralMapLighting();
	~ProceduralMapLighting();

private:
	TArray<FVector> TriangleNormals; //the normal for each triangles face
};
