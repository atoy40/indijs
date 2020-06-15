#include "vector.h"
#include <iostream>
// Base vector

template<typename T, typename V>
BaseVector<T, V>::BaseVector(const Napi::CallbackInfo& info) : Napi::ObjectWrap<T>(info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    _value = info[0].As<Napi::External<V>>().Data();
    _valuesref = Napi::ObjectReference::New(Napi::Array::New(env), 1);
}

template<typename T, typename V>
BaseVector<T, V>::~BaseVector() {
    _valuesref.Unref();
}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::ToObject(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    Napi::Object props = Napi::Object::New(env);

    props.Set("device", _value->device);
    props.Set("name", _value->name);
    props.Set("label", _value->label);
    props.Set("group", _value->group);
    props.Set("state", Napi::Number::New(env, _value->s));
    props.Set("values", _valuesref.Value());

    // setProps(props);

    return props;
}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetDevice(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), _value->device);
}

template<typename T, typename V>
void BaseVector<T, V>::SetDevice(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), _value->name);
}

template<typename T, typename V>
void BaseVector<T, V>::SetName(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetLabel(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), _value->label);
}

template<typename T, typename V>
void BaseVector<T, V>::SetLabel(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetGroup(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), _value->group);
}

template<typename T, typename V>
void BaseVector<T, V>::SetGroup(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetState(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), _value->s);
}

template<typename T, typename V>
void BaseVector<T, V>::SetState(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetValues(const Napi::CallbackInfo& info) {
    return _valuesref.Value();
}

template<typename T, typename V>
void BaseVector<T, V>::SetValues(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetTimestamp(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), _value->timestamp);
}

template<typename T, typename V>
void BaseVector<T, V>::SetTimestamp(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetPermission(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), this->getPermission());
}

template<typename T, typename V>
void BaseVector<T, V>::SetPermission(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Function BaseVector<T, V>::GetClass(Napi::Env env, Napi::Object exports, const char* name) {
    std::vector<Napi::ClassPropertyDescriptor<T>> properties;
    return GetClass(env, exports, name, properties);
}

template<typename T, typename V>
Napi::Function BaseVector<T, V>::GetClass(Napi::Env env, Napi::Object exports, const char* name,
    std::vector<Napi::ClassPropertyDescriptor<T>>& properties) {
    Napi::HandleScope scope(env);

    Napi::Function symbolFor =
        Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
    Napi::Symbol inspectSymbol =
        symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

    properties.push_back(BaseVector::InstanceMethod("toJSON", &BaseVector::ToObject));
    properties.push_back(BaseVector::InstanceMethod(inspectSymbol, &BaseVector::ToObject));
    properties.push_back(
        BaseVector::InstanceAccessor("device", &BaseVector::GetDevice, &BaseVector::SetDevice));
    properties.push_back(
        BaseVector::InstanceAccessor("name", &BaseVector::GetName, &BaseVector::SetName));
    properties.push_back(
        BaseVector::InstanceAccessor("label", &BaseVector::GetLabel, &BaseVector::SetLabel));
    properties.push_back(
        BaseVector::InstanceAccessor("group", &BaseVector::GetGroup, &BaseVector::SetGroup));
    properties.push_back(
        BaseVector::InstanceAccessor("state", &BaseVector::GetState, &BaseVector::SetState));
    properties.push_back(
        BaseVector::InstanceAccessor("values", &BaseVector::GetValues, &BaseVector::SetValues));
    properties.push_back(BaseVector::InstanceAccessor(
        "timestamp", &BaseVector::GetTimestamp, &BaseVector::SetTimestamp));
    properties.push_back(BaseVector::InstanceAccessor(
        "permission", &BaseVector::GetPermission, &BaseVector::SetPermission));

    Napi::Function func = BaseVector::DefineClass(env, name, properties);

    return func;
}

// Number vector

Napi::FunctionReference NumberVector::constructor;

NumberVector::NumberVector(const Napi::CallbackInfo& info) : BaseVector(info) {
    for (int i = 0; i < getHandle()->nnp; i++) {
        Napi::Object value =
            NumberValue::NewInstance(Napi::External<INumber>::New(info.Env(), getHandle()->np + i));
        getValues().Set(i, value);
    }
}

Napi::Object NumberVector::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

void NumberVector::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = BaseVector::GetClass(env, exports, "NumberVector");

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

IPerm NumberVector::getPermission() {
    return getHandle()->p;
}

// Switch vector

Napi::FunctionReference SwitchVector::constructor;

SwitchVector::SwitchVector(const Napi::CallbackInfo& info) : BaseVector(info) {
    for (int i = 0; i < getHandle()->nsp; i++) {
        Napi::Object value =
            SwitchValue::NewInstance(Napi::External<ISwitch>::New(info.Env(), getHandle()->sp + i));
        getValues().Set(i, value);
    }
}

Napi::Value SwitchVector::GetRule(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), getHandle()->r);
}

Napi::Object SwitchVector::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

void SwitchVector::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    std::vector<Napi::ClassPropertyDescriptor<SwitchVector>> properties = {
        InstanceAccessor("rule", &SwitchVector::GetRule, nullptr)};

    Napi::Function func = BaseVector::GetClass(env, exports, "SwitchVector", properties);

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

IPerm SwitchVector::getPermission() {
    return getHandle()->p;
}

// Text vector

Napi::FunctionReference TextVector::constructor;

TextVector::TextVector(const Napi::CallbackInfo& info) : BaseVector(info) {
    for (int i = 0; i < getHandle()->ntp; i++) {
        Napi::Object value =
            TextValue::NewInstance(Napi::External<IText>::New(info.Env(), getHandle()->tp + i));
        getValues().Set(i, value);
        getValues().Set(getHandle()->tp[i].name, value);
    }
}

Napi::Object TextVector::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

void TextVector::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = BaseVector::GetClass(env, exports, "TextVector");

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

IPerm TextVector::getPermission() {
    return getHandle()->p;
}

// light vector

Napi::FunctionReference LightVector::constructor;

LightVector::LightVector(const Napi::CallbackInfo& info) : BaseVector(info) {
    for (int i = 0; i < getHandle()->nlp; i++) {
        Napi::Object value =
            LightValue::NewInstance(Napi::External<ILight>::New(info.Env(), getHandle()->lp + i));
        getValues().Set(i, value);
        getValues().Set(getHandle()->lp[i].name, value);
    }
}

Napi::Object LightVector::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

void LightVector::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = BaseVector::GetClass(env, exports, "LightVector");

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

IPerm LightVector::getPermission() {
    return IP_RO;
}