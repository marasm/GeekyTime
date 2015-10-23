var initDone = false;
var tempScale = 'F';
var tempCorrect = 0;
var btVibrate = 'On';
var dateFormat = 'mmdd';
var autoLocation = 'On';
var manLocation = '';
var owmAppId = '9b46f205cf161eb68ebcf12970587b88';

function sendToWatchSuccess(e)
{
  console.log("Message sent to watch successfully!");
}
function sendToWatchFail(e)
{
  console.log("Message sending failed: " + e);
}

function parseWeatherResponse() {
  if (this.readyState == 4) {
    if(this.status == 200) {
      console.log(this.responseText);
      var response = JSON.parse(this.responseText);
      var temperature = '--';
      var temperatureC = '--';
      var temperatureF = '--';
      var icon = '00';
      var location = 'Unknown';
      if (response && response.weather && response.weather.length > 0) {
        var weatherResult = response.weather[0];
        icon = weatherResult.icon;
      }
      if (response && response.main && response.main.temp !== null &&
          response.main.temp != 'undefined' && response.main.temp !== '' &&
          !isNaN(response.main.temp)) {
        temperatureC = response.main.temp - 273.15;
        console.log('temp C before correction=' + temperatureC);
        console.log('temp Correction=' + tempCorrect);
        temperatureC = Math.round(temperatureC + tempCorrect);
        console.log('temp C after correction=' + temperatureC);
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
        "temperature":temperature.toString(),
        "location":location}, sendToWatchSuccess, sendToWatchFail);

    }
    else
    {
      console.log("HTTP Error = " + this.status);
    }
  }
}

function fetchWeatherForCoords(latitude, longitude) {
  console.log("JS fetch weather from web for coords: " + latitude + "," +
    longitude);
  var req = new XMLHttpRequest();
  req.open('GET', "http://api.openweathermap.org/data/2.5/weather?" +
    "lat=" + latitude + "&lon=" + longitude + '&APPID=' + owmAppId, true);
  req.onload = parseWeatherResponse;
  req.send(null);
}


function fetchWeatherForStaticLocation(locationString) {
  console.log("JS fetch weather from web for static location: " +
      locationString);
  var req = new XMLHttpRequest();
  req.open('GET', "http://api.openweathermap.org/data/2.5/weather?" +
    "q=" + locationString + '&APPID=' + owmAppId, true);
  req.onload = parseWeatherResponse;
  req.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeatherForCoords(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  var errCode = 'UN';
  switch (err.code)
  {
    case err.TIMEOUT:
      errCode = 'TO';
      break;
    case err.POSITION_UNAVAILABLE:
      errCode = 'GE';
      break;
    case err.PERMISSION_DENIED:
      errCode = 'PD';
      break;
  }
  Pebble.sendAppMessage({
    "icon":"00",
    "temperature":"--",
    "location":"LocErr: " + errCode
  });
}

function initConfigOptions()
{
  var tempScaleLS = localStorage.getItem('tempScale');
  if (tempScaleLS !== null && tempScaleLS != 'undefined' && tempScaleLS.length == 1)
  {
    tempScale = tempScaleLS;
    console.log("Assigned tempScale from storage=" + tempScaleLS);
  }
  var tempCorrectLS = localStorage.getItem('tempCorrect');
  if (tempCorrectLS !== null && tempCorrectLS != 'undefined' && !isNaN(tempCorrectLS))
  {
    tempCorrect = parseFloat(tempCorrectLS);
    console.log("Assigned tempCorrect from storage=" + tempCorrectLS);
  }
  else
  {
    tempCorrect = 0;
    console.log("Assigned default tempCorrect=0");
  }
  var btVibrateLS = localStorage.getItem('btVibrate');
  if (btVibrateLS !== null && btVibrateLS != 'undefined' && btVibrateLS.length > 0)
  {
    btVibrate = btVibrateLS;
    console.log("Assigned btVibrate from storage=" + btVibrateLS);
  }
  
  var dateFormatLS = localStorage.getItem('dateFormat');
  if (dateFormatLS !== null && dateFormatLS != 'undefined' && dateFormatLS.length > 0)
  {
    dateFormat = dateFormatLS;
    console.log("Assigned dateFormat from storage=" + dateFormatLS);
  }

  var autoLocationLS = localStorage.getItem('autoLocation');
  if(autoLocationLS !== null && autoLocationLS != 'undefined' &&
     autoLocationLS.trim().length > 0)
  {
    autoLocation = autoLocationLS;
    console.log("Assigned autoLocation from storage=" + autoLocationLS);
  }
  var manLocationLS = localStorage.getItem('manLocation');
  if(manLocationLS !== null && manLocationLS != 'undefined' &&
     manLocationLS.trim().length > 0)
  {
    manLocation = manLocationLS;
    console.log("Assigned manLocation from storage=" + manLocationLS);
  }
  
  sendWatchConfigToWatch();
}

function applyAndStoreConfigOptions(inOptions)
{
  if (inOptions !== null && inOptions != 'undefined')
  {
    //these options are for the JS running on the phone
    if (inOptions.tempScale !== null && inOptions.tempScale.length == 1)
    {
      localStorage.setItem('tempScale', inOptions.tempScale);
      tempScale = inOptions.tempScale;
    }
    if (inOptions.tempCorrect !== null && !isNaN(inOptions.tempCorrect))
    {
      localStorage.setItem('tempCorrect', inOptions.tempCorrect);
      tempCorrect = parseFloat(inOptions.tempCorrect);
    }
    if (inOptions.autoLocation !== null && inOptions.autoLocation.trim().length > 0)
    {
      localStorage.setItem('autoLocation', inOptions.autoLocation);
      autoLocation = inOptions.autoLocation;
    }
    if (inOptions.manLocation !== null && inOptions.manLocation.trim().length > 0)
    {
      localStorage.setItem('manLocation', inOptions.manLocation);
      autoLocation = inOptions.manLocation;
    }

    //this option is applicable to watch app only so store and send to watch
    if (inOptions.btVibrate !== null && inOptions.btVibrate.length > 0)
    {
      localStorage.setItem('btVibrate', inOptions.btVibrate);
      btVibrate = inOptions.btVibrate;
    }

    if (inOptions.dateFormat !== null && inOptions.dateFormat.length > 0)
    {
      localStorage.setItem('dateFormat', inOptions.dateFormat);
      dateFormat = inOptions.dateFormat;
    }
    sendWatchConfigToWatch();
  }
}

//this function will send all the watch config options back to the watch (btVibrate, dateFormat and etc.)
function sendWatchConfigToWatch()
{
  console.log('Sending btVibrate=' + btVibrate + " to the watch");
  Pebble.sendAppMessage({
    "btVibrate" : btVibrate,
    "dateFormat" : dateFormat
  });
}

var locationOptions = { "timeout": 30000, "maximumAge": 600000 };//30s, 10 minutes

function getAppropriateWeatherData()
{
  if ('On' === autoLocation)
  {
    navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  }
  else
  {
    fetchWeatherForStaticLocation(manLocation);
  }
}

Pebble.addEventListener("ready",
                        function(e) {
                          console.log("JS - ready called " + e.ready);
                          if (!initDone)
                          {
                            console.log("JS - performing init tasks" + e.ready);
                            initConfigOptions();
                            initDone = true;
                            getAppropriateWeatherData();
                          }
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                          getAppropriateWeatherData();
                          console.log(e.type);
                        });

Pebble.addEventListener("webviewclosed",
                         function(e) {
                         console.log("webview closed");
                         var options = JSON.parse(decodeURIComponent(e.response));
                         console.log("Options = " + JSON.stringify(options));
                         applyAndStoreConfigOptions(options);
                         getAppropriateWeatherData();
                         });

Pebble.addEventListener("showConfiguration",
                         function() {
                         console.log("showing configuration +");
                         initConfigOptions();
                         Pebble.openURL('http://pebbleappcfg.herokuapp.com/GeekyTime/geekyTimeCfg.html?tempScale=' + tempScale + '&tempCorrect=' + tempCorrect + '&btVibrate=' + btVibrate + '&dateFormat=' + dateFormat + '&autoLocation=' + autoLocation + '&manLocation=' + manLocation + '&allowLocSelect=true');
                         });
