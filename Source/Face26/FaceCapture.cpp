// Fill out your copyright notice in the Description page of Project Settings.


#include "FaceCapture.h"

// Sets default values
AFaceCapture::AFaceCapture()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFaceCapture::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFaceCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFaceCapture::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

