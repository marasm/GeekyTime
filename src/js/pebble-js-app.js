var initDone = false;
var tempScale = 'F';
var tempCorrect = 0;

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
  req.open('GET', "http://api.openweathermap.org/data/2.5/weather?" +
    "lat=" + latitude + "&lon=" + longitude, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var temperature = '--';
        var temperatureC = '--';
        var temperatureF = '--';
        var icon = '00'; 
        var location = 'Unknown';
        if (response && response.weather && response.weather.length > 0) {
          var weatherResult = response.weather[0];
          icon = weatherResult.icon;
        }  
        if (response && response.main && response.main.temp != null 
            && response.main.temp != 'undefined' && response.main.temp != ''
            && !isNaN(response.main.temp)) {
          temperatureC = Math.round(response.main.temp - 273.15 + tempCorrect);
          temperatureF = Math.round(temperatureC * 9 / 5 + 32);
          //assign temp based on settings
          if (tempScale == 'C')
          {
            temperature = temperatureC;
          }
          else
          {
            temperature = temperatureF;
          }
        }
        if (response && response.name ) {
          location = response.name;
        }
        console.log('Icon=' + icon);
        console.log('Temp=' + temperature);
        console.log('Temp C=' + temperatureC);
        console.log('Temp F=' + temperatureF);
        console.log('Location=' + location);

        Pebble.sendAppMessage({
          "icon":icon,
          "temperature":temperature + "",
          "location":location}, sendToWatchSuccess, sendToWatchFail);

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
    "temperature":"--",
    "location":"Unknown"
  });
}

function initConfigOptions()
{
  var tempScaleLS = localStorage.getItem('tempScale');
  if (tempScaleLS != null && tempScaleLS != 'undefined' && tempScaleLS.length == 1)
  {
    tempScale = tempScaleLS;
    console.log("Assigned tempScale from storage=" + tempScaleLS);
  }
  var tempCorrectLS = localStorage.getItem('tempCorrect');
  if (tempCorrectLS != null && tempCorrectLS != 'undefined' && !isNaN(tempCorrectLS))
  {
    tempCorrect = tempCorrectLS;
    console.log("Assigned tempCorrect from storage=" + tempCorrectLS);
  }
  else
  {
    tempCorrect = 0;
    console.log("Assigned default tempCorrect=0");
  }
}

function applyAndStoreConfigOptions(inOptions)
{
  if (inOptions != null && inOptions != 'undefined')
  {
    if (inOptions.tempScale != null && inOptions.tempScale.length == 1)
    {
      localStorage.setItem('tempScale', inOptions.tempScale);
      tempScale = inOptions.tempScale;

      localStorage.setItem('tempCorrect', inOptions.tempCorrect);
      tempCorrect = inOptions.tempCorrect;
    }
  }
}

var locationOptions = { "timeout": 15000, "maximumAge": 600000 };//15s, 10 minutes 


Pebble.addEventListener("ready",
                        function(e) {
                          console.log("JS - ready called" + e.ready);
                          if (!initDone)
                          {
                            console.log("JS - performing init tasks" + e.ready);
                            locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
                            initConfigOptions();
                            initDone = true;
                          }
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                          window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
                          console.log(e.type);
                        });

Pebble.addEventListener("webviewclosed",
                         function(e) {
                         console.log("webview closed");
                         var options = JSON.parse(decodeURIComponent(e.response));
                         console.log("Options = " + JSON.stringify(options));
                         applyAndStoreConfigOptions(options);
                         window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
                         });

Pebble.addEventListener("showConfiguration", 
                         function() {
                         console.log("showing configuration");
                         initConfigOptions();
                         Pebble.openURL('http://pebbleappcfg.herokuapp.com/GeekyTime/geekyTimeCfg.html?tempScale=' + tempScale);
                         });


