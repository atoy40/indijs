#pragma once

#include <napi.h>
#include <libindi/baseclient.h>
#include <libindi/basedevice.h>

template <typename T, typename V>
class BaseValue : public Napi::ObjectWrap<T>
{
public:
  BaseValue(const Napi::CallbackInfo &);
  virtual Napi::Value ToObject(const Napi::CallbackInfo &info);

  Napi::Value GetName(const Napi::CallbackInfo &info);
  void SetName(const Napi::CallbackInfo &info, const Napi::Value &value);
  Napi::Value GetLabel(const Napi::CallbackInfo &info);
  void SetLabel(const Napi::CallbackInfo &info, const Napi::Value &value);
  Napi::Value GetValue(const Napi::CallbackInfo &info);
  void SetValue(const Napi::CallbackInfo &info, const Napi::Value &value);

  virtual Napi::Value getValue(const Napi::Env &env) = 0;
  virtual void setValue(const Napi::Env &env, const Napi::Value &value) = 0;
  virtual void setProps(Napi::Object &object) = 0;

protected:
  V* getHandle() {
    return _value;
  }

private:
  V *_value;
  //static Napi::FunctionReference constructor;
};

template <typename T, typename V>
BaseValue<T, V>::BaseValue(const Napi::CallbackInfo &info) : Napi::ObjectWrap<T>(info)
{
  Napi::Env env = info.Env();

  if (info.Length() != 1)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return;
  }

  _value = info[0].As<Napi::External<V>>().Data();
}

template <typename T, typename V>
Napi::Value BaseValue<T, V>::ToObject(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  Napi::Object propNames = Napi::Object::New(env);

  propNames.Set("name", _value->name);
  propNames.Set("label", _value->label);
  propNames.Set("value", getValue(env));

  setProps(propNames);

  return propNames;
}

template <typename T, typename V>
Napi::Value BaseValue<T, V>::GetName(const Napi::CallbackInfo &info)
{
  return Napi::String::New(info.Env(), _value->name);
}

template <typename T, typename V>
void BaseValue<T, V>::SetName(const Napi::CallbackInfo &info, const Napi::Value &value)
{
}

template <typename T, typename V>
Napi::Value BaseValue<T, V>::GetLabel(const Napi::CallbackInfo &info)
{
  return Napi::String::New(info.Env(), _value->label);
}

template <typename T, typename V>
void BaseValue<T, V>::SetLabel(const Napi::CallbackInfo &info, const Napi::Value &value)
{
}

template <typename T, typename V>
Napi::Value BaseValue<T, V>::GetValue(const Napi::CallbackInfo &info)
{
  return getValue(info.Env());
}

template <typename T, typename V>
void BaseValue<T, V>::SetValue(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  setValue(info.Env(), value);
}