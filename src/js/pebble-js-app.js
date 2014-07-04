function sendToWatchSuccess(e)
{
  console.log("Message sent to watch successfully!");
}
function sendToWatchFail(e)
{
  console.log("Message sending failed: " + e.error.message);
}

function fetchWeather(latitude, longitude) {
  console.log("JS About to fetch weather from web...");
  var response;
  var req = new XMLHttpRequest();
  req.open('GET', "http://api.openweathermap.org/data/2.1/find/city?" +
    "lat=" + latitude + "&lon=" + longitude + "&cnt=1", true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var temperature,temperatureC,temperatureF, icon, location;
        if (response && response.list && response.list.length > 0) {
          var weatherResult = response.list[0];
          temperatureC = Math.round(weatherResult.main.temp - 273.15);
          temperatureF = Math.round(temperatureC * 9 / 5 + 32);
          //TODO assign temp based on settings
          temperature = temperatureF;
          icon = weatherResult.weather[0].icon;
          location = weatherResult.name;
          console.log('Icon=' + icon);
          console.log('Temp C=' + temperatureC);
          console.log('Temp F=' + temperatureF);
          console.log('Location=' + location);
          Pebble.sendAppMessage({
            "icon":icon,
            "temperature":temperature + "",
            "location":location}, sendToWatchSuccess, sendToWatchFail);
        }

      } else {
        console.log("Error");
      }
    }
  }
  req.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
    "icon":"00",
    "location":"Unknown",
    "temperature":"--"
  });
}

var locationOptions = { "timeout": 15000, "maximumAge": 600000 };//15s, 10 minutes 


Pebble.addEventListener("ready",
                        function(e) {
                          console.log("connect!" + e.ready);
                          locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
                          console.log(e.type);
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                          window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
                          console.log(e.type);
                          console.log(e.payload.temperature);
                          console.log("message!");
                        });

Pebble.addEventListener("webviewclosed",
                         function(e) {
                         console.log("webview closed");
                         var options = JSON.parse(decodeURIComponent(e.response));
                         console.log("Options = " + JSON.stringify(options));
                         console.log(e.type);
                         console.log(e.response);
                         });

Pebble.addEventListener("showConfiguration", 
                         function() {
                         console.log("showing configuration");
                         Pebble.openURL('http://pebbleappcfg.herokuapp.com/GeekyTime/geekyTimeCfg.html');
                         });


