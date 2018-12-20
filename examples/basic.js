const Indi = require("../dist/binding.js");

let ttrack;

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
      .on("newDevice", function(device) {
        console.log("New device : " + device.getDeviceName());
      })
      .on("newProperty", function(property) {
        if (property.getName() === "TELESCOPE_TRACK_STATE") {
          ttrack = property.getValue();
        }
      })
      .on("removeDevice", function(device) {
        console.log("Remove device : " + device.getDeviceName());
      })
      .on("removeProperty", function(property) {
        console.log("Remove property :");
        console.log(property);
      })
      .on("newNumber", function(value) {
        //console.log("New number");
        //console.log(value);
      })
      .on("newSwitch", function(value) {
        console.log("New switch");
        console.log(value);
      });
  } catch (e) {
    return console.log(e.message);
  }

  setInterval(() => {
    if (ttrack) {
      console.log(ttrack.values);
      ttrack.values.TRACK_ON.value = ttrack.values.TRACK_OFF.value;
      ttrack.values.TRACK_OFF.value = ttrack.values.TRACK_ON.value;
      indi.sendNewSwitch(ttrack);
    }
  }, 5000);
};

app();
