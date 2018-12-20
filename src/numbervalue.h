#pragma once

#include "basevalue.h"

#include <napi.h>
#include <libindi/baseclient.h>
#include <libindi/basedevice.h>

class NumberValue : public BaseValue<NumberValue, INumber>
{
  public:
    NumberValue(const Napi::CallbackInfo &);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);

    Napi::Value getValue(const Napi::Env &env) {
      return Napi::Number::New(env, getHandle()->value);
    }

    void setProps(Napi::Object &object) {
      object.Set("min", getHandle()->min);
      object.Set("max", getHandle()->max);
      object.Set("step", getHandle()->step);
    }

  private:
    static Napi::FunctionReference constructor;
};


class SwitchValue : public BaseValue<SwitchValue, ISwitch>
{
  public:
    SwitchValue(const Napi::CallbackInfo &);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);

    Napi::Value getValue(const Napi::Env &env) {
      return Napi::Boolean::New(env, getHandle()->s);
    }

    void setProps(Napi::Object &object) {
    }

  private:
    static Napi::FunctionReference constructor;
};
