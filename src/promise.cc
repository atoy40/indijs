#include "promise.h"

PromiseWorker::PromiseWorker(Napi::Env env) : AsyncWorker(env), _deferred(env) {}

Napi::Promise::Deferred PromiseWorker::Deferred() {
    return _deferred;
}

Napi::Value PromiseWorker::GetResolve() {
    Napi::HandleScope scope(Env());
    return Env().Undefined();
}

Napi::Value PromiseWorker::GetReject(const Napi::Error& e) {
    Napi::HandleScope scope(Env());
    return Env().Undefined();
}

void PromiseWorker::OnOK() {
    Napi::HandleScope scope(Env());
    _deferred.Resolve(GetResolve());
}

void PromiseWorker::OnError(const Napi::Error& e) {
    Napi::HandleScope scope(Env());
    _deferred.Reject(GetReject(e));
}
