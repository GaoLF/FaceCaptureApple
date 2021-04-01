// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG\Public\Blueprint\UserWidget.h"
#include "UMG\Public\Components/TextBlock.h"
#include "UMG\Public\Components/Slider.h"
#include "UMG\Public\Components/ScrollBox.h"
#include "UMG\Public\Components/EditableText.h"


#include "FaceUIController.generated.h"
/**
 * 
 */

const FString AnchorNames[52] = {
	TEXT("browDown_L"), TEXT("browDown_R"), TEXT("BrowInnerUp"), TEXT("BrowOuterUpLeft"), TEXT("BrowOuterUpRight"),
	TEXT("CheekPuff"), TEXT("CheekSquintLeft"), TEXT("CheekSuintRight"),

	TEXT("EyeBlink_L"), TEXT("EyeBlink_R"), TEXT("EyeLookDown_L"), TEXT("EyeLookDown_R"), TEXT("EyeLookIn_L"),
	TEXT("EyeLookIn_R"), TEXT("EyeLookOut_L"), TEXT("EyeLookOut_R"), TEXT("EyeLookUp_L"), TEXT("EyeLookUp_R"),
	TEXT("EyeSquint_L"), TEXT("EyeSquint_R"), TEXT("EyeWide_L"), TEXT("EyeWide_R"),

	TEXT("MouthClose"), TEXT("MouthDimple_L"), TEXT("MouthDimple_R"), TEXT("MouthFrown_L"), TEXT("MouthFrown_R"),
	TEXT("MouthFunnel"), TEXT("Mouth_L"), TEXT("MouthLowerDown_L"), TEXT("MouthLowerDown_R"), TEXT("MouthPress_L"),
	TEXT("MouthPress_R"), TEXT("MouthPucker"), TEXT("Mouth_R"),

	TEXT("MouthRollLower"), TEXT("MouthRollUpper"), TEXT("MouthShrugLower"), TEXT("MouthShrugUpper"), TEXT("MouthSmile_L"),
	TEXT("MouthSmile_R"), TEXT("Smile_L"), TEXT("Smile_R"), TEXT("MouthStretch_L"), TEXT("MouthStretch_R"),
	TEXT("MouthUpperUp_L"), TEXT("MouthUpperUp_R"),
	TEXT("NoseSneer_L"), TEXT("NoseSneer_R"),
};

UCLASS()
class FACE26_API UFaceUISlider : public UUserWidget
{
	GENERATED_BODY()
public:
	//UFaceUISlider();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* PoseName;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		USlider* Sliders;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UEditableText* Value;

	void SetValue();

	void SetPoseName(FString Name);

	//~FaceUISlider();
};

UCLASS()
class FACE26_API UFaceUIController : public UUserWidget
{
	GENERATED_BODY()
public:
	//UFaceUIController();
	//~FaceUIController();
	
	//UPROPERTY(BlueprintReadWrite)
	//	UFaceUISlider* PoseSlider1;

	//UPROPERTY(BlueprintReadWrite)
	//	UFaceUISlider* PoseSlider2;

	//UPROPERTY(BlueprintReadWrite)
	//	UFaceUISlider* PoseSlider3;

	//UPROPERTY(BlueprintReadWrite)
	//	UFaceUISlider* PoseSlider4;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UScrollBox* PoseScroll;


};

