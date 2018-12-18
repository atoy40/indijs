
#include "number.h"
#include "numbervalue.h"
#include <iostream>

Number::Number(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
  Napi::Env env = info.Env();

  if (info.Length() != 1)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return;
  }

  _number = info[0].As<Napi::External<INumberVectorProperty>>().Data();
  _values = Napi::Reference<Napi::Array>::New(Napi::Array::New(env));

  for (int i = 0; i < _number->nnp; i++) {
    Napi::Object value = NumberValue::NewInstance(Napi::External<INumber>::New(env, _number->np+i));
    _values.Value().Set(i, value);
  }
}

Napi::FunctionReference Number::constructor;

Napi::Value Number::GetName(const Napi::CallbackInfo &info)
{
  return Napi::String::New(info.Env(), _number->name);
}

void Number::SetName(const Napi::CallbackInfo &info, const Napi::Value &value)
{
}

Napi::Value Number::GetLabel(const Napi::CallbackInfo &info)
{
  return Napi::String::New(info.Env(), _number->label);
}

void Number::SetLabel(const Napi::CallbackInfo &info, const Napi::Value &value)
{
}

Napi::Value Number::GetValues(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  return _values.Value();
}

void Number::SetValues(const Napi::CallbackInfo &info, const Napi::Value &value)
{
}

Napi::Value Number::ToObject(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  Napi::Object propNames = Napi::Object::New(env);

  propNames.Set("name", _number->name);
  propNames.Set("label", _number->label);
  propNames.Set("device", _number->device);
  propNames.Set("values", _values.Value());

  return propNames;
}

void Number::GetClass(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  Napi::Function symbolFor = Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
  Napi::Symbol inspectSymbol = symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

  Napi::Function func = DefineClass(env, "Number", {
                                                       InstanceMethod("toJSON", &Number::ToObject),
                                                       InstanceMethod(inspectSymbol, &Number::ToObject),
                                                       InstanceAccessor("name", &Number::GetName, &Number::SetName),
                                                       InstanceAccessor("label", &Number::GetLabel, &Number::SetLabel),
                                                       InstanceAccessor("values", &Number::GetValues, &Number::SetValues),
                                                   });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
}

Napi::Object Number::NewInstance(Napi::Value arg)
{
  Napi::Object obj = constructor.New({arg});
  return obj;
}
