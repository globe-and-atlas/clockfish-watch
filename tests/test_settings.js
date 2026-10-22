const assert = require('node:assert/strict');
const Module = require('node:module');

global.Pebble = {addEventListener: function () {}};
const originalLoad = Module._load;
Module._load = function (request, parent, isMain) {
  if (request === '@rebble/clay') return class MockClay {};
  return originalLoad.call(this, request, parent, isMain);
};
const {configuration, settingsMessage} = require('../watchface/src/pkjs/index.js');
Module._load = originalLoad;

const settings = settingsMessage(JSON.stringify({
  SETTING_SCENE: '2', SETTING_DATE: '3', SETTING_MODE: '1',
  SETTING_MOTION: 'false', SETTING_BATTERY: '0'
}));
assert.deepEqual(settings, {
  SETTING_SCENE: 2, SETTING_DATE: 3, SETTING_MODE: 1,
  SETTING_BATTERY: 0, SETTING_MOTION: 0
});

assert.deepEqual(settingsMessage('{bad json'), {
  SETTING_SCENE: 0, SETTING_DATE: 0, SETTING_MODE: 2,
  SETTING_BATTERY: 1, SETTING_MOTION: 1
});
assert.equal(settingsMessage('{"SETTING_SCENE":99}').SETTING_SCENE, 0);

const date = configuration().find(item => item.messageKey === 'SETTING_DATE');
assert.deepEqual(date.options.map(item => item.label),
  ['Off', 'YYYY-MM-DD', 'MM/DD/YYYY', 'DD/MM/YYYY']);
console.log('Clockfish settings passed');
