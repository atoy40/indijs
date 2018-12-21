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

        //console.log("new props : " + property.name);
        if (property.name === "TELESCOPE_TRACK_STATE") {
          console.log(property.getValue());
        }
      })
      .on("removeDevice", function(device) {
        //console.log("Remove device : " + device.getName());
      })
      .on("removeProperty", function(property) {
        //console.log("Remove property :");
        //console.log(property);
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
};

app();
