const Indi = require("../dist/binding.js");
const readline = require("readline");
var colors = require("colors");

readline.emitKeypressEvents(process.stdin);
process.stdin.setRawMode(true);

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

const indi = new Indi.Client(process.argv[2]);

process.stdin.on("keypress", (str) => {
  if (str == "q") {
    indi.disconnect();
  } else {
    console.log("Press q to disconnect indi server...".red);
  }
});

indi
  .on("connected", (_) => console.log("[connected]".green))
  .on("disconnected", (code) => {
    console.log(`[disconnected] code ${code}`.red);
    process.exit();
  })
  .on("newDevice", (device) => {
    if (!device.connected) {
      indi.connectDevice(device.getDeviceName());
    }
    const str = `[new device] "${device.getDeviceName()}"`;
    console.log(device.connected ? str.green : str.red);
  })
  .on("removeDevice", (name) => console.log(`[del device] "${name}"`.red))
  .on("newProperty", (property) =>
    displayValues(property.getValue(), " [new props] ")
  )
  .on("removeProperty", (name, deviceName) =>
    console.log(` [del props] "${name}" on "${deviceName}"`.red)
  )
  .on("newNumber", (v) => displayValues(v, "[new number] "))
  .on("newSwitch", (v) => displayValues(v, "[new switch] " + v.rule))
  .on("newText", (v) => displayValues(v, "  [new text] "))
  .on("newLight", (v) => displayValues(v, " [new light] "));

indi.connect().then(res => console.log(res)).catch((res) => console.log("err: "+res));
