#include "client.h"
#include "promise.h"
#include "prop.h"
#include "value.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    IndiClient::GetClass(env, exports);
    Device::GetClass(env, exports);

    PropertyNumber::GetClass(env, exports);
    PropertySwitch::GetClass(env, exports);
    PropertyText::GetClass(env, exports);
    PropertyLight::GetClass(env, exports);

    NumberValue::GetClass(env, exports);
    SwitchValue::GetClass(env, exports);
    TextValue::GetClass(env, exports);
    LightValue::GetClass(env, exports);
    BLOBValue::GetClass(env, exports);

    return exports;
}

NODE_API_MODULE(addon, Init)
