
#include "value.h"
#include <libindi/indicom.h>

// base value

template<typename T, typename V>
BaseValue<T, V>::BaseValue(const Napi::CallbackInfo& info) : Napi::ObjectWrap<T>(info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    _value = info[0].As<Napi::External<V>>().Data();
}

template<typename T, typename V>
Napi::Value BaseValue<T, V>::ToObject(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    Napi::Object propNames = Napi::Object::New(env);

    propNames.Set("name", _value->name);
    propNames.Set("label", _value->label);
    propNames.Set("value", getValue(env));

    setProps(propNames);

    return propNames;
}

template<typename T, typename V>
Napi::Value BaseValue<T, V>::GetName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), _value->name);
}

template<typename T, typename V>
void BaseValue<T, V>::SetName(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value BaseValue<T, V>::GetLabel(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), _value->label);
}

template<typename T, typename V>
void BaseValue<T, V>::SetLabel(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value BaseValue<T, V>::GetValue(const Napi::CallbackInfo& info) {
    return getValue(info.Env());
}

template<typename T, typename V>
void BaseValue<T, V>::SetValue(const Napi::CallbackInfo& info, const Napi::Value& value) {
    setValue(info.Env(), value);
}

template<typename T, typename V>
Napi::Function BaseValue<T, V>::GetClass(Napi::Env env, Napi::Object exports, const char* name,
    std::vector<Napi::ClassPropertyDescriptor<T>> properties) {
    // Napi::HandleScope scope(env);

    Napi::Function symbolFor =
        Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
    Napi::Symbol inspectSymbol =
        symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

    properties.push_back(BaseValue::InstanceMethod("toJSON", &BaseValue::ToObject));
    properties.push_back(BaseValue::InstanceMethod("toJSON", &BaseValue::ToObject));
    properties.push_back(BaseValue::InstanceMethod(inspectSymbol, &BaseValue::ToObject));
    properties.push_back(
        BaseValue::InstanceAccessor("name", &BaseValue::GetName, &BaseValue::SetName));
    properties.push_back(
        BaseValue::InstanceAccessor("label", &BaseValue::GetLabel, &BaseValue::SetLabel));
    properties.push_back(
        BaseValue::InstanceAccessor("value", &BaseValue::GetValue, &BaseValue::SetValue));

    Napi::Function func = BaseValue::DefineClass(env, name, properties);

    return func;
}

// number value

NumberValue::NumberValue(const Napi::CallbackInfo& info) : BaseValue(info) {}

Napi::FunctionReference NumberValue::constructor;

Napi::Value NumberValue::GetMin(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), getHandle()->min);
}

Napi::Value NumberValue::GetMax(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), getHandle()->max);
}

Napi::Value NumberValue::GetStep(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), getHandle()->step);
}

Napi::Value NumberValue::GetFormated(const Napi::CallbackInfo& info) {
    char buf[MAXINDIFORMAT];
    numberFormat(buf, getHandle()->format, getHandle()->value);
    return Napi::String::New(info.Env(), buf);
}

void NumberValue::GetClass(Napi::Env env, Napi::Object exports) {
    // Napi::HandleScope scope(env);

    Napi::Function func = BaseValue::GetClass(env, exports, "NumberValue",
        {InstanceAccessor("min", &NumberValue::GetMin, nullptr),
            InstanceAccessor("max", &NumberValue::GetMax, nullptr),
            InstanceAccessor("step", &NumberValue::GetStep, nullptr),
            InstanceAccessor("formated", &NumberValue::GetFormated, nullptr)});

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object NumberValue::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

// Switch value

SwitchValue::SwitchValue(const Napi::CallbackInfo& info) : BaseValue(info) {}

Napi::FunctionReference SwitchValue::constructor;

void SwitchValue::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = BaseValue::GetClass(env, exports, "SwitchValue", {});
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    /*Napi::Function symbolFor =
        Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
    Napi::Symbol inspectSymbol =
        symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

    Napi::Function func =
        DefineClass(env, "SwitchValue",
                    {
                        InstanceMethod("toJSON", &SwitchValue::ToObject),
                        InstanceMethod(inspectSymbol, &SwitchValue::ToObject),
                        InstanceAccessor("name", &SwitchValue::GetName, &SwitchValue::SetName),
                        InstanceAccessor("label", &SwitchValue::GetLabel, &SwitchValue::SetLabel),
                        InstanceAccessor("value", &SwitchValue::GetValue, &SwitchValue::SetValue),
                    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();*/
}

Napi::Object SwitchValue::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

// Text value

TextValue::TextValue(const Napi::CallbackInfo& info) : BaseValue(info) {}

Napi::FunctionReference TextValue::constructor;

void TextValue::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function symbolFor =
        Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
    Napi::Symbol inspectSymbol =
        symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

    Napi::Function func = DefineClass(env, "TextValue",
        {
            InstanceMethod("toJSON", &TextValue::ToObject),
            InstanceMethod(inspectSymbol, &TextValue::ToObject),
            InstanceAccessor("name", &TextValue::GetName, &TextValue::SetName),
            InstanceAccessor("label", &TextValue::GetLabel, &TextValue::SetLabel),
            InstanceAccessor("value", &TextValue::GetValue, &TextValue::SetValue),
        });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object TextValue::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

// light value

LightValue::LightValue(const Napi::CallbackInfo& info) : BaseValue(info) {}

Napi::FunctionReference LightValue::constructor;

void LightValue::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function symbolFor =
        Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
    Napi::Symbol inspectSymbol =
        symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

    Napi::Function func = DefineClass(env, "LightValue",
        {
            InstanceMethod("toJSON", &LightValue::ToObject),
            InstanceMethod(inspectSymbol, &LightValue::ToObject),
            InstanceAccessor("name", &LightValue::GetName, &LightValue::SetName),
            InstanceAccessor("label", &LightValue::GetLabel, &LightValue::SetLabel),
            InstanceAccessor("value", &LightValue::GetValue, &LightValue::SetValue),
        });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

Napi::Object LightValue::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}