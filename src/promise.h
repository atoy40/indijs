#pragma once

#include <libindi/baseclient.h>
#include <napi.h>

class PromiseWorker : public Napi::AsyncWorker {
  public:
    PromiseWorker(Napi::Env env);
    virtual void OnOK();
    virtual void OnError(const Napi::Error& e);
    virtual Napi::Value GetResolve();
    virtual Napi::Value GetReject(const Napi::Error& e);
    Napi::Promise::Deferred Deferred();

  private:
    Napi::Promise::Deferred _deferred;
};
