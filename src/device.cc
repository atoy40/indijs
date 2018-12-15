
#include "device.h"

using namespace Napi;

Device::Device(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return;
    }

    _device = info[0].As<Napi::External<INDI::BaseDevice>>().Data();
}

Napi::FunctionReference Device::constructor;

Napi::Value Device::GetDeviceName(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() > 0)
    {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, _device->getDeviceName());
}

void Device::GetClass(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Device", {
                                                         Device::InstanceMethod("getDeviceName", &Device::GetDeviceName),
                                                     });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object Device::NewInstance(Napi::Value arg)
{
    Napi::Object obj = constructor.New({arg});
    return obj;
}
