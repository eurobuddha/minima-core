#pragma once
#include "NativeStackTrace.hpp"
#include <stdexcept>
#include <string>
namespace minima {
class JavaCheckedException:public std::runtime_error,public NativeStackTrace {public:using std::runtime_error::runtime_error;};
class JavaNumberFormatException:public std::invalid_argument,public NativeStackTrace {public:using std::invalid_argument::invalid_argument;};
class JavaNegativeArraySizeException:public std::runtime_error,public NativeStackTrace {public:using std::runtime_error::runtime_error;};
class JavaNullPointerException:public std::runtime_error,public NativeStackTrace {public:using std::runtime_error::runtime_error;};
class JavaClassCastException:public std::runtime_error,public NativeStackTrace {public:using std::runtime_error::runtime_error;};
class JavaIllegalMonitorStateException:public std::runtime_error,public NativeStackTrace {public:using std::runtime_error::runtime_error;};
class JavaIllegalArgumentException:public std::invalid_argument,public NativeStackTrace {public:using std::invalid_argument::invalid_argument;};
class JavaArrayIndexOutOfBoundsException:public std::out_of_range,public NativeStackTrace {public:using std::out_of_range::out_of_range;};
class JavaTypedException:public std::runtime_error,public NativeStackTrace {
    std::string mClass,mMessage;
    bool mPreserveEmpty;
public:
    JavaTypedException(std::string name,std::string message,bool preserveEmpty=false):std::runtime_error(message),mClass(std::move(name)),mMessage(std::move(message)),mPreserveEmpty(preserveEmpty){}
    std::string toString() const{return mClass+(mMessage.empty()&&!mPreserveEmpty?"":": "+mMessage);}
};
// Actual native exception types: classification must not depend on text that
// a user-defined toString can change. MVStore is a carrier, not an H2 backend.
class JavaError:public JavaTypedException {
public:JavaError(std::string name,std::string message):JavaTypedException(std::move(name),std::move(message),true){}
};
class JavaOutOfMemoryError:public JavaError {
public:explicit JavaOutOfMemoryError(std::string message):JavaError("java.lang.OutOfMemoryError",std::move(message)){}
};
class JavaMVStoreException:public JavaTypedException {
public:explicit JavaMVStoreException(std::string message):JavaTypedException("org.h2.mvstore.MVStoreException",std::move(message),true){}
};
class JavaTimeoutException:public JavaTypedException {
public:explicit JavaTimeoutException(std::string message):JavaTypedException("java.util.concurrent.TimeoutException",std::move(message),true){}
};
class JavaArithmeticException:public JavaTypedException {
public:explicit JavaArithmeticException(std::string message):JavaTypedException("java.lang.ArithmeticException",std::move(message),true){}
};
inline std::string javaExceptionText(const std::exception& e){
    if(auto typed=dynamic_cast<const JavaTypedException*>(&e))return typed->toString();
    if(dynamic_cast<const std::bad_alloc*>(&e))return "java.lang.OutOfMemoryError";
    if(dynamic_cast<const JavaArrayIndexOutOfBoundsException*>(&e))return std::string("java.lang.ArrayIndexOutOfBoundsException: ")+e.what();
    if(dynamic_cast<const JavaClassCastException*>(&e))return std::string("java.lang.ClassCastException: ")+e.what();
    if(dynamic_cast<const JavaIllegalMonitorStateException*>(&e))return std::string("java.lang.IllegalMonitorStateException")+(std::string(e.what()).empty()?"":std::string(": ")+e.what());
    if(dynamic_cast<const JavaCheckedException*>(&e))return std::string("java.lang.Exception: ")+e.what();
    if(dynamic_cast<const JavaNumberFormatException*>(&e))return std::string("java.lang.NumberFormatException: ")+e.what();
    if(dynamic_cast<const JavaNegativeArraySizeException*>(&e))return std::string("java.lang.NegativeArraySizeException: ")+e.what();
    if(dynamic_cast<const JavaNullPointerException*>(&e))return std::string("java.lang.NullPointerException: ")+e.what();
    if(dynamic_cast<const std::invalid_argument*>(&e))return std::string("java.lang.IllegalArgumentException: ")+e.what();
    return std::string("java.lang.RuntimeException: ")+e.what();
}
}
