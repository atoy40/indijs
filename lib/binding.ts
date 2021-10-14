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

export enum BLOBHandling {
  Never,
  Also,
  Only,
}

export interface IndiDevice {
  getDeviceName(): string;
  getProperty(name: string): IndiProperty<IndiVectors>;
  getProperties(): Array<IndiProperty<IndiVectors>>;
  getDriverInterface(): Number;
  messageQueue(id: number): string;
  connected: boolean;
}

export interface IndiProperty<T extends IndiVector> {
  getName(): string;
  getType(): PropertyType;
  getLabel(): string;
  getGroupName(): string;
  getDeviceName(): string;
  getTimestamp(): string;
  getState(): IndiState;
  getPermission(): IndiPerm;
  getValue(): T;
}

export interface IndiVector {
  name: string;
  label: string;
  group: string;
  device: string;
  state: IndiState;
  permission: IndiPerm;
}

export interface IndiNumberVector extends IndiVector {
  values: Array<IndiNumber>;
}

export interface IndiSwitchVector extends IndiVector {
  rule: IndiRule;
  values: Array<IndiSwitch>;
}

export interface IndiTextVector extends IndiVector {
  values: Array<IndiText>;
}

export interface IndiLightVector extends IndiVector {
  values: Array<IndiLight>;
}

export interface IndiBLOBVector extends IndiVector {
  values: Array<IndiBLOB>;
}

export type IndiVectors =
  | IndiNumberVector
  | IndiSwitchVector
  | IndiTextVector
  | IndiLightVector
  | IndiBLOBVector;

export interface IndiValue<T> {
  name: string;
  label: string;
  value: T;
}

export interface IndiNumber extends IndiValue<number> {
  min: number;
  max: number;
  step: number;
  formated: string;
  getVector(): IndiNumberVector;
}

export interface IndiSwitch extends IndiValue<boolean> {
  getVector(): IndiNumberVector;
}

export interface IndiText extends IndiValue<string> {
  getVector(): IndiTextVector;
}

export interface IndiLight extends IndiValue<IndiState> {
  getVector(): IndiLightVector;
}

export interface IndiBLOB extends IndiValue<Buffer> {
  format: string;
  getVector(): IndiBLOBVector;
}

export declare interface Client {
  on(event: "connected", listener: () => void): this;
  on(event: "disconnected", listener: (code: number) => void): this;
  on(event: "newDevice", listener: (device: IndiDevice) => void): this;
  on(event: "removeDevice", listener: (name: string) => void): this;
  on(
    event: "newProperty",
    listener: (property: IndiProperty<IndiVectors>) => void
  ): this;
  on(
    event: "removeProperty",
    listener: (name: string, device: string) => void
  ): this;
  on(event: "newNumber", listener: (number: IndiNumberVector) => void): this;
  on(event: "newSwitch", listener: (sw: IndiSwitchVector) => void): this;
  on(event: "newText", listener: (text: IndiTextVector) => void): this;
  on(event: "newLight", listener: (light: IndiLightVector) => void): this;
  on(
    event: "newMessage",
    listener: (device: IndiDevice, id: number) => void
  ): this;
  on(event: "newBLOB", listener: (blob: IndiBLOB) => void): this;
  on(eventName: string | symbol, listener: (...args: any[]) => void): this
}

interface IndiClientNative {
  setServer(hostname: string, port: number): void;
  connect(): Promise<boolean>;
  disconnect(): Promise<boolean>;
  connected: boolean;
  connectDevice(name: string): Promise<void>;
  getDevice(name: string): IndiDevice;
  getDevices(): Array<IndiDevice>;
  watchDevice(device: string): void;
  setBLOBMode(mode: BLOBHandling, device: string, prop: string): void;
  setBLOBMode(mode: BLOBHandling, device: string): void;
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

export class Client extends EventEmitter {
  // private members
  private _addonInstance: IndiClientNative;

  constructor(hostname = "localhost", port = 7624) {
    super();
    this._addonInstance = new addon.IndiClient(
      hostname,
      port,
      this.emit.bind(this)
    );
  }

  setServer(hostname: string, port = 7624) {
    this._addonInstance.setServer(hostname, port);
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

  watchDevice(device: string) {
    this._addonInstance.watchDevice(device);
  }

  setBLOBMode(mode: BLOBHandling, device: string, prop?: string) {
    if (prop) {
      this._addonInstance.setBLOBMode(mode, device, prop);
    } else {
      this._addonInstance.setBLOBMode(mode, device);
    }
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
}
