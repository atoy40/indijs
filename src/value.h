#pragma once

#include <libindi/baseclient.h>
#include <libindi/basedevice.h>
#include <napi.h>

template<typename T, typename V>
class BaseValue : public Napi::ObjectWrap<T> {
  public:
    BaseValue(const Napi::CallbackInfo&);
    virtual Napi::Value ToObject(const Napi::CallbackInfo& info);

    Napi::Value GetName(const Napi::CallbackInfo& info);
    void SetName(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetLabel(const Napi::CallbackInfo& info);
    void SetLabel(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetValue(const Napi::CallbackInfo& info);
    void SetValue(const Napi::CallbackInfo& info, const Napi::Value& value);

    virtual Napi::Value getValue(const Napi::Env& env) = 0;
    virtual void setValue(const Napi::Env& env, const Napi::Value& value) = 0;
    virtual void setProps(Napi::Object& object) = 0;

    static Napi::Function GetClass(Napi::Env env, Napi::Object exports, const char* name,
        std::vector<Napi::ClassPropertyDescriptor<T>> properties);

  protected:
    V* getHandle() {
        return _value;
    }

  private:
    V* _value;
    // static Napi::FunctionReference constructor;
};

class NumberValue : public BaseValue<NumberValue, INumber> {
  public:
    NumberValue(const Napi::CallbackInfo&);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);
    Napi::Value GetMin(const Napi::CallbackInfo& info);
    Napi::Value GetMax(const Napi::CallbackInfo& info);
    Napi::Value GetStep(const Napi::CallbackInfo& info);

    Napi::Value getValue(const Napi::Env& env) {
        return Napi::Number::New(env, getHandle()->value);
    }

    void setValue(const Napi::Env& env, const Napi::Value& value) {
        getHandle()->value = value.As<Napi::Number>().DoubleValue();
    }

    void setProps(Napi::Object& object) {
        object.Set("min", getHandle()->min);
        object.Set("max", getHandle()->max);
        object.Set("step", getHandle()->step);
    }

  private:
    static Napi::FunctionReference constructor;
};

class SwitchValue : public BaseValue<SwitchValue, ISwitch> {
  public:
    SwitchValue(const Napi::CallbackInfo&);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);

    Napi::Value getValue(const Napi::Env& env) {
        return Napi::Boolean::New(env, getHandle()->s);
    }

    void setValue(const Napi::Env& env, const Napi::Value& value) {
        getHandle()->s = value.As<Napi::Boolean>() ? ISS_ON : ISS_OFF;
    }

    void setProps(Napi::Object& object) {}

  private:
    static Napi::FunctionReference constructor;
};

class TextValue : public BaseValue<TextValue, IText> {
  public:
    TextValue(const Napi::CallbackInfo&);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);

    Napi::Value getValue(const Napi::Env& env) {
        return Napi::String::New(env, getHandle()->text);
    }

    void setValue(const Napi::Env& env, const Napi::Value& value) {
        getHandle()->text = strcpy(getHandle()->text, value.As<Napi::String>().Utf8Value().c_str());
    }

    void setProps(Napi::Object& object) {}

  private:
    static Napi::FunctionReference constructor;
};

class LightValue : public BaseValue<LightValue, ILight> {
  public:
    LightValue(const Napi::CallbackInfo&);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);

    Napi::Value getValue(const Napi::Env& env) {
        return Napi::Number::New(env, getHandle()->s);
    }

    void setValue(const Napi::Env& env, const Napi::Value& value) {
        getHandle()->s = IPState(value.As<Napi::Number>().Int32Value());
    }

    void setProps(Napi::Object& object) {}

  private:
    static Napi::FunctionReference constructor;
};
