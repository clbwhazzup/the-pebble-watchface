var Clay = require('@rebble/clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};
function weatherCodeToCondition(code) {
  if (code === 0) return '1';
  if (code === 1) return '9';
  if (code === 2) return '9';
  if (code <= 3) return '2';
  if (code <= 48) return '2';
  if (code <= 55) return '3';
  if (code <= 57) return '3';
  if (code <= 65) return '4';
  if (code <= 67) return '4';
  if (code <= 86) return '5';
  if (code === 95) return '8';
  if (code === 96) return '6';
  if (code <= 99) return '7';
  return 'Unknown';
}
function fetchCity(lat, lon, callback) {
  var url = 'https://api.bigdatacloud.net/data/reverse-geocode-client?latitude='
    + lat +'&longitude=' + lon + '&localityLanguage=en';

  xhrRequest(url, 'GET', function(responseText) {
    var json = JSON.parse(responseText);
    var name = '';
    if (json.locality) name = json.locality;
    else if (json.city) name = json.city;
    else if (json.principalSubdivision) name = json.principalSubdivision;
    if (callback) callback(name);
  });
}

function locationSuccess(pos) {
  var url = 'https://api.open-meteo.com/v1/forecast?' +
      'latitude=' + pos.coords.latitude +
      '&longitude=' + pos.coords.longitude +
      '&current=temperature_2m,relative_humidity_2m,weather_code' +
      '&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset' +
      '&timezone=auto';

  xhrRequest(url, 'GET',
    function(responseText) {
      var json = JSON.parse(responseText);

      var temperature = Math.round(json.current.temperature_2m);
      var conditions = weatherCodeToCondition(json.current.weather_code);
      var humidity = Math.round(json.current.relative_humidity_2m);
      var high = Math.round(json.daily.temperature_2m_max[0]);
      var low = Math.round(json.daily.temperature_2m_min[0]);
      var sunrise = json.daily.sunrise[0];
      var sunset = json.daily.sunset[0];
      // keep only the final 5 chars (e.g. "06:12")
      if (sunrise && sunrise.length > 5) sunrise = sunrise.slice(-5);
      if (sunset  && sunset.length  > 5) sunset  = sunset.slice(-5);

      // first retrieve city asynchronously before sending dictionary
      fetchCity(pos.coords.latitude, pos.coords.longitude, function(cityName) {
        var dictionary = {
          'TEMPERATURE': temperature,
          'CONDITIONS': conditions,
          'HUMIDITY': humidity,
          'HIGH': high,
          'LOW': low,
          'SUNRISE': sunrise,
          'SUNSET': sunset,
          'CITY': cityName || ''
        };
        Pebble.sendAppMessage(dictionary,
          function(e) { console.log('Weather info sent!'); },
          function(e) { console.log('Error sending weather info!'); }
        );
      });
    }
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {

  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    { enableHighAccuracy: true, timeout: 15000, maximumAge: 60000 }
  );
}
Pebble.addEventListener('ready',
  function(e) {
    console.log('PebbleKit JS ready!');
    getWeather();
  }
);

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    if (e.payload['REQUEST_WEATHER']) {
      getWeather();
    }
  }
);
