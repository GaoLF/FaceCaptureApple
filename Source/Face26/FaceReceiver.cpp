// Fill out your copyright notice in the Description page of Project Settings.


#include "FaceReceiver.h"
#include "Modules/ModuleManager.h"

//class FUDPReceiverModule : public IModuleInterface
//{
//public:
//	virtual void StartupModule() override;
//	virtual void ShutdownModule() override;
//};
//
//#define LOCTEXT_NAMESPACE "FZCLuaModule"
//
//void FUDPReceiverModule::StartupModule()
//{
//}
//
//void FUDPReceiverModule::ShutdownModule()
//{
//}

//IMPLEMENT_MODULE(FUDPReceiverModule, FACE26_API)

AFaceReceiver::AFaceReceiver()
{
	ListenSocket = NULL;
	PrimaryActorTick.bCanEverTick = true;
}

//AFaceReceiver::AFaceReceiver(const FObjectInitializer& ObjectInitializer)
//	:Super(ObjectInitializer)
//{
//	ListenSocket = NULL;
//	PrimaryActorTick.bCanEverTick = true;
//}

void AFaceReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UDPReceiver)
	{
		UDPReceiver->Stop();
		delete UDPReceiver;
		UDPReceiver = nullptr;

	}

	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}

}

bool AFaceReceiver::StartUDPReceiver(
	const FString& YourChosenSocketName,
	const FString& TheIP,
	const int32 ThePort
)
{
	ScreenMsg("RECEIVER INIT");

	RecordFlag = 0;
	FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);

	FIPv4Endpoint Endpoint(Addr, ThePort);

	int32 BufferSize = 2 * 1024 * 1024;

	ListenSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);
	;

	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(50);
	UDPReceiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP RECEIVER"));
	UDPReceiver->OnDataReceived().BindUObject(this, &AFaceReceiver::Recv);
	UDPReceiver->Start();

	return true;
}

void AFaceReceiver::Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	uint8 data[512];
	FMemory::Memzero(data, 512);
	FMemory::Memcpy(data, ArrayReaderPtr->GetData(), ArrayReaderPtr->Num());

	m_mutex.Lock();
	ReceivedData.Empty();
	int size = ArrayReaderPtr->Num();
	int32 index = 0;

	for (index = 0; index < size; index += 4)
	{
		float* temp = (float*)(data + index);
		ReceivedData.Push(*temp);
	}

	m_mutex.Unlock();

	float sendflag = ReceivedData[56];
	TimeStamp = ReceivedData[57];

	RecordFlag = (int32)sendflag;

	if (FMath::IsNearlyEqual(sendflag, 0.f))
		RecordFlag = 0;
	else if (FMath::IsNearlyEqual(sendflag, 1.f))
		RecordFlag = 1;
	else if (FMath::IsNearlyEqual(sendflag, 2.f))
		RecordFlag = 2;
	else if (FMath::IsNearlyEqual(sendflag, 3.f))
		RecordFlag = 3;

}

// Called when the game starts or when spawned
void AFaceReceiver::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFaceReceiver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RecordFlag == 0)
	{

	}
	else if (RecordFlag == 1)
	{
		CurrentIndex = 0;
		WriteDatas.Empty();
		FFrameRecordData temp;
		temp.FrameIndex = CurrentIndex;
		if (ReceivedData.Num() > 57)
			temp.TimeStamp = ReceivedData[57];

		m_mutex.Lock();

		temp.weights.Empty();

		for (int i = 0; i < ReceivedData.Num(); i++)
		{
			temp.weights.Add(ReceivedData[i]);
		}
		m_mutex.Unlock();
		WriteDatas.Add(temp);
	}
	else if (RecordFlag == 2)
	{
		CurrentIndex++;
		FFrameRecordData temp;
		temp.FrameIndex = CurrentIndex;
		if (ReceivedData.Num() > 57)
			temp.TimeStamp = ReceivedData[57];

		m_mutex.Lock();

		temp.weights.Empty();
		for (int i = 0; i < ReceivedData.Num(); i++)
		{
			temp.weights.Add(ReceivedData[i]);
		}
		m_mutex.Unlock();
		WriteDatas.Add(temp);

	}
	else if (RecordFlag == 3)
	{
		CurrentIndex++;
		FFrameRecordData temp;
		temp.FrameIndex = CurrentIndex;
		if (ReceivedData.Num() > 57)
			temp.TimeStamp = ReceivedData[57];
		m_mutex.Lock();
		UE_LOG(FaceCapture, Log, TEXT("SendFlag: 3 Size:%d"), ReceivedData.Num());
		temp.weights.Empty();
		for (int i = 0; i < ReceivedData.Num(); i++)
		{
			temp.weights.Add(ReceivedData[i]);
		}
		m_mutex.Unlock();
		WriteDatas.Add(temp);
		ExportFile();

		CurrentIndex = 0;
		WriteDatas.Empty();
	}
	
}



void AFaceReceiver::ExportFile()
{
	FDateTime date = FDateTime::Now();
	int32 Month = date.GetMonth();
	int32 Day = date.GetDay();
	int32 Hour = date.GetHour();
	int32 Minute = date.GetMinute();
	int32 Second = date.GetSecond();

	FString DefaultName = TEXT("");
	DefaultName += FString::Printf(TEXT("M%02dD%02d_%02d%02d%d"), Month, Day, Hour, Minute, Second);
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "gamedata/RecordFace";
	FString FileNames = Path + TEXT("/") + DefaultName + TEXT(".txt");

	DataExport(FileNames);
}

bool AFaceReceiver::DataExport(FString FileName)
{
	FArchive* Ar = (&IFileManager::Get())->CreateFileWriter(*FileName, 0x00);
	if (!Ar)
	{
		return false;
	}

	int size = 0;
	uint8* Ch = PackData(size);
	Ar->Serialize((void*)Ch, size * sizeof(uint8));

	delete Ar;
	return true;
}

bool AFaceReceiver::DataTestExport(float x, float head)
{
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "gamedata/RecordFace";

	FString FileName = Path + TEXT("/") + TEXT("TESTFACE.txt");
	WriteDatas.Empty();
	for (int i = 0; i < 1000; i++)
	{
		FFrameRecordData temp;
		temp.FrameIndex = i;
		temp.TimeStamp = 1.f / 20.f * (float)i;
		for (int j = 0; j < 52; j++)
		{
			temp.weights.Add(x * FMath::FRandRange(0.f, 1.f));
		}
		for (int j = 0; j < 4; j++)
			temp.weights.Add(x * FMath::FRandRange(0.f, 1.f));
		WriteDatas.Add(temp);
	}

	{
		for (int i = 0; i < 52; i++)
		{
			WriteDatas[0].weights[i] = 0.f;
		}
	}

	FArchive* Ar = (&IFileManager::Get())->CreateFileWriter(*FileName, 0x00);
	if (!Ar)
	{
		return false;
	}

	int size = 0;
	uint8* Ch = PackData(size);

	Ar->Serialize((void*)Ch, size * sizeof(uint8));
	FMemory::Free(Ch);
	delete Ar;
	return true;

}

uint8* AFaceReceiver::PackData(int32& size)
{
	uint8* Ch = (uint8*)FMemory::Malloc(WriteDatas.Num() * (52 + 4 * 4 + 5) + 1);

	UE_LOG(FaceCapture, Log, TEXT("Malloc:%d"), WriteDatas.Num() * (52 + 4 * 4 + 5) + 1);

	if (Ch == nullptr)
		return Ch;

	FFrameRecordData FirstFrameData;
	
	if (WriteDatas.Num() > 0)
		FirstFrameData = WriteDatas[0];

	UE_LOG(FaceCapture, Log, TEXT("Size of WriteDatas:%d"), WriteDatas.Num());

	for (int i = 0; i < WriteDatas.Num(); i++)
	{
		FFrameRecordData data = WriteDatas[i];
		FString LogText = FString::Printf(TEXT("%d: "), i);
		for (int p = 0; p < WriteDatas[i].weights.Num(); p++)
		{
			LogText += FString::Printf(TEXT("%f "), WriteDatas[i].weights[p]);
		}
		UE_LOG(FaceCapture, Log, TEXT("%s"), *LogText);



		FMemory::Memcpy(Ch + size++, (void*)&data.FrameIndex, 1);
		FMemory::Memcpy(Ch + size, (void*)&data.TimeStamp, 4);
		size += 4;

		for (int32 index = 0; index < FRAME_ARRAY_NUM; index++)
		{
			if (index <= 55 && index >= 52)
			{
				FMemory::Memcpy(Ch + size, (void*)&WriteDatas[i].weights[index], 4);
				size += 4;
			}
			else
			{
				float temp = (WriteDatas[i].weights[index] - FirstFrameData.weights[index]) / FMath::Abs(1.f - FirstFrameData.weights[index]);
				if (temp < 0.f)
					temp = 0.f;
				if (temp > 1.f)
					temp = 1.f;
				Ch[size++] = int8(temp * 100);
			}
		}
	}
	return Ch;
}