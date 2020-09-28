INDIjs
===

INDIjs is a nodejs binding of the INDI library. INDI Library is an Open Source Architecture for Control & Automation of Astronomical Devices. It only binds the client part of the library but device part will be added soon.

Install
---

You'll need INDI library and headers to be able to compile.

```bash
npm install [-g] @atoy40/indijs
```

If you've installed it globally, you can use the simple CLI tool :

```bash
indijs-cli localhost
```

Usage
---

```js
import { Client } from "@atoy40/indijs";

const client = new INDIjs.Client("localhost");

client
  .on("connected", () => { /* code */ })
  .on("disconnected", (reason_code) => { /* code */ })
  .on("newDevice", (device) => { /* code */ })
  .on("removeDevice", (devname) => { /* code */ })
  .on("newProperty", (property) => { /* code */ })
  .on("removeProperty", (propname, devname) => { /* code */ })
  .on("newNumber", (n) => { /* code */ })
  .on("newSwitch", (s) => { /* code */ })
  .on("newText", (t) => { /* code */ })
  .on("newLight", (l) => { /* code */ })
  .on("newBLOB", (buffer) => { /* code */ })
  .on("newMessage", (device, messageID) => { /* code */ })

client.connect().then(() => {
  console.log("connected...");
});
```

A simple example switching the telescope track state is available in examples/ folder.
