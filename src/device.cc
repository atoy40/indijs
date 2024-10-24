
#include "device.h"
#include "prop.h"
#include <iostream>

using namespace Napi;

Device::Device(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    _device = *(info[0].As<Napi::External<INDI::BaseDevice>>().Data());
}

Napi::FunctionReference Device::constructor;

Napi::Value Device::GetDeviceName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, _device.getDeviceName());
}

Napi::Value Device::IsConnected(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), _device.isConnected());
}

Napi::Value Device::IsValid(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), _device.isValid());
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

    INDI::Property p = _device.getProperty(info[0].As<Napi::String>().Utf8Value().c_str());

    if (p.isValid()) {
        return getTypedProperty(env, p);
        /*return Property::NewInstance(Napi::External<INDI::Property>::New(
            env, new INDI::Property(p), [](Napi::Env env, INDI::Property* dp) { delete dp; }));*/
    }

    return env.Undefined();
}

Napi::Value Device::GetProperties(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto properties = _device.getProperties();
    Napi::Array array = Napi::Array::New(env);

    for (auto itr = properties.begin(); itr != properties.end(); itr++) {
        array.Set(std::distance(properties.begin(), itr), getTypedProperty(env, *itr));
    }

    return array;
}

Napi::Value Device::GetDriverInterface(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, _device.getDriverInterface());
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

    std::string msg = _device.messageQueue(info[0].As<Number>().Int64Value());

    return Napi::String::New(env, msg);
}

Napi::Value Device::WatchProperty(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to set the property name").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsFunction()) {
        Napi::TypeError::New(env, "You need to set the callback function").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto tsfn = ThreadSafeFunction::New(env,
        info[1].As<Function>(), // JavaScript function called asynchronously
        "Resource Name", // Name
        0, // Unlimited queue
        1 // Only one thread will use this initially
    );

    _device.watchProperty(
        info[0].As<Napi::String>().Utf8Value().c_str(),
        [tsfn](INDI::Property property) {
            std::cout << "watch" << std::endl;

            auto callback = [property](Napi::Env env, Napi::Function jsCallback) {
                auto prop = getTypedProperty(env, property);
                jsCallback.Call({prop});
            };

            tsfn.BlockingCall(callback);
            // tsfn.
        },
        INDI::BaseDevice::WATCH_NEW_OR_UPDATE);

    return env.Undefined();
}

Napi::Value Device::ToObject(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    Napi::Object propNames = Napi::Object::New(env);

    propNames.Set("name", _device.getDeviceName());

    return propNames;
}

void Device::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function symbolFor = Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
    Napi::Symbol inspectSymbol =
        symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

    Napi::Function func = DefineClass(env, "Device",
        {InstanceMethod("toJSON", &Device::ToObject), InstanceMethod(inspectSymbol, &Device::ToObject),
            Device::InstanceMethod("getDeviceName", &Device::GetDeviceName),
            Device::InstanceAccessor("connected", &Device::IsConnected, nullptr),
            Device::InstanceMethod("isValid", &Device::IsValid),
            Device::InstanceMethod("getProperty", &Device::GetProperty),
            Device::InstanceMethod("getProperties", &Device::GetProperties),
            Device::InstanceMethod("getDriverInterface", &Device::GetDriverInterface),
            Device::InstanceMethod("messageQueue", &Device::MessageQueue),
            Device::InstanceMethod("watchProperty", &Device::WatchProperty)});

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object Device::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}
