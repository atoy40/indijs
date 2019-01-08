const Indi = require("../dist/binding.js");

const app = async () => {
  const indi = new Indi();

  try {
    const events = await indi.connect();

    function displayValues(value, prefix) {
      const values = value.values.map(v => `${v.name}:${v.value}`);
      console.log(`${prefix||""}${value.device}/${value.group}/${value.label || value.name}" => [ ${values.join(", ")} ]`);
    }

    events
      .on("connected", _ => console.log("[connected]"))
      .on("disconnected", code => console.log(`[disconnected] code ${code}`))
      .on("newDevice", device => console.log(`[new] device "${device.getDeviceName()}"`))
      .on("removeDevice", name => console.log(`[del] device "${name}"`))
      .on("newProperty", function(property) {
        const value = property.getValue();
        if (value) {
          displayValues(value, "[new] ");
        }
      })
      .on("removeProperty", (name, deviceName) => console.log(`[del] "${name}" on "${deviceName}"`))
      .on("newNumber", v => displayValues(v, "[num] "))
      .on("newSwitch", v => displayValues(v, "[swt] "))
      .on("newText", v => displayValues(v, "[txt] "))
      .on("newLight", v => displayValues(v, "[lgt] "));
  } catch (e) {
    return console.log(e.message);
  }
};

app();
