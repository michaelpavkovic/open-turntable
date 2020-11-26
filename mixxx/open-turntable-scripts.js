var OpenTurntable = {};

OpenTurntable.wheelTurn = function (channel, control, value, status, group) {
    var newValue = value - 64;
    // newValue *= (128.0 / 2400.0);
    newValue *= 0.07;
    var alpha = 1.0/8;
    var beta = alpha/32;
    // engine.scratchEnable(1, 2400, 33+1/3, alpha, beta);
    // engine.scratchTick(1, newValue);
    engine.setValue(group, 'jog', newValue);
}
