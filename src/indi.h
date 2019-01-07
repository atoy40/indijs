#pragma once

#include "device.h"
#include "vector.h"
#include <iostream>
#include <libindi/baseclient.h>
#include <libindi/basedevice.h>
#include <napi-thread-safe-callback.hpp>
#include <napi.h>

class Indi : public Napi::ObjectWrap<Indi>, public INDI::BaseClient {
  public:
    Indi(const Napi::CallbackInfo&);

    Napi::Value Connect(const Napi::CallbackInfo&);
    Napi::Value Disconnect(const Napi::CallbackInfo&);
    Napi::Value WatchDevice(const Napi::CallbackInfo&);
    Napi::Value GetDevice(const Napi::CallbackInfo&);
    Napi::Value SendNewSwitch(const Napi::CallbackInfo&);
    Napi::Value SendNewNumber(const Napi::CallbackInfo&);

    void reset() {
        _callback.reset();
    };

    static Napi::Function GetClass(Napi::Env);

  private:
    class ConnectWorker;

    // INDI::BaseClient implementation.
    void newDevice(INDI::BaseDevice* dp);
    void newProperty(INDI::Property* property);
    void newSwitch(ISwitchVectorProperty* svp);
    void newNumber(INumberVectorProperty* nvp);
    void serverConnected();
    void serverDisconnected(int exit_code);
    void removeDevice(INDI::BaseDevice* dp);
    void removeProperty(INDI::Property* property);

    void newBLOB(IBLOB* bp){};
    void newText(ITextVectorProperty* tvp){};
    void newLight(ILightVectorProperty* lvp){};
    void newMessage(INDI::BaseDevice* dp, int messageID){};

    // helper
    Napi::Object prop2obj(Napi::Env env, INDI::Property* property);
    Napi::Object nvp2obj(Napi::Env env, INumberVectorProperty* nvp);

    std::shared_ptr<ThreadSafeCallback> _callback;
};

class Indi::ConnectWorker : public Napi::AsyncWorker {
  public:
    ConnectWorker(Napi::Function& callback, Napi::Promise::Deferred& deferred, Indi* client)
        : Napi::AsyncWorker(callback), _deferred(deferred), _client(client){};

    void Execute();
    void OnOK();
    void OnError(const Napi::Error& e);

  private:
    Napi::Promise::Deferred _deferred;
    Indi* _client;
};

template<typename V>
class SendNewValueWorker : public Napi::AsyncWorker {
  public:
    SendNewValueWorker(Napi::Function& callback, Napi::Promise::Deferred& deferred, Indi* client,
                       V* vector)
        : Napi::AsyncWorker(callback), _deferred(deferred), _client(client), _vector(vector){};

    // virtual void Execute() = 0;
    void OnOK() {
        Napi::HandleScope scope(Env());
        _deferred.Resolve(Env().Undefined());
    };
    void OnError(const Napi::Error& e) {
        Napi::HandleScope scope(Env());
        _deferred.Reject(Env().Undefined());
    };

  protected:
    Napi::Promise::Deferred _deferred;
    Indi* _client;
    V* _vector;
};

class SendNewNumberWorker : public SendNewValueWorker<INumberVectorProperty> {
  public:
    SendNewNumberWorker(Napi::Function& callback, Napi::Promise::Deferred& deferred, Indi* client,
                        INumberVectorProperty* vector)
        : SendNewValueWorker(callback, deferred, client, vector) {}

    void Execute() {
        _client->sendNewNumber(_vector);
    }
};

class SendNewSwitchWorker : public SendNewValueWorker<ISwitchVectorProperty> {
  public:
    SendNewSwitchWorker(Napi::Function& callback, Napi::Promise::Deferred& deferred, Indi* client,
                        ISwitchVectorProperty* vector)
        : SendNewValueWorker(callback, deferred, client, vector) {}

    void Execute() {
        _client->sendNewSwitch(_vector);
    }
};