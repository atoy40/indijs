#pragma once

#include <iostream>
#include <napi.h>
#include <libindi/baseclient.h>
#include <libindi/basedevice.h>

class Device : public Napi::ObjectWrap<Device>
{
  public:
    Device(const Napi::CallbackInfo &);
    Napi::Value GetDeviceName(const Napi::CallbackInfo &);
    Napi::Value ToObject(const Napi::CallbackInfo &info);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);

  private:
    static Napi::FunctionReference constructor;
    INDI::BaseDevice *_device;
};
