const Indi = require("../dist/binding.js");
const readline = require('readline');
var colors = require('colors');

readline.emitKeypressEvents(process.stdin);
process.stdin.setRawMode(true);

const indi = new Indi.Client(process.argv[2]);

const app = async () => {

  try {
    const events = await indi.connect();

    process.stdin.on('keypress', (str, key) => {
      if (str == 'q') {
        indi.disconnect();
      } else {
        console.log("Press q to disconnect indi server...".red)
      }
    });

    function stateToEmoji(state) {
      switch(state) {
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
        const values = value.values.map(v => `${v.name}:${v.value}`.cyan);
        console.log(`${prefix.yellow||""} ${stateToEmoji(value.state)} ${value.device}/${value.group}/${value.label || value.name}" => [ ${values.join(", ")} ]`);
      }
    }

    events
      .on("connected", _ => console.log("[connected]".green))
      .on("disconnected", code => {
        console.log(`[disconnected] code ${code}`.red)
        process.exit();
      })
      .on("newDevice", device => {
        if (!device.connected) {
          indi.connectDevice(device.getDeviceName());
        }
        const str = `[new device] "${device.getDeviceName()}"`;
        console.log(device.connected ? str.green : str.red);
      })
      .on("removeDevice", name => console.log(`[del device] "${name}"`.red))
      .on("newProperty", property => displayValues(property.getValue(), " [new props] "))
      .on("removeProperty", (name, deviceName) => console.log(` [del props] "${name}" on "${deviceName}"`.red))
      .on("newNumber", v => displayValues(v, "[new number] "))
      .on("newSwitch", v => displayValues(v, "[new switch] "+v.rule))
      .on("newText", v => displayValues(v, "  [new text] "))
      .on("newLight", v => displayValues(v, " [new light] "));
  } catch (e) {
    return console.log("ERROR: "+e.message);
  }
};

app();
