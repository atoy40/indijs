const Indi = require("./dist/binding.js");

const app = async () => {
  const indi = new Indi();

  try {
    const events = await indi.connect();

    events
    .on('connected', function() {
      console.log("INDI Connected");
    })
    .on('disconnected', function(code) {
      console.log("INDI disconnected : "+code);
    })
    .on('device', function(device) {
      console.log("New device : " + device.getDeviceName());
    })
    .on('property', function(property) {
      console.log("New property : " + property.getName());
      console.log(" " + property.getType());
      console.log(" " + property.getLabel());
      console.log(" " + property.getGroupName());
      console.log(" " + property.getDeviceName());
    });
  } catch (e) {
    return console.log(e.message);
  }

  setTimeout(indi.disconnect.bind(indi), 5000);
};

app();