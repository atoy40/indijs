#pragma once

#include "device.h"
#include "promise.h"
#include <iostream>
#include <libindi/baseclient.h>
#include <libindi/basedevice.h>
#include <napi.h>

class IndiClient : public Napi::ObjectWrap<IndiClient>, public INDI::BaseClient {
  public:
    IndiClient(const Napi::CallbackInfo&);

    Napi::Value SetServer(const Napi::CallbackInfo&);
    Napi::Value GetHost(const Napi::CallbackInfo&);
    Napi::Value GetPort(const Napi::CallbackInfo&);
    Napi::Value Connect(const Napi::CallbackInfo&);
    Napi::Value Disconnect(const Napi::CallbackInfo&);
    Napi::Value IsConnected(const Napi::CallbackInfo&);
    Napi::Value WatchDevice(const Napi::CallbackInfo&);
    Napi::Value SetBLOBMode(const Napi::CallbackInfo&);
    Napi::Value GetDevice(const Napi::CallbackInfo&);
    Napi::Value GetDevices(const Napi::CallbackInfo&);
    Napi::Value ConnectDevice(const Napi::CallbackInfo&);
    Napi::Value SendNewNumber(const Napi::CallbackInfo&);
    Napi::Value SendNewSwitch(const Napi::CallbackInfo&);
    Napi::Value SendNewText(const Napi::CallbackInfo&);

    void reset() {
        _tsfn.Release();
    };

    static void GetClass(Napi::Env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Value arg);

  private:
    // async workers
    class ConnectWorker;
    class DisconnectWorker;
    class ConnectDeviceWorker;
    class SendNewNumberWorker;
    class SendNewSwitchWorker;
    class SendNewTextWorker;

    // INDI::BaseClient implementation.
    void serverConnected() override;
    void serverDisconnected(int exit_code) override;
    void newDevice(INDI::BaseDevice device) override;
    void newProperty(INDI::Property property) override;
    void updateProperty(INDI::Property property) override;
    void removeDevice(INDI::BaseDevice device) override;
    void removeProperty(INDI::Property property) override;
    void newMessage(INDI::BaseDevice device, int messageID) override;
    /*void newSwitch(ISwitchVectorProperty* svp);
    void newNumber(INumberVectorProperty* nvp);
    void newText(ITextVectorProperty* tvp);
    void newLight(ILightVectorProperty* lvp);
    void newBLOB(IBLOB* bp);*/

    Napi::ThreadSafeFunction _tsfn;
    static Napi::FunctionReference constructor;
};

class IndiClient::ConnectWorker : public PromiseWorker {
  public:
    ConnectWorker(Napi::Env env, IndiClient* client) : PromiseWorker(env), _client(client) {};

    void Execute();
    Napi::Value GetResolve();
    Napi::Value GetReject(const Napi::Error& e);

  private:
    bool _result;
    IndiClient* _client;
};

class IndiClient::DisconnectWorker : public PromiseWorker {
  public:
    DisconnectWorker(Napi::Env env, IndiClient* client) : PromiseWorker(env), _client(client) {};

    void Execute();
    Napi::Value GetResolve();
    Napi::Value GetReject(const Napi::Error& e);

  private:
    bool _result;
    IndiClient* _client;
};

class IndiClient::ConnectDeviceWorker : public PromiseWorker {
  public:
    ConnectDeviceWorker(Napi::Env env, INDI::BaseClient* client, std::string devname)
        : PromiseWorker(env), _client(client), _devname(devname) {};
    void Execute();

  private:
    INDI::BaseClient* _client;
    std::string _devname;
};

template<typename V>
class SendNewValueWorker : public Napi::AsyncWorker {
  public:
    SendNewValueWorker(Napi::Env env, Napi::Promise::Deferred& deferred, INDI::BaseClient* client, INDI::Property prop)
        : Napi::AsyncWorker(env), _deferred(deferred), _client(client), _prop(prop) {};
    SendNewValueWorker(Napi::Env env, Napi::Promise::Deferred& deferred, INDI::BaseClient* client, std::string device,
        std::string property, std::string element)
        : Napi::AsyncWorker(env), _deferred(deferred), _client(client), _device(device), _property(property),
          _element(element) {};

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
    INDI::BaseClient* _client;
    INDI::Property _prop;
    std::string _device;
    std::string _property;
    std::string _element;
};

class IndiClient::SendNewNumberWorker : public SendNewValueWorker<INumberVectorProperty> {
  public:
    SendNewNumberWorker(
        Napi::Env env, Napi::Promise::Deferred& deferred, INDI::BaseClient* client, INDI::Property prop)
        : SendNewValueWorker(env, deferred, client, prop) {};
    SendNewNumberWorker(Napi::Env env, Napi::Promise::Deferred& deferred, INDI::BaseClient* client, std::string device,
        std::string property, std::string element, double value)
        : SendNewValueWorker(env, deferred, client, device, property, element), _value(value) {};

    void Execute() {
        if (!_prop.isEmpty()) {
            _client->sendNewNumber(_prop);
        } else {
            _client->sendNewNumber(_device.c_str(), _property.c_str(), _element.c_str(), _value);
        }
    }

  private:
    double _value;
};

class IndiClient::SendNewSwitchWorker : public SendNewValueWorker<ISwitchVectorProperty> {
  public:
    SendNewSwitchWorker(
        Napi::Env env, Napi::Promise::Deferred& deferred, INDI::BaseClient* client, INDI::Property prop)
        : SendNewValueWorker(env, deferred, client, prop) {}
    SendNewSwitchWorker(Napi::Env env, Napi::Promise::Deferred& deferred, INDI::BaseClient* client, std::string device,
        std::string property, std::string element)
        : SendNewValueWorker(env, deferred, client, device, property, element) {}

    void Execute() {
        if (!_prop.isEmpty()) {
            _client->sendNewSwitch(_prop);
        } else {
            _client->sendNewSwitch(_device.c_str(), _property.c_str(), _element.c_str());
        }
    }

  private:
};

class IndiClient::SendNewTextWorker : public SendNewValueWorker<ITextVectorProperty> {
  public:
    SendNewTextWorker(
        Napi::Env env, Napi::Promise::Deferred& deferred, INDI::BaseClient* client, INDI::Property prop)
        : SendNewValueWorker(env, deferred, client, prop) {}
    SendNewTextWorker(Napi::Env env, Napi::Promise::Deferred& deferred, INDI::BaseClient* client, std::string device,
        std::string property, std::string element, std::string value)
        : SendNewValueWorker(env, deferred, client, device, property, element), _value(value) {}

    void Execute() {
        if (!_prop.isEmpty()) {
            _client->sendNewText(_prop);
        } else {
            _client->sendNewText(_device.c_str(), _property.c_str(), _element.c_str(), _value.c_str());
        }
    }

  private:
    std::string _value;
};