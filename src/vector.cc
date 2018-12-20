#include "vector.h"

// Base vector

template<typename T, typename V>
BaseVector<T, V>::BaseVector(const Napi::CallbackInfo& info) : Napi::ObjectWrap<T>(info) {
    Napi::Env env = info.Env();

    if(info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    _value = info[0].As<Napi::External<V>>().Data();
    _valuesref = Napi::Reference<Napi::Array>::New(Napi::Array::New(env));
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
Napi::Function BaseVector<T, V>::GetClass(Napi::Env env, Napi::Object exports, const char* name) {
    Napi::HandleScope scope(env);

    Napi::Function symbolFor =
        Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
    Napi::Symbol inspectSymbol =
        symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

    std::vector<Napi::ClassPropertyDescriptor<T>> properties = {
        BaseVector::InstanceMethod("toJSON", &BaseVector::ToObject),
        BaseVector::InstanceMethod(inspectSymbol, &BaseVector::ToObject),
        BaseVector::InstanceAccessor("device", &BaseVector::GetDevice, &BaseVector::SetDevice),
        BaseVector::InstanceAccessor("name", &BaseVector::GetName, &BaseVector::SetName),
        BaseVector::InstanceAccessor("label", &BaseVector::GetLabel, &BaseVector::SetLabel),
        BaseVector::InstanceAccessor("group", &BaseVector::GetGroup, &BaseVector::SetGroup),
        BaseVector::InstanceAccessor("state", &BaseVector::GetState, &BaseVector::SetState),
        BaseVector::InstanceAccessor("values", &BaseVector::GetValues, &BaseVector::SetValues)};

    Napi::Function func = BaseVector::DefineClass(env, name, properties);

    return func;
}

// Number vector

Napi::FunctionReference NumberVector::constructor;

NumberVector::NumberVector(const Napi::CallbackInfo& info) : BaseVector(info) {
    for(int i = 0; i < getHandle()->nnp; i++) {
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

// Switch vector

Napi::FunctionReference SwitchVector::constructor;

SwitchVector::SwitchVector(const Napi::CallbackInfo& info) : BaseVector(info) {
    for(int i = 0; i < getHandle()->nsp; i++) {
        Napi::Object value =
            SwitchValue::NewInstance(Napi::External<ISwitch>::New(info.Env(), getHandle()->sp + i));
        getValues().Set(i, value);
				getValues().Set(getHandle()->sp[i].name, value);
    }
}

Napi::Object SwitchVector::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

void SwitchVector::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = BaseVector::GetClass(env, exports, "SwitchVector");

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
}