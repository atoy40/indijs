#pragma once

//#include <libindi/baseclient.h>
#include <libindi/basedevice.h>
#include <napi.h>

template<typename T, typename V>
class PropertyBase : public Napi::ObjectWrap<T> {
  public:
    PropertyBase(const Napi::CallbackInfo&);
    // virtual ~PropertyBase();

    Napi::Value GetBaseDevice(const Napi::CallbackInfo& info);
    Napi::Value GetDeviceName(const Napi::CallbackInfo& info);
    Napi::Value GetGroupName(const Napi::CallbackInfo& info);
    Napi::Value GetLabel(const Napi::CallbackInfo& info);
    Napi::Value GetName(const Napi::CallbackInfo& info);
    Napi::Value GetPermission(const Napi::CallbackInfo& info);
    Napi::Value GetPermissionAsString(const Napi::CallbackInfo& info);
    Napi::Value GetRegistered(const Napi::CallbackInfo& info);
    Napi::Value GetState(const Napi::CallbackInfo& info);
    Napi::Value GetStateAsString(const Napi::CallbackInfo& info);
    Napi::Value GetTimestamp(const Napi::CallbackInfo& info);
    Napi::Value GetType(const Napi::CallbackInfo& info);
    Napi::Value GetTypeAsString(const Napi::CallbackInfo& info);
    Napi::Value Count(const Napi::CallbackInfo& info);
    virtual Napi::Value GetValues(const Napi::CallbackInfo& info) = 0;

    //void SetValues(const Napi::CallbackInfo& info, const Napi::Value& value) = 0;
    


    void SetDeviceName(const Napi::CallbackInfo& info, const Napi::Value& value);
    void SetName(const Napi::CallbackInfo& info, const Napi::Value& value);
    void SetLabel(const Napi::CallbackInfo& info, const Napi::Value& value);
    void SetGroupName(const Napi::CallbackInfo& info, const Napi::Value& value);
    void SetState(const Napi::CallbackInfo& info, const Napi::Value& value);
    void SetTimestamp(const Napi::CallbackInfo& info, const Napi::Value& value);
    void SetPermission(const Napi::CallbackInfo& info, const Napi::Value& value);

    static Napi::Function GetClass(Napi::Env env, Napi::Object exports, const char* name,
                                   std::vector<Napi::ClassPropertyDescriptor<T>>& properties);
    static Napi::Function GetClass(Napi::Env env, Napi::Object exports, const char* name);

    // virtual Napi::Value getValue(const Napi::Env &env) = 0;
    // virtual void setProps(Napi::Object &object) = 0;

    virtual V getProperty() = 0;
};

class PropertyNumber : public PropertyBase<PropertyNumber, INDI::PropertyNumber> {
  public:
    PropertyNumber(const Napi::CallbackInfo& info);
    static INDI::PropertyNumber getProp(const Napi::CallbackInfo& info);

    static Napi::Object NewInstance(Napi::Value arg);
    static void GetClass(Napi::Env env, Napi::Object exports);
    Napi::Value GetValues(const Napi::CallbackInfo& info) override;
    INDI::PropertyNumber getProperty() {
        return _prop;
    };

  private:
    static Napi::FunctionReference constructor;
    INDI::PropertyNumber _prop;
};

class PropertySwitch : public PropertyBase<PropertySwitch, INDI::PropertySwitch> {
  public:
    PropertySwitch(const Napi::CallbackInfo& info);
    static INDI::PropertySwitch getProp(const Napi::CallbackInfo& info);

    static Napi::Object NewInstance(Napi::Value arg);
    static void GetClass(Napi::Env env, Napi::Object exports);
    Napi::Value GetValues(const Napi::CallbackInfo& info) override;
    Napi::Value GetRule(const Napi::CallbackInfo& info);
    Napi::Value GetRuleAsString(const Napi::CallbackInfo& info);
    Napi::Value FindOnSwitchIndex(const Napi::CallbackInfo& info);
    Napi::Value FindOnSwitchName(const Napi::CallbackInfo& info);
    Napi::Value FindOnSwitch(const Napi::CallbackInfo& info);
    INDI::PropertySwitch getProperty() {
        return _prop;
    };

  private:
    static Napi::FunctionReference constructor;
    INDI::PropertySwitch _prop;
};

class PropertyText : public PropertyBase<PropertyText, INDI::PropertyText> {
  public:
    PropertyText(const Napi::CallbackInfo& info);
    static INDI::PropertyText getProp(const Napi::CallbackInfo& info);

    static Napi::Object NewInstance(Napi::Value arg);
    static void GetClass(Napi::Env env, Napi::Object exports);
    Napi::Value GetValues(const Napi::CallbackInfo& info) override;
    INDI::PropertyText getProperty() {
        return _prop;
    };

  private:
    static Napi::FunctionReference constructor;
    INDI::PropertyText _prop;
};

class PropertyLight : public PropertyBase<PropertyLight, INDI::PropertyLight> {
  public:
    PropertyLight(const Napi::CallbackInfo& info);
    static INDI::PropertyLight getProp(const Napi::CallbackInfo& info);

    static Napi::Object NewInstance(Napi::Value arg);
    static void GetClass(Napi::Env env, Napi::Object exports);
    Napi::Value GetValues(const Napi::CallbackInfo& info) override;
    INDI::PropertyLight getProperty() {
        return _prop;
    };

  private:
    static Napi::FunctionReference constructor;
    INDI::PropertyLight _prop;
};


// utility functions
Napi::Object getTypedProperty(Napi::Env env, INDI::Property property);