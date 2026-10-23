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
  SETTING_FISH_COLOR: '1', SETTING_BACKGROUND: '5',
  SETTING_TIME_FONT: '5', SETTING_MOTION: 'false', SETTING_BATTERY: '0'
}));
assert.deepEqual(settings, {
  SETTING_SCENE: 2, SETTING_DATE: 3, SETTING_MODE: 1,
  SETTING_FISH_COLOR: 1, SETTING_BACKGROUND: 5,
  SETTING_TIME_FONT: 5, SETTING_BATTERY: 0, SETTING_MOTION: 0
});

assert.deepEqual(settingsMessage('{bad json'), {
  SETTING_SCENE: 0, SETTING_DATE: 0, SETTING_MODE: 2,
  SETTING_FISH_COLOR: 0, SETTING_BACKGROUND: 0,
  SETTING_TIME_FONT: 0, SETTING_BATTERY: 1, SETTING_MOTION: 1
});
assert.equal(settingsMessage('{"SETTING_SCENE":99}').SETTING_SCENE, 0);
assert.equal(settingsMessage('{"SETTING_FISH_COLOR":99,"SETTING_BACKGROUND":-1}').SETTING_FISH_COLOR, 0);
assert.equal(settingsMessage('{"SETTING_FISH_COLOR":99,"SETTING_BACKGROUND":-1}').SETTING_BACKGROUND, 0);
assert.equal(settingsMessage('{"SETTING_MOTION":"false"}').SETTING_MOTION, 0);
assert.equal(settingsMessage('{"SETTING_MOTION":"true"}').SETTING_MOTION, 1);
assert.equal(settingsMessage('{"SETTING_MOTION":"2"}').SETTING_MOTION, 2);
assert.equal(settingsMessage('{"SETTING_TIME_FONT":99}').SETTING_TIME_FONT, 0);
assert.equal(settingsMessage('{"SETTING_TIME_FONT":-1}').SETTING_TIME_FONT, 0);

const fishChoices = configuration().find(item => item.messageKey === 'SETTING_FISH_COLOR');
const backgroundChoices = configuration().find(item => item.messageKey === 'SETTING_BACKGROUND');
const fontChoices = configuration().find(item => item.messageKey === 'SETTING_TIME_FONT');
assert.deepEqual(fishChoices.options.map(item => item.label),
  ['Match aquarium', 'Orange', 'Blue', 'Turquoise', 'Grayscale', 'Violet', 'Sea green', 'Random daily']);
assert.deepEqual(backgroundChoices.options.map(item => item.label),
  ['Match aquarium', 'Castle', 'Kelp forest', 'Reef', 'Grayscale', 'Random daily']);
assert.deepEqual(fontChoices.options.map(item => item.label),
  ['Rounded', 'Digital bold', 'Digital fine', 'Classic bold', 'Compact bold', 'Tall light']);
assert.equal(fontChoices.defaultValue, '0');

const date = configuration().find(item => item.messageKey === 'SETTING_DATE');
assert.deepEqual(date.options.map(item => item.label),
  ['Off', 'YYYY-MM-DD', 'MM/DD/YYYY', 'DD/MM/YYYY']);
const motion = configuration().find(item => item.messageKey === 'SETTING_MOTION');
assert.deepEqual(motion.options.map(item => item.label), ['Still', 'Gentle', 'Lively']);
console.log('Clockfish settings passed');
