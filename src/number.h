#pragma once

#include <napi.h>
#include <libindi/baseclient.h>
#include <libindi/basedevice.h>

class Number : public Napi::ObjectWrap<Number>
{
  public:
    Number(const Napi::CallbackInfo &);
    Napi::Value GetName(const Napi::CallbackInfo &info);
    void SetName(const Napi::CallbackInfo &info, const Napi::Value &value);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);

  private:
    static Napi::FunctionReference constructor;
    INumberVectorProperty *_number;
};
