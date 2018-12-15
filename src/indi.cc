#include "indi.h"
#include "property.h"

using namespace Napi;

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

    setServer(info[0].As<String>().Utf8Value().c_str(), info[1].As<Number>().Uint32Value());
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

    _callback = std::make_shared<ThreadSafeCallback>(info[0].As<Function>());

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());
    Napi::Function cb = info[0].As<Function>();
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

Napi::Function Indi::GetClass(Napi::Env env)
{
    return DefineClass(env, "Indi", {
                                        Indi::InstanceMethod("connect", &Indi::Connect),
                                        Indi::InstanceMethod("disconnect", &Indi::Disconnect),
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
        args = {Napi::String::New(env, "device"), dev};
    });
}

void Indi::newProperty(INDI::Property *property)
{
    _callback->call([property](Napi::Env env, std::vector<napi_value> &args) {
        Napi::Object prop = Property::NewInstance(Napi::External<INDI::Property>::New(env, property));
        args = {Napi::String::New(env, "property"), prop};
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
    HandleScope scope(Env());
    _deferred.Resolve(Env().Undefined());
}

void Indi::ConnectWorker::OnError(const Napi::Error &e)
{
    HandleScope scope(Env());
    _deferred.Reject(e.Value());
    _client->reset();
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "Indi");
    exports.Set(name, Indi::GetClass(env));

    Device::GetClass(env, exports);
    Property::GetClass(env, exports);

    return exports;
}

NODE_API_MODULE(addon, Init)
