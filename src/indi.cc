#include "client.h"
#include "promise.h"
#include "property.h"
#include "vector.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    IndiClient::GetClass(env, exports);
    Device::GetClass(env, exports);
    Property::GetClass(env, exports);

    NumberValue::GetClass(env, exports);
    SwitchValue::GetClass(env, exports);
    TextValue::GetClass(env, exports);
    LightValue::GetClass(env, exports);
    BLOBValue::GetClass(env, exports);

    NumberVector::GetClass(env, exports);
    SwitchVector::GetClass(env, exports);
    TextVector::GetClass(env, exports);
    LightVector::GetClass(env, exports);
    BLOBVector::GetClass(env, exports);

    return exports;
}

NODE_API_MODULE(addon, Init)
