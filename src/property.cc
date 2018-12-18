
#include "property.h"

using namespace Napi;

Property::Property(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
  Napi::Env env = info.Env();

  if (info.Length() != 1)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return;
  }

  _property = info[0].As<Napi::External<INDI::Property>>().Data();
}

Napi::FunctionReference Property::constructor;

Napi::Value Property::GetName(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() > 0)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::String::New(env, _property->getName());
}

Napi::Value Property::GetType(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() > 0)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::Number::New(env, _property->getType());
}

Napi::Value Property::GetLabel(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() > 0)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::String::New(env, _property->getLabel());
}

Napi::Value Property::GetGroupName(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() > 0)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::String::New(env, _property->getGroupName());
}

Napi::Value Property::GetDeviceName(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() > 0)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::String::New(env, _property->getDeviceName());
}

Napi::Value Property::GetTimestamp(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() > 0)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::String::New(env, _property->getTimestamp());
}

Napi::Value Property::GetOwnPropertyNames(const Napi::CallbackInfo &info)
{
  Napi::Array propNames = Napi::Array::New(info.Env());
  propNames.Set((uint32_t)0, "name");

  return propNames;
}

void Property::GetClass(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "Property", {
                                                         InstanceMethod("keys", &Property::GetOwnPropertyNames),
                                                         Property::InstanceMethod("getName", &Property::GetName),
                                                         Property::InstanceMethod("getType", &Property::GetType),
                                                         Property::InstanceMethod("getLabel", &Property::GetLabel),
                                                         Property::InstanceMethod("getGroupName", &Property::GetGroupName),
                                                         Property::InstanceMethod("getDeviceName", &Property::GetDeviceName),
                                                         Property::InstanceMethod("getTimestamp", &Property::GetTimestamp),
                                                     });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
}

Napi::Object Property::NewInstance(Napi::Value arg)
{
  Napi::Object obj = constructor.New({arg});
  return obj;
}
