const Indi = require("@atoy40/indijs");

const indi = new Indi.Client(process.argv[2] || "localhost");
indi
  .on("connected", function () {
    console.log("INDI Connected");
  })
  .on("disconnected", function (code) {
    console.log("INDI disconnected : " + code);
  })
  .on("newDevice", (device) => {
    const str = `[new device] "${device.getDeviceName()}"`;
    console.log(device.connected ? str.green : str.red);
    // auto connect device
    if (!device.connected) {
      indi.connectDevice(device.getDeviceName());
    }
  })
  .on("newProperty", async function (property) {
    if (property.getName() === "TELESCOPE_TRACK_STATE") {
      const ttrack = property.getValue();

      console.log("Set tracking to: " + !ttrack.values[0].value);
      ttrack.values[0].value = !ttrack.values[0].value;
      ttrack.values[1].value = !ttrack.values[1].value;

      // send new track value
      await indi.sendNewSwitch(ttrack);

      indi.disconnect();
    }
  });

const app = async () => {
  try {
    await indi.connect();
  } catch (e) {
    return console.log(e);
  }
};

app();
