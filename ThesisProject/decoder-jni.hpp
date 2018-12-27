#include <stdint.h>

namespace djinni {
  struct ByteBuffer {
        using CppType = int8_t *;
        using JniType = jobject;

        static CppType toCpp(JNIEnv* env, JniType j) {
            return (int8_t *) env->GetDirectBufferAddress(j);
        }

        static JniType fromCpp(JNIEnv *env, CppType c) {
            return env->NewDirectByteBuffer(c, sizeof(c));
        }
    };
	
	struct Point {
        using CppType = int8_t *;
        using JniType = jobject;

        static CppType toCpp(JNIEnv* env, JniType j) {
            return (int8_t *) env->GetDirectBufferAddress(j);
        }

        static JniType fromCpp(JNIEnv *env, CppType c) {
            return env->NewDirectByteBuffer(c, sizeof(c));
        }
    };
}