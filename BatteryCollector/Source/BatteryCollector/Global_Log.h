// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define GET_CLASSNAME_WITH_FUNCTION		FString(__FUNCTION__)
#define GET_LINE_NUMBER					FString::FromInt(__LINE__)

// https://www.youtube.com/watch?v=TO-UdwqUt6M
#define LOG(LogCategory, Message)	UE_LOG(LogCategory, Log, TEXT("%s"), *FString(Message))
