
#include "numbervalue.h"

NumberValue::NumberValue(const Napi::CallbackInfo &info) : BaseValue(info)
{
  Napi::Env env = info.Env();

  if (info.Length() != 1)
  {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return;
  }

  _number = info[0].As<Napi::External<INumber>>().Data();
}

Napi::FunctionReference NumberValue::constructor;

void NumberValue::GetClass(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  Napi::Function symbolFor = Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
  Napi::Symbol inspectSymbol = symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

  Napi::Function func = DefineClass(env, "Number", {
                                                       InstanceMethod("toJSON", &NumberValue::ToObject),
                                                       InstanceMethod(inspectSymbol, &NumberValue::ToObject),
                                                       InstanceAccessor("name", &NumberValue::GetName, &NumberValue::SetName),
                                                       InstanceAccessor("label", &NumberValue::GetLabel, &NumberValue::SetLabel),
                                                       InstanceAccessor("value", &NumberValue::GetValue, &NumberValue::SetValue),
                                                   });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
}

Napi::Object NumberValue::NewInstance(Napi::Value arg)
{
  Napi::Object obj = constructor.New({arg});
  return obj;
}
