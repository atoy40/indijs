const addon = require("../build/Release/indi-native");
const EventEmitter = require("events").EventEmitter;

enum PropertyType {
  Number,
  Switch,
  Text,
  Light,
  Blob,
  Unknown
}

interface IDevice {
  getDeviceName(): string;
}

interface IProperty {
  getName(): string;
  getType(): PropertyType;
  getLabel(): string;
  getGroupName(): string;
  getDeviceName(): string;
  getTimestamp(): string;
}

interface BaseVector {
  name: string;
  label: string;
  dimension: number;
}

interface NumberVector extends BaseVector {
  values: { [name: string]: NumberElement };
}

interface NumberElement {
  name: string;
  label: string;
  value: number;
  min: number;
  max: number;
  step: number;
}

interface IEventEmitter {
  on(event: "device", listener: (device: IDevice) => void): this;
  on(event: string, listener: Function): this;
}

interface IIndiNative {
  connect(callback: Function): Promise<void>;
  disconnect(): void;
  sendNewSwitch(
    deviceName: string,
    propertyName: string,
    elementName: string
  ): void;
}

class Indi {
  constructor(hostname = "localhost", port = 7624) {
    this._addonInstance = new addon.Indi(hostname, port);
    this._ievents = new EventEmitter();
    this._events = new EventEmitter();

    this._ievents.on('newDevice', (value: any) => {
     //console.log(value);
      /*this._events.emit("newProperty", {
        name: value.getName(),
      });*/
    });

    this._ievents.on('newProperty', (value: any) => {
      //console.log(value);
      /*this._events.emit("newProperty", {
        name: value.getName(),
      });*/
    });

    this._ievents.on('newNumber', (value: any) => {
      //console.log(value);
      /*this._events.emit("newProperty", {
        name: value.getName(),
      });*/
    });
    this._ievents.on('newSwitch', (value: any) => {
      console.log(value);
      /*this._events.emit("newProperty", {
        name: value.getName(),
      });*/
    });
  }

  async connect() {
    //this._addonInstance.connect(this._events.emit.bind(this._events));
    await this._addonInstance.connect(this._ievents.emit.bind(this._ievents));
    return this._events as IEventEmitter;
  }

  disconnect() {
    this._addonInstance.disconnect();
  }

  sendNewSwitch(deviceName: string, propertyName: string, elementName: string) {
    this._addonInstance.sendNewSwitch(deviceName, propertyName, elementName);
  }

  // private members
  private _addonInstance: IIndiNative;
  private _ievents: any;
  private _events: any;
}

export = Indi;
