let _CONFIG_;

function getConfig() {
    _CONFIG_ = _CONFIG_ || loadConfig();
    return _CONFIG_;
}

function loadConfig() {
    const signals = loadSignalsArray();
    const signalsCount = signals.length;
    const maxSignalsPerBatch = signalsCount < 50 ? signalsCount : 50;
    return {
        signals,
        signalsCount,
        maxSignalsPerBatch,
        maxBufferedLogs: 100,
    };
}
