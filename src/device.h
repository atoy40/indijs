#pragma once

#include <libindi/baseclient.h>
#include <libindi/basedevice.h>
#include <napi.h>

class Device : public Napi::ObjectWrap<Device> {
  public:
    Device(const Napi::CallbackInfo&);
    Napi::Value GetDeviceName(const Napi::CallbackInfo&);
    Napi::Value IsConnected(const Napi::CallbackInfo&);
    Napi::Value IsValid(const Napi::CallbackInfo&);
    Napi::Value GetProperty(const Napi::CallbackInfo&);
    Napi::Value GetProperties(const Napi::CallbackInfo&);
    Napi::Value GetDriverInterface(const Napi::CallbackInfo&);
    Napi::Value MessageQueue(const Napi::CallbackInfo&);
    Napi::Value WatchProperty(const Napi::CallbackInfo&);
    Napi::Value ToObject(const Napi::CallbackInfo& info);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);

  private:
    static Napi::FunctionReference constructor;
    INDI::BaseDevice _device;
};
