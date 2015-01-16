Pebble.addEventListener("ready",
  function(e) {
    console.log("PebbleKit JS ready!");
  }
);

Pebble.addEventListener("showConfiguration",
  function(e) {
    var configurationSTR = localStorage.getItem("nazar_frames_config") || '{"showSecondHand": 0, "digitalWatch": 0}';
    var configuration = JSON.parse(configurationSTR);
    
    //Load the remote config page
    Pebble.openURL("http://codecorner.galanter.net/pebble/nazar_frames_1_x_config.htm?showSecondHand=" + configuration.showSecondHand + "&digitalWatch=" + configuration.digitalWatch);
    
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    var configurationSTR = JSON.stringify(configuration);
    console.log("Configuration window returned: " + configurationSTR);
    
    if (configurationSTR != '{}') {
      
      localStorage.setItem("nazar_frames_config", configurationSTR);
 
      //Send to Pebble, persist there
      Pebble.sendAppMessage(
          {
            "KEY_SHOW_SECOND_HAND": configuration.showSecondHand,
            "KEY_DIGITAL_WATCH": configuration.digitalWatch
          },
        function(e) {
          console.log("Sending settings data...");
        },
        function(e) {
          console.log("Settings feedback failed!");
        }
      );
    }
  }
);