const Indi = require("../dist/binding.js");

const app = async () => {
  const indi = new Indi();

  try {
    const events = await indi.connect();

    events
      .on("connected", function() {
        console.log("INDI Connected");
      })
      .on("disconnected", function(code) {
        console.log("INDI disconnected : " + code);
      })
      .on("newProperty", async function(property) {
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
  } catch (e) {
    return console.log(e.message);
  }
};

app();
