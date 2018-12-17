#include "indi.h"
#include "property.h"
#include "number.h"

Indi::Indi(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 2)
    {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "You need to set the server hostname")
            .ThrowAsJavaScriptException();
        return;
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "You need to set the server port")
            .ThrowAsJavaScriptException();
        return;
    }

    setServer(info[0].As<Napi::String>().Utf8Value().c_str(), info[1].As<Napi::Number>().Uint32Value());
}

Napi::Value Indi::Connect(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsFunction())
    {
        Napi::TypeError::New(env, "You need to provide a callback")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    _callback = std::make_shared<ThreadSafeCallback>(info[0].As<Napi::Function>());

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());
    Napi::Function cb = info[0].As<Napi::Function>();
    ConnectWorker *wk = new ConnectWorker(cb, deferred, this); // the cb is never call because this async simply resolve a promise.
    wk->Queue();

    return deferred.Promise();
}

Napi::Value Indi::Disconnect(const Napi::CallbackInfo &info)
{
    disconnectServer();
    _callback.reset();

    return info.Env().Undefined();
}

Napi::Value Indi::SendNewSwitch(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 3)
    {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "You need to provide a device name")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsString())
    {
        Napi::TypeError::New(env, "You need to provide a property name")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsString())
    {
        Napi::TypeError::New(env, "You need to provide an element name")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    sendNewSwitch(info[0].As<Napi::String>().Utf8Value().c_str(), info[1].As<Napi::String>().Utf8Value().c_str(), info[2].As<Napi::String>().Utf8Value().c_str());

    return info.Env().Undefined();
}

Napi::Function Indi::GetClass(Napi::Env env)
{
    return DefineClass(env, "Indi", {
                                        Indi::InstanceMethod("connect", &Indi::Connect),
                                        Indi::InstanceMethod("disconnect", &Indi::Disconnect),
                                        Indi::InstanceMethod("sendNewSwitch", &Indi::SendNewSwitch),
                                    });
}

void Indi::serverConnected()
{
    _callback->call([](Napi::Env env, std::vector<napi_value> &args) {
        args = {Napi::String::New(env, "connected")};
    });
}
void Indi::serverDisconnected(int exit_code)
{
    _callback->call([exit_code](Napi::Env env, std::vector<napi_value> &args) {
        args = {Napi::String::New(env, "disconnected"), Napi::Number::New(env, exit_code)};
    });
}

void Indi::newDevice(INDI::BaseDevice *dp)
{
    _callback->call([dp](Napi::Env env, std::vector<napi_value> &args) {
        Napi::Object dev = Device::NewInstance(Napi::External<INDI::BaseDevice>::New(env, dp));
        args = {Napi::String::New(env, "newDevice"), dev};
    });
}

void Indi::removeDevice(INDI::BaseDevice *dp)
{
    _callback->call([dp](Napi::Env env, std::vector<napi_value> &args) {
        Napi::Object dev = Device::NewInstance(Napi::External<INDI::BaseDevice>::New(env, dp));
        args = {Napi::String::New(env, "removeDevice"), dev};
    });
}

Napi::Object Indi::prop2obj(Napi::Env env, INDI::Property *property)
{
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("name", property->getName());
    obj.Set("type", Napi::Number::New(env, property->getType()));
    obj.Set("label", property->getLabel());
    obj.Set("groupName", property->getGroupName());
    obj.Set("deviceName", property->getDeviceName());

    return obj;
}

Napi::Object Indi::nvp2obj(Napi::Env env, INumberVectorProperty *nvp)
{
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("dimension", nvp->nnp);
    obj.Set("name", nvp->name);
    obj.Set("label", nvp->label);

    Napi::Object values = Napi::Object::New(env);
    for (int i = 0; i < nvp->nnp; i++)
    {
        Napi::Object v = Napi::Object::New(env);
        v.Set("label", nvp->np[i].label);
        v.Set("value", nvp->np[i].value);
        v.Set("min", nvp->np[i].min);
        v.Set("max", nvp->np[i].max);
        v.Set("step", nvp->np[i].step);
        values.Set(nvp->np[i].name, v);
    }
    obj.Set("values", values);

    return obj;
}

void Indi::newProperty(INDI::Property *property)
{
    _callback->call([this, property](Napi::Env env, std::vector<napi_value> &args) {
        // Napi::Object prop = Property::NewInstance(Napi::External<INDI::Property>::New(env, property));
        args = {Napi::String::New(env, "newProperty"), prop2obj(env, property)};
    });
};

void Indi::removeProperty(INDI::Property *property)
{
    std::string name(property->getName());
    std::string devname(property->getDeviceName());

    _callback->call([name, devname](Napi::Env env, std::vector<napi_value> &args) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("name", name);
        obj.Set("deviceName", devname);
        args = {Napi::String::New(env, "removeProperty"), obj};
    });
};

void Indi::newNumber(INumberVectorProperty *nvp)
{
    _callback->call([this, nvp](Napi::Env env, std::vector<napi_value> &args) {
        args = {Napi::String::New(env, "newNumber"), nvp2obj(env, nvp)};
    });
};

void Indi::newSwitch(ISwitchVectorProperty *svp)
{
    _callback->call([svp](Napi::Env env, std::vector<napi_value> &args) {
        //Napi::Object prop = Number::NewInstance(Napi::External<INumberVectorProperty>::New(env, nvp));

        Napi::Object pValue = Napi::Object::New(env);

        pValue.Set("dimension", svp->nsp);
        pValue.Set("name", svp->name);
        pValue.Set("label", svp->label);

        Napi::Object values = Napi::Object::New(env);
        for (int i = 0; i < svp->nsp; i++)
        {
            Napi::Object v = Napi::Object::New(env);
            v.Set("value", Napi::Boolean::New(env, svp->sp[i].s));
            v.Set("label", svp->sp[i].label);
            values.Set(svp->sp[i].name, v);
        }
        pValue.Set("values", values);

        args = {Napi::String::New(env, "newSwitch"), pValue};
    });
};

void Indi::ConnectWorker::Execute()
{
    if (!_client->connectServer())
    {
        SetError("Unable to connect");
    }
}

void Indi::ConnectWorker::OnOK()
{
    Napi::HandleScope scope(Env());
    _deferred.Resolve(Env().Undefined());
}

void Indi::ConnectWorker::OnError(const Napi::Error &e)
{
    Napi::HandleScope scope(Env());
    _deferred.Reject(e.Value());
    _client->reset();
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "Indi");
    exports.Set(name, Indi::GetClass(env));

    Device::GetClass(env, exports);
    Property::GetClass(env, exports);
    Number::GetClass(env, exports);

    return exports;
}

NODE_API_MODULE(addon, Init)
