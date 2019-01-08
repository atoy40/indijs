#pragma once

#include "value.h"
#include <libindi/baseclient.h>
#include <napi.h>

template<typename T, typename V>
class BaseVector : public Napi::ObjectWrap<T> {
  public:
    BaseVector(const Napi::CallbackInfo&);
    Napi::Value ToObject(const Napi::CallbackInfo& info);

    Napi::Value GetDevice(const Napi::CallbackInfo& info);
    void SetDevice(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetName(const Napi::CallbackInfo& info);
    void SetName(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetLabel(const Napi::CallbackInfo& info);
    void SetLabel(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetGroup(const Napi::CallbackInfo& info);
    void SetGroup(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetState(const Napi::CallbackInfo& info);
    void SetState(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetValues(const Napi::CallbackInfo& info);
    void SetValues(const Napi::CallbackInfo& info, const Napi::Value& value);

    static Napi::Function GetClass(Napi::Env env, Napi::Object exports, const char* name);

    // virtual Napi::Value getValue(const Napi::Env &env) = 0;
    // virtual void setProps(Napi::Object &object) = 0;

    V* getHandle() {
        return _value;
    }

    Napi::Array getValues() {
        return _valuesref.Value();
    }

  private:
    V* _value;
    Napi::Reference<Napi::Array> _valuesref;
};

class NumberVector : public BaseVector<NumberVector, INumberVectorProperty> {
  public:
    NumberVector(const Napi::CallbackInfo& info);

    static Napi::Object NewInstance(Napi::Value arg);
    static void GetClass(Napi::Env env, Napi::Object exports);

  private:
    static Napi::FunctionReference constructor;
};

class SwitchVector : public BaseVector<SwitchVector, ISwitchVectorProperty> {
  public:
    SwitchVector(const Napi::CallbackInfo& info);

    static Napi::Object NewInstance(Napi::Value arg);
    static void GetClass(Napi::Env env, Napi::Object exports);

  private:
    static Napi::FunctionReference constructor;
};

class TextVector : public BaseVector<TextVector, ITextVectorProperty> {
  public:
    TextVector(const Napi::CallbackInfo& info);

    static Napi::Object NewInstance(Napi::Value arg);
    static void GetClass(Napi::Env env, Napi::Object exports);

  private:
    static Napi::FunctionReference constructor;
};

class LightVector : public BaseVector<LightVector, ILightVectorProperty> {
  public:
    LightVector(const Napi::CallbackInfo& info);

    static Napi::Object NewInstance(Napi::Value arg);
    static void GetClass(Napi::Env env, Napi::Object exports);

  private:
    static Napi::FunctionReference constructor;
};