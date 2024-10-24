#include "client.h"
#include "promise.h"
#include "prop.h"

#ifdef DEBUG
#define log(msg) std::cout << msg << std::endl;
#else
#define log(msg)
#endif

Napi::FunctionReference IndiClient::constructor;

IndiClient::IndiClient(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    Napi::Env env = info.Env();

    if (info.Length() != 3) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to set the server hostname").ThrowAsJavaScriptException();
        return;
    }

    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "You need to set the server port").ThrowAsJavaScriptException();
        return;
    }

    if (!info[2].IsFunction()) {
        Napi::TypeError::New(env, "You need to set the event emitter").ThrowAsJavaScriptException();
        return;
    }

    _tsfn = Napi::ThreadSafeFunction::New(env, info[2].As<Napi::Function>(), "indi thread",
        1, // one callback in the queue to avoid multiple dev/prop removes in the same loop
        1 // Only one thread will use this initially
    );

    setServer(info[0].As<Napi::String>().Utf8Value().c_str(), info[1].As<Napi::Number>().Uint32Value());
}

Napi::Value IndiClient::SetServer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to provide a host name").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "You need to provide a port").ThrowAsJavaScriptException();
        return env.Null();
    }

    setServer(info[0].As<Napi::String>().Utf8Value().c_str(), info[1].As<Napi::Number>().Uint32Value());

    return info.Env().Undefined();
}

Napi::Value IndiClient::GetHost(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, getHost());
}

Napi::Value IndiClient::GetPort(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, getPort());
}

Napi::Value IndiClient::Connect(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    ConnectWorker* wk = new ConnectWorker(env, this);
    wk->Queue();

    return wk->Deferred().Promise();
}

Napi::Value IndiClient::Disconnect(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    DisconnectWorker* wk = new DisconnectWorker(env, this);
    wk->Queue();

    return wk->Deferred().Promise();
}

Napi::Value IndiClient::IsConnected(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() > 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Boolean::New(env, isServerConnected());
}

Napi::Value IndiClient::WatchDevice(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to provide a device name").ThrowAsJavaScriptException();
        return env.Null();
    }

    watchDevice(info[0].As<Napi::String>().Utf8Value().c_str());

    return info.Env().Undefined();
}

Napi::Value IndiClient::SetBLOBMode(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "You need to provide a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsString()) {
        Napi::TypeError::New(env, "You need to provide a device name").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() == 3 && !info[2].IsString()) {
        Napi::TypeError::New(env, "You need to provide a property name").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() == 3) {
        setBLOBMode((BLOBHandling)info[0].As<Napi::Number>().Uint32Value(),
            info[1].As<Napi::String>().Utf8Value().c_str(), info[2].As<Napi::String>().Utf8Value().c_str());
    } else {
        setBLOBMode(
            (BLOBHandling)info[0].As<Napi::Number>().Uint32Value(), info[1].As<Napi::String>().Utf8Value().c_str());
    }

    return info.Env().Undefined();
}

Napi::Value IndiClient::GetDevice(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to provide a device name").ThrowAsJavaScriptException();
        return env.Null();
    }

    INDI::BaseDevice dp = getDevice(info[0].As<Napi::String>().Utf8Value().c_str());

    if (dp) {
        return Device::NewInstance(Napi::External<INDI::BaseDevice>::New(
            env, new INDI::BaseDevice(dp), [](Napi::Env env, INDI::BaseDevice* dp) { delete dp; }));
    }

    return info.Env().Undefined();
}

Napi::Value IndiClient::GetDevices(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::vector<INDI::BaseDevice> devices = getDevices();

    Napi::Array array = Napi::Array::New(env);

    if (!isServerConnected()) {
        return array;
    }

    for (auto itr = devices.begin(); itr != devices.end(); itr++) {
        array.Set(std::distance(devices.begin(), itr),
            Device::NewInstance(Napi::External<INDI::BaseDevice>::New(
                env, new INDI::BaseDevice(*itr), [](Napi::Env env, INDI::BaseDevice* dp) { delete dp; })));
    }

    return array;
}

Napi::Value IndiClient::ConnectDevice(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to provide a device name").ThrowAsJavaScriptException();
        return env.Null();
    }

    ConnectDeviceWorker* wk = new ConnectDeviceWorker(env, this, info[0].As<Napi::String>().Utf8Value());
    wk->Queue();

    return wk->Deferred().Promise();
}

Napi::Value IndiClient::SendNewNumber(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    SendNewNumberWorker* wk;

    if (info.Length() == 1 && info[0].IsObject()) {
        if (!info[0].IsObject()) {
            Napi::TypeError::New(env, "You need to provide a property object").ThrowAsJavaScriptException();
            return env.Null();
        }
        PropertyNumber* sv = Napi::ObjectWrap<PropertyNumber>::Unwrap(info[0].As<Napi::Object>());
        wk = new SendNewNumberWorker(env, deferred, this, sv->getProperty());
    } else if (info.Length() == 4) {
        if (!info[0].IsString()) {
            Napi::TypeError::New(env, "You need to provide a device name").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsString()) {
            Napi::TypeError::New(env, "You need to provide a property name").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsString()) {
            Napi::TypeError::New(env, "You need to provide an element name").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[3].IsNumber()) {
            Napi::TypeError::New(env, "You need to provide a value").ThrowAsJavaScriptException();
            return env.Null();
        }

        wk = new SendNewNumberWorker(env, deferred, this, info[0].As<Napi::String>().Utf8Value(),
            info[1].As<Napi::String>().Utf8Value(), info[2].As<Napi::String>().Utf8Value(),
            info[3].As<Napi::Number>().DoubleValue());
    } else {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    wk->Queue();
    return deferred.Promise();
}

Napi::Value IndiClient::SendNewSwitch(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());
    SendNewSwitchWorker* wk;

    if (info.Length() == 1 && info[0].IsObject()) {
        if (!info[0].IsObject()) {
            Napi::TypeError::New(env, "You need to provide a property object").ThrowAsJavaScriptException();
            return env.Null();
        }
        PropertySwitch* sv = Napi::ObjectWrap<PropertySwitch>::Unwrap(info[0].As<Napi::Object>());
        wk = new SendNewSwitchWorker(env, deferred, this, sv->getProperty());
    } else if (info.Length() == 3) {
        if (!info[0].IsString()) {
            Napi::TypeError::New(env, "You need to provide a device name").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsString()) {
            Napi::TypeError::New(env, "You need to provide a property name").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsString()) {
            Napi::TypeError::New(env, "You need to provide an element name").ThrowAsJavaScriptException();
            return env.Null();
        }

        wk = new SendNewSwitchWorker(env, deferred, this, info[0].As<Napi::String>().Utf8Value(),
            info[1].As<Napi::String>().Utf8Value(), info[2].As<Napi::String>().Utf8Value());
    } else {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    wk->Queue();
    return deferred.Promise();
}

Napi::Value IndiClient::SendNewText(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    SendNewTextWorker* wk;

    if (info.Length() == 1 && info[0].IsObject()) {
        if (!info[0].IsObject()) {
            Napi::TypeError::New(env, "You need to provide a property object").ThrowAsJavaScriptException();
            return env.Null();
        }
        PropertyText* sv = Napi::ObjectWrap<PropertyText>::Unwrap(info[0].As<Napi::Object>());
        wk = new SendNewTextWorker(env, deferred, this, sv->getProperty());
    } else if (info.Length() == 4) {
        if (!info[0].IsString()) {
            Napi::TypeError::New(env, "You need to provide a device name").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsString()) {
            Napi::TypeError::New(env, "You need to provide a property name").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsString()) {
            Napi::TypeError::New(env, "You need to provide an element name").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[3].IsString()) {
            Napi::TypeError::New(env, "You need to provide a value").ThrowAsJavaScriptException();
            return env.Null();
        }

        wk = new SendNewTextWorker(env, deferred, this, info[0].As<Napi::String>().Utf8Value(),
            info[1].As<Napi::String>().Utf8Value(), info[2].As<Napi::String>().Utf8Value(),
            info[3].As<Napi::String>().Utf8Value());
    } else {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    wk->Queue();
    return deferred.Promise();
}

void IndiClient::GetClass(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "IndiClient",
        {
            InstanceAccessor<&IndiClient::GetHost>("host"),
            InstanceAccessor<&IndiClient::GetPort>("port"),
            InstanceAccessor<&IndiClient::IsConnected>("connected"),
            InstanceMethod<&IndiClient::SetServer>("setServer"),
            InstanceMethod<&IndiClient::Connect>("connect"),
            InstanceMethod<&IndiClient::Disconnect>("disconnect"),
            InstanceMethod<&IndiClient::WatchDevice>("watchDevice"),
            InstanceMethod<&IndiClient::SetBLOBMode>("setBLOBMode"),
            InstanceMethod<&IndiClient::GetDevice>("getDevice"),
            InstanceMethod<&IndiClient::GetDevices>("getDevices"),
            InstanceMethod<&IndiClient::ConnectDevice>("connectDevice"),
            InstanceMethod<&IndiClient::SendNewNumber>("sendNewNumber"),
            InstanceMethod<&IndiClient::SendNewSwitch>("sendNewSwitch"),
            InstanceMethod<&IndiClient::SendNewText>("sendNewText"),
        });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("IndiClient", func);
}

Napi::Object IndiClient::NewInstance(Napi::Value arg) {
    Napi::Object obj = constructor.New({arg});
    return obj;
}

// INDI mediator implementation
void IndiClient::serverConnected() {
    log("CONNECTED");

    auto callback = [](Napi::Env env, Napi::Function jsCallback) {
        jsCallback.Call({Napi::String::New(env, "connected")});
    };

    _tsfn.BlockingCall(callback);
}

void IndiClient::serverDisconnected(int exit_code) {
    log("DISCONNECTED");

    int* value = new int(exit_code);
    auto callback = [](Napi::Env env, Napi::Function jsCallback, int* exit_code) {
        jsCallback.Call({Napi::String::New(env, "disconnected"), Napi::Number::New(env, *exit_code)});
        delete exit_code;
    };

    _tsfn.BlockingCall(value, callback);

    // reset();
}

void IndiClient::newDevice(INDI::BaseDevice device) {
    log("NEW DEVICE");

    auto callback = [device](Napi::Env env, Napi::Function jsCallback) {
        Napi::Object dev = Device::NewInstance(Napi::External<INDI::BaseDevice>::New(
            env, new INDI::BaseDevice(device), [](Napi::Env env, INDI::BaseDevice* dp) { delete dp; }));
        jsCallback.Call({Napi::String::New(env, "newDevice"), dev});
    };

    _tsfn.BlockingCall(callback);
}

void IndiClient::removeDevice(INDI::BaseDevice device) {
    log("REMOVE DEVICE");

    std::string name(device.getDeviceName());

    auto callback = [name](Napi::Env env, Napi::Function jsCallback) {
        jsCallback.Call({Napi::String::New(env, "removeDevice"), Napi::String::New(env, name)});
    };

    _tsfn.BlockingCall(callback);
}

void IndiClient::newProperty(INDI::Property property) {
    log("NEW PROPS");

    auto callback = [property](Napi::Env env, Napi::Function jsCallback) {
        auto prop = getTypedProperty(env, property);
        jsCallback.Call({Napi::String::New(env, "newProperty"), prop});
    };

    _tsfn.BlockingCall(callback);
}

void IndiClient::updateProperty(INDI::Property property) {
    log("UPDATE PROPS");

    auto callback = [property](Napi::Env env, Napi::Function jsCallback) {
        auto prop = getTypedProperty(env, property);
        jsCallback.Call({Napi::String::New(env, "updateProperty"), prop});
    };

    _tsfn.BlockingCall(callback);
}

void IndiClient::removeProperty(INDI::Property property) {
    log("REMOVE PROPS");

    std::string name(property.getName());
    std::string devname(property.getDeviceName());

    auto callback = [name, devname](Napi::Env env, Napi::Function jsCallback) {
        jsCallback.Call(
            {Napi::String::New(env, "removeProperty"), Napi::String::New(env, name), Napi::String::New(env, devname)});
    };

    _tsfn.BlockingCall(callback);
}

void IndiClient::newMessage(INDI::BaseDevice device, int messageID) {
    log("NEW MSG");

    auto callback = [device, messageID](Napi::Env env, Napi::Function jsCallback) {
        Napi::Object dev = Device::NewInstance(Napi::External<INDI::BaseDevice>::New(
            env, new INDI::BaseDevice(device), [](Napi::Env env, INDI::BaseDevice* dp) { delete dp; }));
        jsCallback.Call({Napi::String::New(env, "newMessage"), dev, Napi::Number::New(env, messageID)});
    };

    _tsfn.BlockingCall(callback);
}

/*
void IndiClient::newNumber(INumberVectorProperty* nvp) {
    log("NEW NUMBER");

    std::cout << "new number" << std::endl;

    auto callback = [nvp](Napi::Env env, Napi::Function jsCallback) {
        Napi::Object prop =
            NumberVector::NewInstance(Napi::External<INumberVectorProperty>::New(env, nvp));
        jsCallback.Call({Napi::String::New(env, "newNumber"), prop});
    };

    _tsfn.BlockingCall(callback);
};

void IndiClient::newSwitch(ISwitchVectorProperty* svp) {
    log("NEW SWITCH");

    auto callback = [svp](Napi::Env env, Napi::Function jsCallback) {
        Napi::Object prop =
            SwitchVector::NewInstance(Napi::External<ISwitchVectorProperty>::New(env, svp));
        jsCallback.Call({Napi::String::New(env, "newSwitch"), prop});
    };

    _tsfn.BlockingCall(callback);
};

void IndiClient::newText(ITextVectorProperty* tvp) {
    log("NEW TEXT");

    auto callback = [tvp](Napi::Env env, Napi::Function jsCallback) {
        Napi::Object prop =
            TextVector::NewInstance(Napi::External<ITextVectorProperty>::New(env, tvp));
        jsCallback.Call({Napi::String::New(env, "newText"), prop});
    };

    _tsfn.BlockingCall(callback);
};

void IndiClient::newLight(ILightVectorProperty* lvp) {
    log("NEW LIGHT");

    auto callback = [lvp](Napi::Env env, Napi::Function jsCallback) {
        Napi::Object prop =
            LightVector::NewInstance(Napi::External<ILightVectorProperty>::New(env, lvp));
        jsCallback.Call({Napi::String::New(env, "newLight"), prop});
    };

    _tsfn.BlockingCall(callback);
};

void IndiClient::newBLOB(IBLOB* bp) {
    log("NEW BLOB");

    auto callback = [bp](Napi::Env env, Napi::Function jsCallback) {
        Napi::Object blob = BLOBValue::NewInstance(Napi::External<IBLOB>::New(env, bp));
        jsCallback.Call({Napi::String::New(env, "newBLOB"), blob});
    };

    _tsfn.BlockingCall(callback);
}
*/

void IndiClient::ConnectWorker::Execute() {
    _result = _client->connectServer();
    if (!_result) {
        SetError("Unable to connect");
    }
}

Napi::Value IndiClient::ConnectWorker::GetResolve() {
    return Napi::Boolean::New(Env(), _result);
}

Napi::Value IndiClient::ConnectWorker::GetReject(const Napi::Error& e) {
    _client->reset();
    return PromiseWorker::GetReject(e);
}

void IndiClient::DisconnectWorker::Execute() {
    _result = _client->disconnectServer();
    if (!_result) {
        SetError("Unable to disconnect");
    }
}

Napi::Value IndiClient::DisconnectWorker::GetResolve() {
    return Napi::Boolean::New(Env(), _result);
}

Napi::Value IndiClient::DisconnectWorker::GetReject(const Napi::Error& e) {
    _client->reset();
    return PromiseWorker::GetReject(e);
}

void IndiClient::ConnectDeviceWorker::Execute() {
    _client->connectDevice(_devname.c_str());
}
