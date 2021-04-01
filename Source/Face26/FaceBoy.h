// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FaceBoy.generated.h"

#define FRAME_ARRAY_NUM 56
#define FRAME_DATA_LENGTH (58 + 3)

USTRUCT(BlueprintType)
struct FFrameRecordData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 FrameIndex;
	float TimeStamp;
	TArray<float> weights;
};

UCLASS()
class FACE26_API AFaceBoy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFaceBoy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face")
		TArray<float> FirstFrameParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face")
		TArray<float> Params;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face")
		int32 BeginToShowInEditor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face")
		int32 BeginToShowInGame;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face")
		FRotator FaceRot;

	UPROPERTY()
		TMap<int, float> TongYongNvValue;

	FCriticalSection m_mutex;
	float TotalDuration;
	int32 TotalFrame;
	float LoadRecordTimeStep;
	float Duration;
	int32 CurrentFrameIndex;
	TArray<FFrameRecordData> Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Face")
		bool UseFileOrNot;


	UFUNCTION(BlueprintCallable, Category = "Face")
		void SetParam(const TArray<float>& Vec);

	UFUNCTION(BlueprintCallable, Category = "Face")
		void PlayFaceRecordFromFile(FString FimeName);

	UFUNCTION(BlueprintCallable, Category = "Face")
		bool LoadFaceRecorDataFromFile(FString FimeName);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
