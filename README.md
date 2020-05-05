INDIjs
===

INDIjs is a nodejs binding for the INDI library. INDI Library is an Open Source Architecture for Control & Automation of Astronomical Devices.

Install
---

```bash
npm install [-g] @toy40/indijs
```

If you've installed globally, you can use the simple CLI tool :
```bash
indijs-cli localhost
```

Usage
---

```js
const indijs = require("@atoy40/indijs");

const client = indijs.Client("localhost");

client
  .on("connect", () => {})
  .on("disconnect", () => {})
  .on("newDevice", (device) => {})
  .on("removeDevice", (devname) => {})
  .on("newProperty", (property) => {})
  .on("removeProperty", (propname, devname) => {})
  .on("newNumber", (number) => {})
  .on("newSwitch", (switch) => {})
  .on("newText", (text) => {})
  .on("newLight", (light) => {})

client.connect().then(() => {
  console.log("connected...);
})
```

API
---

TBD