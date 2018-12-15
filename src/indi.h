#pragma once

#include <iostream>
#include <napi.h>
#include <napi-thread-safe-callback.hpp>
#include <libindi/baseclient.h>
#include <libindi/basedevice.h>
#include "device.h"

class Indi : public Napi::ObjectWrap<Indi>, private INDI::BaseClient
{
  public:
    Indi(const Napi::CallbackInfo &);
    void reset() { _callback.reset(); };
    Napi::Value Connect(const Napi::CallbackInfo &);
    Napi::Value Disconnect(const Napi::CallbackInfo &);
    static Napi::Function GetClass(Napi::Env);

  private:

    class ConnectWorker;

    // INDI::BaseClient implementation.
    void newDevice(INDI::BaseDevice *dp);

    void removeDevice(INDI::BaseDevice *dp) { };
    void newProperty(INDI::Property *property);
    void removeProperty(INDI::Property *property) { };
    void newBLOB(IBLOB *bp) { };
    void newSwitch(ISwitchVectorProperty *svp) { };
    void newNumber(INumberVectorProperty *nvp) { };
    void newText(ITextVectorProperty *tvp) { };
    void newLight(ILightVectorProperty *lvp) { };
    void newMessage(INDI::BaseDevice *dp, int messageID) { };
    void serverConnected();
    void serverDisconnected(int exit_code);

    std::shared_ptr<ThreadSafeCallback> _callback;
};

class Indi::ConnectWorker : public Napi::AsyncWorker {
  public:
    ConnectWorker(Napi::Function& callback, Napi::Promise::Deferred& deferred, Indi *client) : Napi::AsyncWorker(callback), _deferred(deferred), _client(client) {};

    void Execute();
    void OnOK();
    void OnError(const Napi::Error& e);

  private:
    Napi::Promise::Deferred _deferred;
    Indi *_client;
    bool _result;
};
