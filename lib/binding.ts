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
  isValid(): boolean;
  getDeviceName(): string;
  getProperty(name: string): IndiProperty | undefined;
  getProperties(): Array<IndiProperty>;
  getDriverInterface(): Number;
  messageQueue(id: number): string;
  watchProperty(name: string, cb: (prop: IndiProperty) => void): void;
  connected: boolean;
}

export interface IndiProperty {
  getBaseDevice(): IndiDevice;
  getRegistered(): boolean;
  getName(): string;
  getType(): PropertyType;
  getTypeAsString(): string;
  getLabel(): string;
  getGroupName(): string;
  getDeviceName(): string;
  getTimestamp(): string;
  getState(): IndiState;
  getStateAsString(): string;
  getPermission(): IndiPerm;
  getPermissionAsString(): string;
  count(): number;
}

export interface IndiPropertyNumber extends IndiProperty {
  getValues(): IndiNumber[];
}

export interface IndiPropertySwitch extends IndiProperty {
  getValues(): IndiSwitch[];
  getRule(): IndiRule;
  getRuleAsString(): string;
  findOnSwitchIndex(): number;
  findOnSwitchName(): string;
  findOnSwitch(): IndiSwitch | undefined;
}

export interface IndiPropertyText extends IndiProperty {
  getValues(): IndiText[];
}

export interface IndiPropertyLight extends IndiProperty {
  getValues(): IndiLight[];
}

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
}

export interface IndiSwitch extends IndiValue<boolean> {}

export interface IndiText extends IndiValue<string> {}

export interface IndiLight extends IndiValue<IndiState> {}

export interface IndiBLOB extends IndiValue<Buffer> {
  format: string;
}

export declare interface Client {
  on(event: "connected", listener: () => void): this;
  on(event: "disconnected", listener: (code: number) => void): this;
  on(event: "newDevice", listener: (device: IndiDevice) => void): this;
  on(event: "removeDevice", listener: (name: string) => void): this;
  on(
    event: "newProperty",
    listener: (property: IndiProperty) => void
  ): this;
  on(
    event: "updateProperty",
    listener: (property: IndiProperty) => void
  ): this;
  on(
    event: "removeProperty",
    listener: (name: string, device: string) => void
  ): this;
  on(
    event: "newMessage",
    listener: (device: IndiDevice, id: number) => void
  ): this;
  on(eventName: string | symbol, listener: (...args: any[]) => void): this;
}

interface IndiClientNative {
  host: string;
  port: number;
  connected: boolean;
  setServer(hostname: string, port: number): void;
  connect(): Promise<boolean>;
  disconnect(): Promise<boolean>;
  connectDevice(name: string): Promise<void>;
  getDevice(name: string): IndiDevice | undefined;
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

  get host() {
    return this._addonInstance.host;
  }

  get port() {
    return this._addonInstance.port;
  }

  get connected() {
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
    propOrDevice: IndiPropertyNumber | string,
    property?: string,
    element?: string,
    value?: number
  ) {
    if (
      typeof propOrDevice === "string" &&
      typeof property === "string" &&
      typeof element === "string" &&
      typeof value === "number"
    ) {
      return this._addonInstance.sendNewNumber(
        propOrDevice,
        property,
        element,
        value
      );
    } else {
      return this._addonInstance.sendNewNumber(propOrDevice);
    }
  }

  sendNewSwitch(
    propOrDevice: IndiPropertySwitch | string,
    property?: string,
    element?: string
  ) {
    if (
      typeof propOrDevice === "string" &&
      typeof property === "string" &&
      typeof element === "string"
    ) {
      return this._addonInstance.sendNewSwitch(propOrDevice, property, element);
    } else {
      return this._addonInstance.sendNewSwitch(propOrDevice);
    }
  }

  sendNewText(
    propOrDevice: IndiPropertyText | string,
    property?: string,
    element?: string,
    value?: string
  ) {
    if (
      typeof propOrDevice === "string" &&
      typeof property === "string" &&
      typeof element === "string" &&
      typeof value === "string"
    ) {
      return this._addonInstance.sendNewText(
        propOrDevice,
        property,
        element,
        value
      );
    } else {
      return this._addonInstance.sendNewText(propOrDevice);
    }
  }
}
