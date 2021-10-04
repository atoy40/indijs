#!/usr/bin/env node

const Indi = require("../dist/binding.js");
var colors = require("colors");

// disconnect on SIGINT
process.on("SIGINT", () => {
  indi.disconnect();
});

function stateToEmoji(state) {
  switch (state) {
    case 0:
      return "⚪";
    case 1:
      return "🟢";
    case 2:
      return "🟡";
    case 3:
      return "🔴";
  }
}

function displayValues(value, prefix) {
  if (!value) {
    return console.log("no value");
  }
  if (value && value.values) {
    const values = value.values.map((v) => `${v.name}:${v.value}`.cyan);
    console.log(
      `${prefix.yellow || ""} ${stateToEmoji(value.state)} ${value.device}/${
        value.group
      }/${value.label || value.name}" => [ ${values.join(", ")} ]`
    );
  }
}

const indi = new Indi.Client(process.argv[2] || "localhost");

indi
  .on("connected", (_) => console.log("[connected]".green))
  .on("disconnected", (code) => {
    console.log(`[disconnected] code ${code}`.red);
  })

  .on("newDevice", (device) => {
    indi.setBLOBMode(Indi.BLOBHandling.Never, device.getDeviceName());
    const str = `[new device] "${device.getDeviceName()}"`;
    console.log(device.connected ? str.green : str.red);
    // auto connect device
    if (!device.connected) {
      indi.connectDevice(device.getDeviceName());
    }
  })
  .on("removeDevice", (name) => console.log(`[del device] "${name}"`.red))

  .on("newProperty", (p) => displayValues(p.getValue(), " [new props] "))
  .on("removeProperty", (name, deviceName) =>
    console.log(` [del props] "${name}" on "${deviceName}"`.red)
  )

  .on("newNumber", (v) => displayValues(v, "[new number] "))
  .on("newSwitch", (v) => displayValues(v, "[new switch] "))
  .on("newText", (v) => displayValues(v, "  [new text] "))
  .on("newLight", (v) => displayValues(v, " [new light] "));

indi
  .connect()
  .then((res) => console.log(res))
  .catch((res) => console.log("err: " + res));