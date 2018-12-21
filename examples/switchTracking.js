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

          console.log("Set tracking to: " + !ttrack.values.TRACK_ON.value);
          ttrack.values.TRACK_ON.value = !ttrack.values.TRACK_ON.value;
          ttrack.values.TRACK_OFF.value = !ttrack.values.TRACK_OFF.value;

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
