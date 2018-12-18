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

  private:
    static Napi::FunctionReference constructor;
    INumber *_number;
};
