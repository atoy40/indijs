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

export enum IndiDriverInterface {
  General = 0,
  Telescope = 1 << 0,
  Ccd = 1 << 1,
  Guider = 1 << 2,
  Focuser = 1 << 3,
  Filter = 1 << 4,
  Dome = 1 << 5,
  Gps = 1 << 6,
  Weather = 1 << 7,
  Ao = 1 << 8,
  Dustcap = 1 << 9,
  Lightbox = 1 << 10,
  Detector = 1 << 11,
  Rotator = 1 << 12,
  Spectrograph = 1 << 13,
  Correlator = 1 << 14,
  Aux = 1 << 15,
}

export enum IndiRule {
  OneOfMany,
  AtMostOne,
  NOfMany,
}

export enum IndiState {
  Idle,
  Ok,
  Busy,
  Alert,
}

export enum IndiPerm {
  ReadOnly,
  WriteOnly,
  ReadWrite,
}

interface IndiDevice {
  getDeviceName(): string;
  getProperty(): IndiProperty<IndiVector>;
  getProperties(): Array<IndiProperty<IndiVector>>;
  getDriverInterface(): Number;
  connected: boolean;
}

interface IndiProperty<T extends IndiVector> {
  getName(): string;
  getType(): PropertyType;
  getLabel(): string;
  getGroupName(): string;
  getDeviceName(): string;
  getTimestamp(): string;
  getPermission(): IndiPerm;
  getValue(): T;
}

interface IndiVector {
  name: string;
  label: string;
  group: string;
  device: string;
  state: IndiState;
  permission: IndiPerm;
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
  on(event: "connected", listener: () => void): this;
  on(event: "disconnected", listener: (code: number) => void): this;
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
  on(
    event: "newMessage",
    listener: (device: IndiDevice, id: number) => void
  ): this;
}

interface IndiClientNative {
  connect(): Promise<boolean>;
  disconnect(): Promise<boolean>;
  connected: boolean;
  connectDevice(name: string): Promise<void>;
  getDevice(name: string): IndiDevice;
  getDevices(): Array<IndiDevice>;
  sendNewNumber(props: Object): Promise<void>;
  sendNewNumber(
    device: string,
    property: string,
    element: string,
    value: number
  ): Promise<void>;
  sendNewSwitch(props: Object): Promise<void>;
  sendNewSwitch(
    device: string,
    property: string,
    element: string
  ): Promise<void>;
  sendNewText(props: Object): Promise<void>;
  sendNewText(
    device: string,
    property: string,
    element: string,
    value: string
  ): Promise<void>;
}

export class Client extends EventEmitter implements IndiEventEmitter {
  constructor(hostname = "localhost", port = 7624) {
    super();
    this._addonInstance = new addon.IndiClient(
      hostname,
      port,
      this.emit.bind(this)
    );
  }

  connect() {
    return this._addonInstance.connect();
  }

  disconnect() {
    return this._addonInstance.disconnect();
  }

  get connected(): boolean {
    return this._addonInstance.connected;
  }

  connectDevice(name: string) {
    return this._addonInstance.connectDevice(name);
  }

  getDevice(name: string) {
    return this._addonInstance.getDevice(name);
  }

  getDevices() {
    return this._addonInstance.getDevices();
  }

  sendNewNumber(
    propsOrDevice: IndiProperty<IndiNumberVector> | string,
    property?: string,
    element?: string,
    value?: number
  ) {
    if (
      typeof propsOrDevice === "string" &&
      typeof property === "string" &&
      typeof element === "string" &&
      typeof value === "number"
    ) {
      return this._addonInstance.sendNewNumber(
        propsOrDevice,
        property,
        element,
        value
      );
    } else {
      return this._addonInstance.sendNewNumber(propsOrDevice);
    }
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

  sendNewText(
    propsOrDevice: IndiProperty<IndiTextVector> | string,
    property?: string,
    element?: string,
    value?: string
  ) {
    if (
      typeof propsOrDevice === "string" &&
      typeof property === "string" &&
      typeof element === "string" &&
      typeof value === "string"
    ) {
      return this._addonInstance.sendNewText(
        propsOrDevice,
        property,
        element,
        value
      );
    } else {
      return this._addonInstance.sendNewText(propsOrDevice);
    }
  }

  // private members
  private _addonInstance: IndiClientNative;
}
