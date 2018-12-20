
#include "numbervalue.h"

NumberValue::NumberValue(const Napi::CallbackInfo& info) : BaseValue(info) {}

Napi::FunctionReference NumberValue::constructor;

void NumberValue::GetClass(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function symbolFor =
		Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
	Napi::Symbol inspectSymbol =
		symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

	Napi::Function func =
		DefineClass(env, "NumberValue",
					{
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

// SwitchValue
SwitchValue::SwitchValue(const Napi::CallbackInfo& info) : BaseValue(info) {}

Napi::FunctionReference SwitchValue::constructor;

void SwitchValue::GetClass(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function symbolFor =
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
	constructor.SuppressDestruct();
}

Napi::Object SwitchValue::NewInstance(Napi::Value arg)
{
	Napi::Object obj = constructor.New({arg});
	return obj;
}
