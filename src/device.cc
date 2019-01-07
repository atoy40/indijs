
#include "device.h"

using namespace Napi;

Device::Device(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    Napi::Env env = info.Env();

    if(info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    _device = info[0].As<Napi::External<INDI::BaseDevice>>().Data();
}

Napi::FunctionReference Device::constructor;

Napi::Value Device::GetDeviceName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if(info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, _device->getDeviceName());
}

Napi::Value Device::ToObject(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    Napi::Object propNames = Napi::Object::New(env);

    propNames.Set("name", _device->getDeviceName());

    return propNames;
}

void Device::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function symbolFor =
        Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
    Napi::Symbol inspectSymbol =
        symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

    Napi::Function func =
        DefineClass(env, "Device",
                    {
                        InstanceMethod("toJSON", &Device::ToObject),
                        InstanceMethod(inspectSymbol, &Device::ToObject),
                        Device::InstanceMethod("getDeviceName", &Device::GetDeviceName),
                    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object Device::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}
