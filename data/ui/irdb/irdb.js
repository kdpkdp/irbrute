"use strict";

const IR_DB = {};

function loadSignalsArray() {
    const result = [];
    Object.keys(IR_DB).forEach(key => {
        IR_DB[key].signals.forEach(s => {
            result.push(makeRawSignal(key, s));
        });
    });
    return result;
}

function makeRawSignal(key, s) {
    return {
        id: `_${s.id}_${key}|${s.protocol}`,
        data: formatRawSignalData(s),
    };
}

function formatRawSignalData(s) {
    let data = "Raw:";
    data += `${s.frequency}|`;
    s.data.forEach(dur => {
        data += `${dur}|`;
    });
    return data;
}
