#pragma once

#include <libindi/baseclient.h>
#include <libindi/basedevice.h>
#include <napi.h>

class Property : public Napi::ObjectWrap<Property> {
  public:
    Property(const Napi::CallbackInfo&);
    Napi::Value GetRegistered(const Napi::CallbackInfo&);
    Napi::Value GetName(const Napi::CallbackInfo&);
    Napi::Value GetType(const Napi::CallbackInfo&);
    Napi::Value GetLabel(const Napi::CallbackInfo&);
    Napi::Value GetGroupName(const Napi::CallbackInfo&);
    Napi::Value GetDeviceName(const Napi::CallbackInfo&);
    Napi::Value GetTimestamp(const Napi::CallbackInfo&);
    Napi::Value GetState(const Napi::CallbackInfo&);
    Napi::Value GetPermission(const Napi::CallbackInfo&);
    Napi::Value GetValue(const Napi::CallbackInfo&);
    Napi::Value GetNumber(const Napi::CallbackInfo&);
    Napi::Value GetSwitch(const Napi::CallbackInfo&);
    Napi::Value GetText(const Napi::CallbackInfo&);
    Napi::Value GetLight(const Napi::CallbackInfo&);
    Napi::Value GetBLOB(const Napi::CallbackInfo&);

    static void GetClass(Napi::Env, Napi::Object);
    static Napi::Object NewInstance(Napi::Value);

  private:
    static Napi::FunctionReference constructor;
    INDI::Property* _property;
};
