
#include "number.h"

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
}

Napi::FunctionReference Number::constructor;

Napi::Value Number::GetName(const Napi::CallbackInfo &info)
{
  return Napi::String::New(info.Env(), _number->name);
}

void Number::SetName(const Napi::CallbackInfo &info, const Napi::Value &value)
{
}

void Number::GetClass(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "Number", {
                                                       InstanceAccessor("name", &Number::GetName, &Number::SetName),
                                                   });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
}

Napi::Object Number::NewInstance(Napi::Value arg)
{
  Napi::Object obj = constructor.New({arg});
  return obj;
}
