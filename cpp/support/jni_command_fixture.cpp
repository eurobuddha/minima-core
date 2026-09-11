// Test-only JNI library: records calls and returns chosen bytes. No mining algorithm.
#include <jni.h>
extern "C" JNIEXPORT void JNICALL Java_org_minima_utils_jni_jnifunctions_sayHello(JNIEnv* env,jobject) {
    auto klass=env->FindClass("JNICommandOracle");if(!klass)return;
    auto method=env->GetStaticMethodID(klass,"hello","()V");if(method)env->CallStaticVoidMethod(klass,method);
}
extern "C" JNIEXPORT jbyteArray JNICALL Java_org_minima_utils_jni_jnifunctions_hashHeaderWithDiff(JNIEnv* env,jobject,jbyteArray nonce,jint attempts,jbyteArray difficulty,jbyteArray header) {
    auto klass=env->FindClass("JNICommandOracle");if(!klass)return nullptr;
    auto method=env->GetStaticMethodID(klass,"hash","([BI[B[B)[B");
    return method?static_cast<jbyteArray>(env->CallStaticObjectMethod(klass,method,nonce,attempts,difficulty,header)):nullptr;
}
