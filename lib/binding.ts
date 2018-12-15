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
  getDeviceName() : string;
}

interface IProperty {
  getName() : string;
  getType() : PropertyType;
  getLabel() : string;
  getGroupName() : string;
  getDeviceName() : string;
  getTimestamp() : string;
}

interface IEventEmitter {
  on(event: "device", listener: (device: IDevice) => void): this;
  on(event: "property", listener: (property: IProperty) => void): this;
  on(event: string, listener: Function): this;
}

interface IIndiNative {
  connect(callback: Function): Promise<void>;
  disconnect(): void;
}

class Indi {
  constructor(hostname = "localhost", port = 7624) {
    this._addonInstance = new addon.Indi(hostname, port);
    this._events = new EventEmitter();
  }

  async connect() {
    //this._addonInstance.connect(this._events.emit.bind(this._events));
    await this._addonInstance.connect(this._events.emit.bind(this._events));
    return this._events;
  }

  disconnect() {
    this._addonInstance.disconnect();
  }

  // private members
  private _addonInstance: IIndiNative;
  private _events: any;
}

export = Indi;
