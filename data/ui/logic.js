$(document).ready(() => init());

let g_controls = [];
let logger;

function init() {
    const cfg = getConfig();

    sender = new Sender();

    g_controls.push(new LogLine());
    g_controls.push(new LogsWindow());

    logger = new Logger(cfg.maxBufferedLogs);

    g_controls.push(new ModesSwitcher({
        batch: new ModeBatch(Object.assign(cfg, {sender})),
        batchSequence: new ModeBatchSequence(Object.assign(cfg, {sender})),
    }));
}

function log(...args) {
    logger ? logger.log(...args) : undefined
};

function onError(err) {
    log(err, "error");
}
