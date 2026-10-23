var Clay = require('@rebble/clay');

function configuration() {
  return [
    {type: 'heading', defaultValue: 'Clockfish'},
    {type: 'text', defaultValue: 'Aquarium life moves in short bursts. Tap to send the clock fish gliding when motion is enabled.'},
    {type: 'select', messageKey: 'SETTING_SCENE', label: 'Aquarium scene', defaultValue: '0', options: [
      {label: 'Castle aquarium', value: '0'},
      {label: 'Kelp forest', value: '1'},
      {label: 'Midnight reef', value: '2'},
      {label: 'Grayscale', value: '3'}
    ]},
    {type: 'select', messageKey: 'SETTING_FISH_COLOR', label: 'Clock fish color', defaultValue: '0', options: [
      {label: 'Match aquarium', value: '0'},
      {label: 'Orange', value: '1'},
      {label: 'Blue', value: '2'},
      {label: 'Turquoise', value: '3'},
      {label: 'Grayscale', value: '4'},
      {label: 'Violet', value: '5'},
      {label: 'Sea green', value: '6'},
      {label: 'Random daily', value: '7'}
    ]},
    {type: 'select', messageKey: 'SETTING_BACKGROUND', label: 'Background', defaultValue: '0', options: [
      {label: 'Match aquarium', value: '0'},
      {label: 'Castle', value: '1'},
      {label: 'Kelp forest', value: '2'},
      {label: 'Reef', value: '3'},
      {label: 'Grayscale', value: '4'},
      {label: 'Random daily', value: '5'}
    ]},
    {type: 'select', messageKey: 'SETTING_MODE', label: 'Day / night', defaultValue: '2', options: [
      {label: 'Always day', value: '0'},
      {label: 'Always night', value: '1'},
      {label: 'Automatic', value: '2'}
    ]},
    {type: 'select', messageKey: 'SETTING_DATE', label: 'Date', defaultValue: '0', options: [
      {label: 'Off', value: '0'},
      {label: 'YYYY-MM-DD', value: '1'},
      {label: 'MM/DD/YYYY', value: '2'},
      {label: 'DD/MM/YYYY', value: '3'}
    ]},
    {type: 'toggle', messageKey: 'SETTING_BATTERY', label: 'Show battery fish', defaultValue: true},
    {type: 'select', messageKey: 'SETTING_MOTION', label: 'Aquarium motion', defaultValue: '1', options: [
      {label: 'Still', value: '0'},
      {label: 'Gentle', value: '1'},
      {label: 'Lively', value: '2'}
    ]},
    {type: 'submit', defaultValue: 'Save to watch'}
  ];
}

var clay = new Clay(configuration(), null, {autoHandleEvents: false});

function asInt(value, low, high, fallback) {
  var number = Number(value);
  return isFinite(number) && Math.floor(number) === number && number >= low && number <= high ? number : fallback;
}

function asToggle(value, fallback) {
  if (value === true || value === 1 || value === 'true' || value === '1') return 1;
  if (value === false || value === 0 || value === 'false' || value === '0') return 0;
  return fallback;
}

function asMotion(value) {
  if (value === false || value === 'false') return 0;
  if (value === true || value === 'true') return 1;
  return asInt(value, 0, 2, 1);
}

function settingsMessage(raw) {
  var saved;
  try { saved = JSON.parse(raw) || {}; } catch (error) { saved = {}; }
  return {
    SETTING_SCENE: asInt(saved.SETTING_SCENE, 0, 3, 0),
    SETTING_FISH_COLOR: asInt(saved.SETTING_FISH_COLOR, 0, 7, 0),
    SETTING_BACKGROUND: asInt(saved.SETTING_BACKGROUND, 0, 5, 0),
    SETTING_DATE: asInt(saved.SETTING_DATE, 0, 3, 0),
    SETTING_MODE: asInt(saved.SETTING_MODE, 0, 2, 2),
    SETTING_BATTERY: asToggle(saved.SETTING_BATTERY, 1),
    SETTING_MOTION: asMotion(saved.SETTING_MOTION)
  };
}

function send(raw) {
  Pebble.sendAppMessage(settingsMessage(raw));
}

Pebble.addEventListener('ready', function () {
  var saved = localStorage.getItem('clay-settings');
  if (saved) send(saved);
});
Pebble.addEventListener('showConfiguration', function () { Pebble.openURL(clay.generateUrl()); });
Pebble.addEventListener('webviewclosed', function (event) {
  if (!event || !event.response || event.response === 'CANCELLED') return;
  try { clay.getSettings(event.response); } catch (error) { return; }
  send(localStorage.getItem('clay-settings'));
});

if (typeof module !== 'undefined') module.exports = {
  configuration: configuration,
  settingsMessage: settingsMessage
};
