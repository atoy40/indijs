#include "vector.h"

// Base vector

template<typename T, typename V>
BaseVector<T, V>::BaseVector(const Napi::CallbackInfo& info) : Napi::ObjectWrap<T>(info)
{
	Napi::Env env = info.Env();

	if(info.Length() != 1)
	{
		Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
		return;
	}

	_value = info[0].As<Napi::External<V>>().Data();
  _valuesref = Napi::Reference<Napi::Array>::New(Napi::Array::New(env));
}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::ToObject(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	Napi::Object props = Napi::Object::New(env);

	props.Set("name", _value->name);
	props.Set("label", _value->label);
	props.Set("values", _valuesref.Value());
  props.Set("state", Napi::Number::New(env, _value->s));

	// setProps(props);

	return props;
}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetName(const Napi::CallbackInfo& info)
{
	return Napi::String::New(info.Env(), _value->name);
}

template<typename T, typename V>
void BaseVector<T, V>::SetName(const Napi::CallbackInfo& info, const Napi::Value& value)
{
}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetLabel(const Napi::CallbackInfo& info)
{
	return Napi::String::New(info.Env(), _value->label);
}

template<typename T, typename V>
void BaseVector<T, V>::SetLabel(const Napi::CallbackInfo& info, const Napi::Value& value)
{
}

template<typename T, typename V>
Napi::Value BaseVector<T, V>::GetValues(const Napi::CallbackInfo& info)
{
	return _valuesref.Value();
}

template<typename T, typename V>
void BaseVector<T, V>::SetValues(const Napi::CallbackInfo& info, const Napi::Value& value)
{
}

// Switch vector

Napi::FunctionReference SwitchVector::constructor;

Napi::Object SwitchVector::NewInstance(Napi::Value arg)
{
	Napi::Object obj = constructor.New({arg});
	return obj;
}

void SwitchVector::GetClass(Napi::Env env, Napi::Object exports)
{
	Napi::HandleScope scope(env);

	Napi::Function symbolFor =
		Napi::Env(env).Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>();
	Napi::Symbol inspectSymbol =
		symbolFor.Call({Napi::String::New(env, "nodejs.util.inspect.custom")}).As<Napi::Symbol>();

	Napi::Function func =
		DefineClass(env, "SwitchVector",
					{InstanceMethod("toJSON", &SwitchVector::ToObject),
					 InstanceMethod(inspectSymbol, &SwitchVector::ToObject),
					 InstanceAccessor("name", &SwitchVector::GetName, &SwitchVector::SetName),
					 InstanceAccessor("label", &SwitchVector::GetLabel, &SwitchVector::SetLabel),
           InstanceAccessor("values", &SwitchVector::GetValues, &SwitchVector::SetValues)});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();
}
