const addon = require("../build/Release/indi-native");
import { EventEmitter } from "events";

export enum PropertyType {
  Number,
  Switch,
  Text,
  Light,
  Blob,
  Unknown,
}

export enum IndiState {
  Idle,
  Ok,
  Busy,
  Alert,
}

export enum IndiRule {
  OneOfMany,
  AtMostOne,
  NOfMany,
}

interface IndiDevice {
  getDeviceName(): string;
  connected: boolean;
}

interface IndiProperty<T extends IndiVector> {
  getName(): string;
  getType(): PropertyType;
  getLabel(): string;
  getGroupName(): string;
  getDeviceName(): string;
  getTimestamp(): string;
  getValue(): T;
}

interface IndiVector {
  name: string;
  label: string;
  group: string;
  state: IndiState;
}

interface IndiNumberVector extends IndiVector {
  values: Array<IndiNumber>;
}

interface IndiSwitchVector extends IndiVector {
  rule: IndiRule;
  values: Array<IndiSwitch>;
}

interface IndiTextVector extends IndiVector {
  values: Array<IndiText>;
}

interface IndiLightVector extends IndiVector {
  values: Array<IndiLight>;
}

interface IndiValue<T> {
  name: string;
  label: string;
  value: T;
}

interface IndiNumber extends IndiValue<number> {
  min: number;
  max: number;
  step: number;
}

interface IndiSwitch extends IndiValue<boolean> {}

interface IndiText extends IndiValue<string> {}

interface IndiLight extends IndiValue<IndiState> {}

interface IndiEventEmitter {
  on(event: "connect", listener: () => void): this;
  on(event: "disconnect", listener: (code: number) => void): this;
  on(event: "newDevice", listener: (device: IndiDevice) => void): this;
  on(event: "removeDevice", listener: (name: string) => void): this;
  on(
    event: "newProperty",
    listener: (
      property: IndiProperty<
        IndiNumberVector | IndiSwitchVector | IndiTextVector | IndiLightVector
      >
    ) => void
  ): this;
  on(
    event: "removeProperty",
    listener: (name: string, device: string) => void
  ): this;
  on(event: "newNumber", listener: (number: IndiNumberVector) => void): this;
  on(event: "newSwitch", listener: (number: IndiSwitchVector) => void): this;
  on(event: "newText", listener: (number: IndiTextVector) => void): this;
  on(event: "newLight", listener: (number: IndiLightVector) => void): this;
}

interface IndiClientNative {
  connect(): Promise<boolean>;
  disconnect(): void;
  connectDevice(name: string): Promise<void>;
  sendNewNumber(props: Object): Promise<void>;
  sendNewSwitch(props: Object): Promise<void>;
  sendNewSwitch(
    device: string,
    property: string,
    element: string
  ): Promise<void>;
}

export class Client extends EventEmitter {
  constructor(hostname = "localhost", port = 7624) {
    super();
    this._addonInstance = new addon.IndiClient(hostname, port, this.emit.bind(this));
    //this._events = new EventEmitter();
  }

  connect() {
    return this._addonInstance.connect();
  }

  disconnect() {
    this._addonInstance.disconnect();
  }

  connectDevice(name: string) {
    return this._addonInstance.connectDevice(name);
  }

  sendNewNumber(props: Object) {
    return this._addonInstance.sendNewNumber(props);
  }

  sendNewSwitch(
    propsOrDevice: IndiProperty<IndiSwitchVector> | string,
    property?: string,
    element?: string
  ) {
    if (
      typeof propsOrDevice === "string" &&
      typeof property === "string" &&
      typeof element === "string"
    ) {
      return this._addonInstance.sendNewSwitch(
        propsOrDevice,
        property,
        element
      );
    } else {
      return this._addonInstance.sendNewSwitch(propsOrDevice);
    }
  }

  // private members
  private _addonInstance: IndiClientNative;
  //private _events: any;
}
