#include "prop.h"
#include "device.h"
#include "value.h"
#include <iostream>

template<typename T, typename V>
PropertyBase<T, V>::PropertyBase(const Napi::CallbackInfo& info) : Napi::ObjectWrap<T>(info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }
}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetBaseDevice(const Napi::CallbackInfo& info) {
    Napi::Object dev = Device::NewInstance(Napi::External<INDI::BaseDevice>::New(info.Env(),
        new INDI::BaseDevice(getProperty().getBaseDevice()), [](Napi::Env env, INDI::BaseDevice* dp) { delete dp; }));

    return dev;
}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetRegistered(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), getProperty().getRegistered());
}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetType(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), getProperty().getType());
}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetTypeAsString(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), getProperty().getTypeAsString());
}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetDeviceName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), getProperty().getDeviceName());
}

template<typename T, typename V>
void PropertyBase<T, V>::SetDeviceName(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), getProperty().getName());
}

template<typename T, typename V>
void PropertyBase<T, V>::SetName(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetLabel(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), getProperty().getLabel());
}

template<typename T, typename V>
void PropertyBase<T, V>::SetLabel(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetGroupName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), getProperty().getGroupName());
}

template<typename T, typename V>
void PropertyBase<T, V>::SetGroupName(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetState(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), getProperty().getState());
}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetStateAsString(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), getProperty().getStateAsString());
}

template<typename T, typename V>
void PropertyBase<T, V>::SetState(const Napi::CallbackInfo& info, const Napi::Value& value) {}

/*template<typename T, typename V>
Napi::Value Property<T, V>::GetValues(const Napi::CallbackInfo& info) {
    return getProperty()sref.Value();
}

template<typename T, typename V>
void Property<T, V>::SetValues(const Napi::CallbackInfo& info, const Napi::Value& value) {}*/

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetTimestamp(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), getProperty().getTimestamp());
}

template<typename T, typename V>
void PropertyBase<T, V>::SetTimestamp(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetPermission(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), getProperty().getPermission());
}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::GetPermissionAsString(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), getProperty().getPermissionAsString());
}

template<typename T, typename V>
void PropertyBase<T, V>::SetPermission(const Napi::CallbackInfo& info, const Napi::Value& value) {}

template<typename T, typename V>
Napi::Value PropertyBase<T, V>::Count(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), getProperty().count());
}

template<typename T, typename V>
Napi::Function PropertyBase<T, V>::GetClass(Napi::Env env, Napi::Object exports, const char* name) {
    std::vector<Napi::ClassPropertyDescriptor<T>> properties;
    return GetClass(env, exports, name, properties);
}

template<typename T, typename V>
Napi::Function PropertyBase<T, V>::GetClass(
    Napi::Env env, Napi::Object exports, const char* name, std::vector<Napi::ClassPropertyDescriptor<T>>& properties) {
    Napi::HandleScope scope(env);

    properties.push_back(PropertyBase::InstanceMethod("getBaseDevice", &PropertyBase::GetBaseDevice));
    properties.push_back(PropertyBase::InstanceMethod("getRegistered", &PropertyBase::GetRegistered));
    properties.push_back(PropertyBase::InstanceMethod("getType", &PropertyBase::GetType));
    properties.push_back(PropertyBase::InstanceMethod("getTypeAsString", &PropertyBase::GetTypeAsString));
    properties.push_back(PropertyBase::InstanceMethod("getDeviceName", &PropertyBase::GetDeviceName));
    properties.push_back(PropertyBase::InstanceMethod("getName", &PropertyBase::GetName));
    properties.push_back(PropertyBase::InstanceMethod("getLabel", &PropertyBase::GetLabel));
    properties.push_back(PropertyBase::InstanceMethod("getGroupName", &PropertyBase::GetGroupName));
    properties.push_back(PropertyBase::InstanceMethod("getState", &PropertyBase::GetState));
    properties.push_back(PropertyBase::InstanceMethod("getStateAsString", &PropertyBase::GetStateAsString));
    properties.push_back(PropertyBase::InstanceMethod("getTimestamp", &PropertyBase::GetTimestamp));
    properties.push_back(PropertyBase::InstanceMethod("getPermission", &PropertyBase::GetPermission));
    properties.push_back(PropertyBase::InstanceMethod("getPermissionAsString", &PropertyBase::GetPermissionAsString));
    properties.push_back(PropertyBase::InstanceMethod("count", &PropertyBase::Count));

    /*properties.push_back(Property::InstanceAccessor<&Property::GetValues>("values"));*/

    Napi::Function func = PropertyBase::DefineClass(env, name, properties);

    return func;
}

////////////////////
// Number property
////////////////////

Napi::FunctionReference PropertyNumber::constructor;

PropertyNumber::PropertyNumber(const Napi::CallbackInfo& info) : PropertyBase(info), _prop(getProp(info)) {}

INDI::PropertyNumber PropertyNumber::getProp(const Napi::CallbackInfo& info) {
    return *(info[0].As<Napi::External<INDI::PropertyNumber>>().Data());
}

Napi::Object PropertyNumber::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

Napi::Value PropertyNumber::GetValues(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array ret = Napi::Array::New(env);

    for (auto itr = _prop.begin(); itr != _prop.end(); itr++) {
        auto val = *itr;
        Napi::Object value = NumberValue::NewInstance(Napi::External<INDI::WidgetViewNumber>::New(env, itr));
        ret.Set(std::distance(_prop.begin(), itr), value);
    }

    return ret;
}

void PropertyNumber::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    std::vector<Napi::ClassPropertyDescriptor<PropertyNumber>> properties = {
        InstanceMethod("getValues", &PropertyNumber::GetValues)};

    Napi::Function func = PropertyBase::GetClass(env, exports, "PropertyNumber", properties);

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

////////////////////
// Switch property
////////////////////

Napi::FunctionReference PropertySwitch::constructor;

PropertySwitch::PropertySwitch(const Napi::CallbackInfo& info) : PropertyBase(info), _prop(getProp(info)) {}

INDI::PropertySwitch PropertySwitch::getProp(const Napi::CallbackInfo& info) {
    return *(info[0].As<Napi::External<INDI::PropertySwitch>>().Data());
}

Napi::Object PropertySwitch::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

Napi::Value PropertySwitch::GetValues(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array ret = Napi::Array::New(env);

    for (auto itr = _prop.begin(); itr != _prop.end(); itr++) {
        auto val = *itr;
        Napi::Object value = SwitchValue::NewInstance(Napi::External<INDI::WidgetViewSwitch>::New(env, itr));
        ret.Set(std::distance(_prop.begin(), itr), value);
    }

    return ret;
}

Napi::Value PropertySwitch::GetRule(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, _prop.getRule());
}

Napi::Value PropertySwitch::GetRuleAsString(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, _prop.getRuleAsString());
}

Napi::Value PropertySwitch::FindOnSwitchIndex(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, _prop.findOnSwitchIndex());
}

Napi::Value PropertySwitch::FindOnSwitchName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, _prop.findOnSwitchName());
}

Napi::Value PropertySwitch::FindOnSwitch(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto w = _prop.findOnSwitch();

    if (w) {
        return SwitchValue::NewInstance(Napi::External<INDI::WidgetViewSwitch>::New(env, w));
    }

    return env.Undefined();
}

void PropertySwitch::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    std::vector<Napi::ClassPropertyDescriptor<PropertySwitch>> properties = {
        InstanceMethod<&PropertySwitch::GetValues>("getValues"), InstanceMethod<&PropertySwitch::GetRule>("getRule"),
        InstanceMethod<&PropertySwitch::GetRuleAsString>("getRuleAsString"),
        InstanceMethod<&PropertySwitch::FindOnSwitchIndex>("findOnSwitchIndex"),
        InstanceMethod<&PropertySwitch::FindOnSwitchIndex>("findOnSwitchName"),
        InstanceMethod<&PropertySwitch::FindOnSwitch>("findOnSwitch")};

    Napi::Function func = PropertyBase::GetClass(env, exports, "PropertySwitch", properties);

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

////////////////////
// Text property
////////////////////

Napi::FunctionReference PropertyText::constructor;

PropertyText::PropertyText(const Napi::CallbackInfo& info) : PropertyBase(info), _prop(getProp(info)) {}

INDI::PropertyText PropertyText::getProp(const Napi::CallbackInfo& info) {
    return *(info[0].As<Napi::External<INDI::PropertyText>>().Data());
}

Napi::Object PropertyText::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

Napi::Value PropertyText::GetValues(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array ret = Napi::Array::New(env);

    for (auto itr = _prop.begin(); itr != _prop.end(); itr++) {
        auto val = *itr;
        Napi::Object value = TextValue::NewInstance(Napi::External<INDI::WidgetViewText>::New(env, itr));
        ret.Set(std::distance(_prop.begin(), itr), value);
    }

    return ret;
}

void PropertyText::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    std::vector<Napi::ClassPropertyDescriptor<PropertyText>> properties = {
        InstanceMethod("getValues", &PropertyText::GetValues)};

    Napi::Function func = PropertyBase::GetClass(env, exports, "PropertyText", properties);

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

////////////////////
// Light property
////////////////////

Napi::FunctionReference PropertyLight::constructor;

PropertyLight::PropertyLight(const Napi::CallbackInfo& info) : PropertyBase(info), _prop(getProp(info)) {}

INDI::PropertyLight PropertyLight::getProp(const Napi::CallbackInfo& info) {
    return *(info[0].As<Napi::External<INDI::PropertyLight>>().Data());
}

Napi::Object PropertyLight::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

Napi::Value PropertyLight::GetValues(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array ret = Napi::Array::New(env);

    for (auto itr = _prop.begin(); itr != _prop.end(); itr++) {
        auto val = *itr;
        Napi::Object value = LightValue::NewInstance(Napi::External<INDI::WidgetViewLight>::New(env, itr));
        ret.Set(std::distance(_prop.begin(), itr), value);
    }

    return ret;
}

void PropertyLight::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    std::vector<Napi::ClassPropertyDescriptor<PropertyLight>> properties = {
        InstanceMethod("getValues", &PropertyLight::GetValues)};

    Napi::Function func = PropertyBase::GetClass(env, exports, "PropertyLight", properties);

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}

// utility functions

Napi::Object getTypedProperty(Napi::Env env, INDI::Property property) {
    switch (property.getType()) {
        case INDI_NUMBER: {
            Napi::Object prop = PropertyNumber::NewInstance(Napi::External<INDI::PropertyNumber>::New(
                env, new INDI::PropertyNumber(property), [](Napi::Env env, INDI::PropertyNumber* dp) { delete dp; }));
            return prop;
        }
        case INDI_SWITCH: {
            Napi::Object prop = PropertySwitch::NewInstance(Napi::External<INDI::PropertySwitch>::New(
                env, new INDI::PropertySwitch(property), [](Napi::Env env, INDI::PropertySwitch* dp) { delete dp; }));
            return prop;
        }
        case INDI_TEXT: {
            Napi::Object prop = PropertyText::NewInstance(Napi::External<INDI::PropertyText>::New(
                env, new INDI::PropertyText(property), [](Napi::Env env, INDI::PropertyText* dp) { delete dp; }));
            return prop;
        }
        case INDI_LIGHT: {
            Napi::Object prop = PropertyLight::NewInstance(Napi::External<INDI::PropertyLight>::New(
                env, new INDI::PropertyLight(property), [](Napi::Env env, INDI::PropertyLight* dp) { delete dp; }));
            return prop;
        }
    }
}