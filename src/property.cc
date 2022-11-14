
#include "property.h"
#include "vector.h"

using namespace Napi;

Napi::FunctionReference Property::constructor;

Property::Property(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    _property = info[0].As<Napi::External<INDI::Property>>().Data();
}

Napi::Value Property::GetRegistered(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, _property->getRegistered());
}

Napi::Value Property::GetName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, _property->getName());
}

Napi::Value Property::GetType(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, _property->getType());
}

Napi::Value Property::GetLabel(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, _property->getLabel());
}

Napi::Value Property::GetGroupName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, _property->getGroupName());
}

Napi::Value Property::GetDeviceName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, _property->getDeviceName());
}

Napi::Value Property::GetTimestamp(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, _property->getTimestamp());
}

Napi::Value Property::GetState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, _property->getState());
}

Napi::Value Property::GetPermission(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, _property->getPermission());
}

Napi::Value Property::GetValue(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    switch (_property->getType()) {
        case INDI_SWITCH:
            return SwitchVector::NewInstance(
                Napi::External<ISwitchVectorProperty>::New(env, _property->getSwitch()));
        case INDI_NUMBER:
            return NumberVector::NewInstance(
                Napi::External<INumberVectorProperty>::New(env, _property->getNumber()));
        case INDI_TEXT:
            return TextVector::NewInstance(
                Napi::External<ITextVectorProperty>::New(env, _property->getText()));
        case INDI_LIGHT:
            return LightVector::NewInstance(
                Napi::External<ILightVectorProperty>::New(env, _property->getLight()));
        case INDI_BLOB:
            return BLOBVector::NewInstance(
                Napi::External<IBLOBVectorProperty>::New(env, _property->getBLOB()));
        default:
            return env.Undefined();
    }
    return env.Undefined();
}

Napi::Value Property::GetNumber(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto value = _property->getNumber();

    if (value != nullptr) {
        return NumberVector::NewInstance(
            Napi::External<INumberVectorProperty>::New(env, value));
    }

    return env.Undefined();
}

Napi::Value Property::GetSwitch(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto value = _property->getSwitch();

    if (value != nullptr) {
        return SwitchVector::NewInstance(
            Napi::External<ISwitchVectorProperty>::New(env, value));
    }

    return env.Undefined();
}

Napi::Value Property::GetText(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto value = _property->getText();

    if (value != nullptr) {
        return TextVector::NewInstance(
            Napi::External<ITextVectorProperty>::New(env, value));
    }

    return env.Undefined();
}

Napi::Value Property::GetLight(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto value = _property->getLight();

    if (value != nullptr) {
        return LightVector::NewInstance(
            Napi::External<ILightVectorProperty>::New(env, value));
    }

    return env.Undefined();
}

Napi::Value Property::GetBLOB(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto value = _property->getBLOB();

    if (value != nullptr) {
        return BLOBVector::NewInstance(
            Napi::External<IBLOBVectorProperty>::New(env, value));
    }

    return env.Undefined();
}

void Property::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Property",
        {
            Property::InstanceMethod("getRegistered", &Property::GetRegistered),
            Property::InstanceMethod("getName", &Property::GetName),
            Property::InstanceMethod("getType", &Property::GetType),
            Property::InstanceMethod("getLabel", &Property::GetLabel),
            Property::InstanceMethod("getGroupName", &Property::GetGroupName),
            Property::InstanceMethod("getDeviceName", &Property::GetDeviceName),
            Property::InstanceMethod("getTimestamp", &Property::GetTimestamp),
            Property::InstanceMethod("getState", &Property::GetState),
            Property::InstanceMethod("getPermission", &Property::GetPermission),
            Property::InstanceMethod("getValue", &Property::GetValue),
            Property::InstanceMethod("getNumber", &Property::GetNumber),
            Property::InstanceMethod("getSwitch", &Property::GetSwitch),
            Property::InstanceMethod("getText", &Property::GetText),
            Property::InstanceMethod("getLight", &Property::GetLight),
            Property::InstanceMethod("getBLOB", &Property::GetBLOB),
        });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object Property::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}
