var OpenTurntable = {};

OpenTurntable.wheelTurn = function (channel, control, value, status, group) {
    var newValue = value - 64;
    newValue *= (128.0 / 2400.0);
    // newValue *= 0.07;
    var alpha = 1.0/8;
    var beta = alpha/32;
    // engine.scratchEnable(1, 2400, 33+1/3, alpha, beta);
    // engine.scratchTick(1, newValue);
    engine.setValue(group, 'jog', newValue);
}

// Events from Mixxx to handle cue and play LEDs
var cueIndicatorCallback = function(value, group, control) {
    midi.sendShortMsg(0x90, 0x0C, value * 0x7F);
};
var cueIndicatorConnection = engine.makeConnection('[Channel1]', 'cue_indicator', cueIndicatorCallback);

var playIndicatorCallback = function(value, group, control) {
    midi.sendShortMsg(0x90, 0x0B, value * 0x7F);
};
var playIndicatorConnection = engine.makeConnection('[Channel1]', 'play_indicator', playIndicatorCallback);

// engine.beginTimer(100, function() {
//     midi.sendShortMsg(0x90, 0x0D, engine.getValue('[Channel1]', playposition))
// });
 