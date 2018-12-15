#pragma once

#include <napi.h>
#include <libindi/baseclient.h>
#include <libindi/basedevice.h>

class Property : public Napi::ObjectWrap<Property>
{
  public:
    Property(const Napi::CallbackInfo &);
    Napi::Value GetName(const Napi::CallbackInfo &);
    Napi::Value GetType(const Napi::CallbackInfo &);
    Napi::Value GetLabel(const Napi::CallbackInfo &);
    Napi::Value GetGroupName(const Napi::CallbackInfo &);
    Napi::Value GetDeviceName(const Napi::CallbackInfo &);
    Napi::Value GetTimestamp(const Napi::CallbackInfo &);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);

  private:
    static Napi::FunctionReference constructor;
    INDI::Property *_property;
};
