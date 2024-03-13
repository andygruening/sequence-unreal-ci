#pragma once

#include "Authenticator.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif // PLATFORM_ANDROID

namespace NativeOAuth
{
#if PLATFORM_ANDROID
void AndroidThunkCpp_SignInWithGoogle(const FString& clientId, const FString& nonce);
jstring ConvertToJavaString(JNIEnv* jenv, const FString& string);
void AndroidLog(const FString& message);
#endif // PLATFORM_ANDROID
	inline UAuthenticator * Callback = nullptr;
	void SignInWithGoogle(const FString& clientId, const FString& nonce, UAuthenticator * AuthCallback);
}