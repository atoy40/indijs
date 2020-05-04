#include "promise.h"

PromiseWorker::PromiseWorker(Napi::Env env) : AsyncWorker(env), _deferred(env) {}

Napi::Promise::Deferred PromiseWorker::Deferred() {
    return _deferred;
}

Napi::Value PromiseWorker::GetResolve() {
    return Env().Undefined();
}

Napi::Value PromiseWorker::GetReject(const Napi::Error& e) {
    return Napi::String::New(Env(), e.Message());
}

void PromiseWorker::OnOK() {
    _deferred.Resolve(GetResolve());
}

void PromiseWorker::OnError(const Napi::Error& e) {
    _deferred.Reject(GetReject(e));
}
