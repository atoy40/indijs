#pragma once

#include <libindi/baseclient.h>
#include <napi.h>
#include "numbervalue.h"
// #include<libindi / basedevice.h>

template<typename T, typename V>
class BaseVector : public Napi::ObjectWrap<T>
{
  public:
	BaseVector(const Napi::CallbackInfo&);
	Napi::Value ToObject(const Napi::CallbackInfo& info);

	Napi::Value GetName(const Napi::CallbackInfo& info);
	void SetName(const Napi::CallbackInfo& info, const Napi::Value& value);
	Napi::Value GetLabel(const Napi::CallbackInfo& info);
	void SetLabel(const Napi::CallbackInfo& info, const Napi::Value& value);
	Napi::Value GetValues(const Napi::CallbackInfo& info);
	void SetValues(const Napi::CallbackInfo& info, const Napi::Value& value);

	// virtual Napi::Value getValue(const Napi::Env &env) = 0;
	// virtual void setProps(Napi::Object &object) = 0;

  protected:
	V* getHandle()
	{
		return _value;
	}

	Napi::Array getValues()
	{
		return _valuesref.Value();
	}

  private:
	V* _value;
	Napi::Reference<Napi::Array> _valuesref;
};

class SwitchVector : public BaseVector<SwitchVector, ISwitchVectorProperty>
{
  public:
	SwitchVector(const Napi::CallbackInfo& info) : BaseVector(info)
	{
    Napi::Env env = info.Env();

		for(int i = 0; i < getHandle()->nsp; i++)
		{
			Napi::Object value =
				SwitchValue::NewInstance(Napi::External<ISwitch>::New(env, getHandle()->sp + i));
			getValues().Set(i, value);
		}
	}

	static Napi::Object NewInstance(Napi::Value arg);
	static void GetClass(Napi::Env env, Napi::Object exports);

  private:
	static Napi::FunctionReference constructor;
};