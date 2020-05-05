const Indi = require("../dist/binding.js");

const indi = new Indi.Client(process.argv[2] || "localhost");
indi
  .on("connected", function () {
    console.log("INDI Connected");
  })
  .on("disconnected", function (code) {
    console.log("INDI disconnected : " + code);
  })
  .on("newProperty", async function (property) {
    if (property.getName() === "TELESCOPE_TRACK_STATE") {
      //const ttrack = property.getValue();

      //console.log("Set tracking to: " + !ttrack.values[0].value);
      //ttrack.values[0].value = !ttrack.values[0].value;
      //ttrack.values[1].value = !ttrack.values[1].value;

      // send new track value
      //await indi.sendNewSwitch(ttrack);
      await indi.sendNewSwitch(
        property.getDeviceName(),
        property.getName(),
        process.argv[3] === "on" ? "TRACK_ON" : "TRACK_OFF"
      );

      indi.disconnect();
    }
  });

const app = async () => {
  try {
    const events = await indi.connect();
  } catch (e) {
    return console.log(e);
  }
};

app();
