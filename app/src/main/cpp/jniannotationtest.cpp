#include <android/log.h>
#include <jni.h>

#define ANDROID_O 26

static const char TAG[] = "JNIAnnotationTest";

void JNICALL normalFunc(JNIEnv *, jclass) {}
void JNICALL bangJniFunc(JNIEnv *, jclass) {}
void JNICALL fastNativeFunc(JNIEnv *, jclass) {}
void JNICALL criticalNativeFunc() {}

int getApiVersion(JNIEnv *env) {
    jint sdkInt = -1;
    bool success = true;
    jclass versionClass = env->FindClass("android/os/Build$VERSION");

    if (versionClass == nullptr) [[unlikely]] {
        success = false;
    }

    jfieldID sdkIntFieldID;
    if (success) [[likely]] {
        success = ((sdkIntFieldID = env->GetStaticFieldID(versionClass, "SDK_INT", "I")) != nullptr);
    }

    if (success) [[likely]] {
        sdkInt = env->GetStaticIntField(versionClass, sdkIntFieldID);
    }

    return sdkInt;
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) [[unlikely]] {
        return JNI_ERR;
    }

    int apiVersion = getApiVersion(env);

    if (apiVersion == -1) {
        __android_log_write(ANDROID_LOG_ERROR, TAG, "Could not get Android API version!");
        return JNI_ERR;
    }

    jint result;

    if (apiVersion >= ANDROID_O) {
        // Redirect func_bangJni to fastNativeFunc since !bang JNI is not supported anymore
        JNINativeMethod methodsInternal[] = {
                { "func_normal", "()V", (void *) normalFunc },
                { "func_bangJni", "()V", (void *) fastNativeFunc },
                { "func_fastnative", "()V", (void *) fastNativeFunc },
                { "func_criticalnative", "()V", (void *) criticalNativeFunc }
        };

        result = env->RegisterNatives(
                env->FindClass("website/nickb/jniannotationtest/MainActivity"),
                methodsInternal,
                sizeof(methodsInternal) / sizeof(JNINativeMethod));
    }
    else {
        // Redirect CriticalNative function to !bang JNI function (need JNIEnv and jclass on Android 7-)
        // !bang JNI notation used to demonstrate faster calls on Android 7-
        JNINativeMethod methodsInternal[] = {
                { "func_normal", "()V", (void *) normalFunc },
                { "func_bangJni", "!()V", (void *) bangJniFunc },
                { "func_fastnative", "!()V", (void *) bangJniFunc },
                { "func_criticalnative", "!()V", (void *) bangJniFunc }
        };

        result = env->RegisterNatives(
                env->FindClass("website/nickb/jniannotationtest/MainActivity"),
                methodsInternal,
                sizeof(methodsInternal) / sizeof(JNINativeMethod));
    }

    if (result != JNI_OK) [[unlikely]] {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "Error code %d when registering native functions!",
                            result);
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
