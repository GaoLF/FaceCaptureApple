// Fill out your copyright notice in the Description page of Project Settings.


#include "FaceBoy.h"
#include "Core/Public/HAL/FileManager.h"
#define TIMESTEP (1.f / 60.f)



// Sets default values
AFaceBoy::AFaceBoy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


// Called when the game starts or when spawned
void AFaceBoy::BeginPlay()
{
	Super::BeginPlay();

	BeginToShowInGame = 0;
	UseFileOrNot = false;

	TongYongNvValue.Empty();
	TongYongNvValue.Add(0, 0.75f);
	TongYongNvValue.Add(1, 0.75f);
	TongYongNvValue.Add(2, 0.5f);
	TongYongNvValue.Add(3, 0.5f);
	TongYongNvValue.Add(4, 0.5f);

	TongYongNvValue.Add(8, 1.3f);
	TongYongNvValue.Add(9, 1.3f);

	TongYongNvValue.Add(18, 0.2f);
	TongYongNvValue.Add(19, 0.2f);
	TongYongNvValue.Add(24, 0.5f);

	TongYongNvValue.Add(29, 0.8f);
	TongYongNvValue.Add(30, 0.8f);
	TongYongNvValue.Add(33, 0.f);
	TongYongNvValue.Add(34, 0.f);

	TongYongNvValue.Add(39, 0.8f);
	TongYongNvValue.Add(40, 0.8f);
	TongYongNvValue.Add(42, 0.8f);
	TongYongNvValue.Add(43, 0.8f);
	TongYongNvValue.Add(44, 0.8f);
	TongYongNvValue.Add(45, 0.8f);
	TongYongNvValue.Add(46, 0.8f);
} 

// Called every frame
void AFaceBoy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (!ensure(!FMath::IsNearlyZero(LoadRecordTimeStep)))
	//{
	//	return;
	//}

	if (UseFileOrNot == true)
	{
		if (BeginToShowInGame)
		{
			Duration += DeltaTime;
			int currentframe = 0;
			if (LoadRecordTimeStep > 0)
				currentframe = Duration / LoadRecordTimeStep;
			else
				currentframe = Duration / TIMESTEP;

			if ((currentframe + 1) < TotalFrame)
			{
				for (int i = 0; i < Data[currentframe].weights.Num(); i++)
				{
					float temp = Data[currentframe].weights[i];
					if (i < Params.Num())
					{

						if (Params[i] - temp > 0.03f)
							Params[i] -= 0.03;
						else if (Params[i] - temp < -0.03)
							Params[i] += 0.03;
						else
							Params[i] = temp;
					}
					else
						Params.Add(temp);

				}
			}
			else
			{
				BeginToShowInGame = false;
				m_mutex.Lock();
				Params.Empty();
				for (int32 index = 0; index < 52; index++)
				{
					Params.Add(0.f);
				}
				m_mutex.Unlock();
				return;
			}

			FaceRot = FRotator(0.f, 0.f, 0.f);
			if (Params.Num() >= 56)
			{
				float x = Params[52];
				float y = Params[53];
				float z = Params[54];
				float w = Params[55];

				FQuat quat(x, y, z, w);
				FaceRot = FRotator(quat);
			}

			if (CurrentFrameIndex >= TotalFrame)
			{
				BeginToShowInGame = false;
			}
		}
	}
}

// Called to bind functionality to input
void AFaceBoy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFaceBoy::PlayFaceRecordFromFile(FString FileName)
{
	if (LoadFaceRecorDataFromFile(FileName))
	{
		CurrentFrameIndex = 0;
		UseFileOrNot = true;
		BeginToShowInGame = true;
		Duration = 0.f;
		if (TotalFrame > 0)
			LoadRecordTimeStep = TotalDuration / TotalFrame;
	}
}
bool AFaceBoy::LoadFaceRecorDataFromFile(FString FileName)
{
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "gamedata/RecordFace";
	FString PathName = Path + TEXT("/") + FileName;
	FScopedLoadingState ScopedLoadingState(*PathName);

	FArchive* Reader = IFileManager::Get().CreateFileReader(*PathName, 0x00);

	if (!Reader)
		return false;

	int32 Size = Reader->TotalSize();

	if (!Size)
		return false;

	uint8* Ch = (uint8*)FMemory::Malloc(Size);
	uint8* Head = Ch;
	Reader->Serialize(Ch, Size);

	int32 framenum = Size / FRAME_DATA_LENGTH;
	TotalDuration = 0.f;
	TotalFrame = framenum;

	m_mutex.Lock();
	Data.Empty();

	int32 i = *(Ch);
	for (;i < framenum; i++)
	{
		FFrameRecordData data;

		int32 index = *(Ch++);
		if ((i % 256) != index)
		{
			bool SUccess = Reader->Close();
			delete Reader;
			FMemory::Free(Head);
			framenum = i;
			TotalFrame = framenum;
			return true;
		}
		data.FrameIndex = i;
		float timestamp = *(float*)Ch;
		Ch += 4;
		data.TimeStamp = timestamp;
		TotalDuration = FMath::Max(timestamp, TotalDuration);
		for (int weightindex = 0; weightindex < (FRAME_ARRAY_NUM); weightindex++)
		{
			float weight = 0.f;
			if (weightindex < 52)
				weight = (float)(*(Ch++)) / 100.f;
			else
			{
				weight = (float)(*(float*)Ch);
				Ch += 4;
			}

			data.weights.Add(weight);

		}
		Data.Add(data);
	}
	m_mutex.Unlock();

	bool Success = Reader->Close();
	delete Reader;
	FMemory::Free(Head);

	return Success;
}


void AFaceBoy::SetParam(const TArray<float>& Vec)
{
	if (BeginToShowInGame == 1)
	{
		m_mutex.Lock();
		FirstFrameParams.Empty();
		for (int i = 0; i < Vec.Num(); i++)
		{
			FirstFrameParams.Add(Vec[i]);
		}
		BeginToShowInGame = 2;

		Params.Empty();
		for (int32 index = 0; index < 56; index++)
			Params.Add(0.f);
		m_mutex.Unlock();

		return;
	}
	else if (BeginToShowInGame == 2)
	{
		m_mutex.Lock();

		Params.Empty();
		for (int i = 0; i < Vec.Num(); i++)
		{
			Params.Add(Vec[i]);
		}

		if (Params.Num() >= 56)
		{
			for (int i = 0; i < 52; i++)
			{
				if (i < Params.Num() && i < FirstFrameParams.Num())
				{
					if (FMath::IsNearlyZero(FMath::Abs(1.f - FirstFrameParams[i])))
					{
						Params[i] = 0.f;
					}
					else
					{
						Params[i] = (Params[i] - FirstFrameParams[i]) / FMath::Abs(1.f - FirstFrameParams[i]);

						if (Params[i] > 1.f)
							Params[i] = 1.f;
						if (Params[i] < 0.f)
							Params[i] = 0.f;
					}
				}
			}
		}
	}
}