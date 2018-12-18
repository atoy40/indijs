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
      .on("newDevice", function(device) {
        console.log("New device : " + device.getDeviceName());
      })
      .on("newProperty", function(property) {
        console.log("New property :");
        console.log(property);
      })
      .on("removeDevice", function(device) {
        console.log("Remove device : " + device.getDeviceName());
      })
      .on("removeProperty", function(property) {
        console.log("Remove property :");
        console.log(property);
      })
      .on("newNumber", function(value) {
        console.log("New number");
        console.log(value.name);
        console.log(value);
      })
      .on("newSwitch", function(value) {
        console.log("New switch");
        console.log(value);
      });
  } catch (e) {
    return console.log(e.message);
  }

  //setTimeout(indi.disconnect.bind(indi), 5000);
  let track = true;
  /*setInterval(() => {
    indi.sendNewSwitch(
      "Telescope Simulator",
      "USEJOYSTICK",
      track ? "ENABLE" : "DISABLE"
    );
    track = !track;
  }, 2000);*/
};

app();
