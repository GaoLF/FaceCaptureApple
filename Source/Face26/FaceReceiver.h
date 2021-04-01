// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "FaceBoy.h"

#include "FaceReceiver.generated.h"


DEFINE_LOG_CATEGORY_STATIC(FaceCapture, Log, All);

USTRUCT(BlueprintType)
struct FFrameData
{
	GENERATED_USTRUCT_BODY()

	float timestamp;
	int32 index;
	TArray<float> pose;
};

USTRUCT(BlueprintType)
struct FAnyCustomData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")
		FString Name = "Victory!";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")
		int32 Count = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")
		float Scale = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")
		FLinearColor Color = FLinearColor::Red;

	FAnyCustomData()
	{}

};

FORCEINLINE FArchive& operator<<(FArchive& Ar, FAnyCustomData& TheStruct)
{
	Ar << TheStruct.Name;
	Ar << TheStruct.Count;
	Ar << TheStruct.Scale;
	Ar << TheStruct.Color;

	return Ar;
};


UCLASS()
class FACE26_API AFaceReceiver : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFaceReceiver();


	UFUNCTION(BlueprintImplementableEvent)
		void BPEvent_DataReceived();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Receiver")
		int32 RecordFlag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Receiver")
		TArray<float> ReceivedData;
	
	int32 CurrentIndex;
	float TimeStamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Receiver")
		TArray<FFrameRecordData>WriteDatas;

	FCriticalSection m_mutex;
	FSocket* ListenSocket;
	FUdpSocketReceiver* UDPReceiver = nullptr;

	void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);

	UFUNCTION(BlueprintCallable, Category = "UDP")
		bool StartUDPReceiver(
			const FString& YourChosenSocketName,
			const FString& TheIP,
			const int32 ThePort
		);

	bool DataExport(FString FileName);

	UFUNCTION(BlueprintCallable, Category = "UDP")
		bool DataTestExport(float x, float head = 0.f);

	void ExportFile();

	uint8* PackData(int32& size);

	FORCEINLINE void ScreenMsg(const FString& Msg)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *Msg);
	}

	FORCEINLINE void ScreenMsg(const FString& Msg, const float Value)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s %f"), *Msg, Value));
	}
	FORCEINLINE void ScreenMsg(const FString& Msg, const FString Msg2)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s %s"), *Msg, *Msg2));
	}

	UFUNCTION(BlueprintCallable, Category = "UDP")
		TArray<float> GetReceivedData()
	{
		FScopeLock Lock(&m_mutex);
		{
			return ReceivedData;
		}
	}

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
