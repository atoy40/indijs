
#include "device.h"
#include "property.h"

using namespace Napi;

Device::Device(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    _device = info[0].As<Napi::External<INDI::BaseDevice>>().Data();
}

Napi::FunctionReference Device::constructor;

Napi::Value Device::GetDeviceName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, _device->getDeviceName());
}

Napi::Value Device::IsConnected(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), _device->isConnected());
}

Napi::Value Device::GetProperty(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to provide a property name").ThrowAsJavaScriptException();
        return env.Null();
    }

    INDI::Property *p = _device->getProperty(info[0].As<Napi::String>().Utf8Value().c_str());

    if (p) {
        return Property::NewInstance(Napi::External<INDI::Property>::New(env, p));
    }

    return env.Undefined();
}

Napi::Value Device::GetProperties(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::vector<INDI::Property*> *properties = _device->getProperties();
    Napi::Array array = Napi::Array::New(env);

    for (unsigned int i = 0; i < properties->size(); i++) {
        array.Set(i, Property::NewInstance(Napi::External<INDI::Property>::New(env, properties->at(i))));
    }

    return array;
}

Napi::Value Device::GetDriverInterface(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, _device->getDriverInterface());
}

Napi::Value Device::MessageQueue(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "You need to set the message ID").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string msg = _device->messageQueue(info[0].As<Number>().Int64Value());

    return Napi::String::New(env, msg);
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

    Napi::Function func = DefineClass(env, "Device",
        {InstanceMethod("toJSON", &Device::ToObject),
            InstanceMethod(inspectSymbol, &Device::ToObject),
            Device::InstanceMethod("getDeviceName", &Device::GetDeviceName),
            Device::InstanceAccessor("connected", &Device::IsConnected, nullptr),
            Device::InstanceMethod("getProperty", &Device::GetProperty),
            Device::InstanceMethod("getProperties", &Device::GetProperties),
            Device::InstanceMethod("getDriverInterface", &Device::GetDriverInterface),
            Device::InstanceMethod("messageQueue", &Device::MessageQueue)});

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object Device::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}
