const SENDER_STATE_IDLE = "idle";
const SENDER_STATE_SENDING = "sending";
const SENDER_STATE_PAUSED = "paused";
const SENDER_STATE_PAUSING = "pausing";
const SENDER_STATE_STOPPING = "stopping";

class Sender {
    constructor() {
        this.m_state = SENDER_STATE_IDLE;
        this.m_stateListeners = [];
        this.m_progressListeners = [];
    }

    getState() {
        return this.m_state;
    }

    getRange() {
        return this.m_task ? this.m_task.getRange() : {};
    }

    addStateListener(fn) {
        if (fn) {
            this.m_stateListeners.push(fn);
        }
    }

    addProgressListener(fn) {
        if (fn) {
            this.m_progressListeners.push(fn);
        }
    }

    send(range) {
        if (this.canSend()) {
            this.restoreStateIfException(() => {
                const task = new SendTask({
                    range: range,
                    onDone: () => this.onTaskDone(),
                    onError: () => this.onTaskFailed(),
                    onProgress: (progress) => this.onTaskProgress(progress),
                });

                this.m_task = task;
                this.setState(SENDER_STATE_SENDING);
                task.start();
            });
        }
    }

    pause() {
        if (this.canPause()) {
            this.restoreStateIfException(async () => {
                this.setState(SENDER_STATE_PAUSING);
                await this.m_task.stop();
                this.setState(SENDER_STATE_PAUSED);
            });
        }
    }

    resume() {
        if (this.canResume()) {
            this.restoreStateIfException(() => {
                this.setState(SENDER_STATE_SENDING);
                this.m_task.start();
            });
        }
    }

    stop() {
        if (this.canStop()) {
            this.restoreStateIfException(async () => {
                this.setState(SENDER_STATE_STOPPING);
                await this.m_task.stop();
                this.onTaskDone();
            });
        }
    }

    restoreStateIfException(fn) {
        const state = this.m_state;
        try {
            return fn();
        } catch (e) {
            this.setState(state);
            throw e;
        }
    }

    isIdle() {
        return this.m_state === SENDER_STATE_IDLE;
    }

    canSend() {
        return this.isIdle();
    }

    isPaused() {
        return this.m_state === SENDER_STATE_PAUSED;
    }

    canPause() {
        return this.m_state === SENDER_STATE_SENDING;
    }

    canStop() {
        return this.m_state === SENDER_STATE_PAUSED || this.m_state === SENDER_STATE_SENDING;
    }

    canResume() {
        return this.m_state === SENDER_STATE_PAUSED;
    }

    setState(state) {
        if (this.m_state !== state) {
            const oldState = this.m_state;
            this.m_state = state;
            log(`Sender state changed: ${oldState} -> ${this.m_state}`);
            this.notifyAboutStateChanged();
        }
    }

    notifyAboutStateChanged() {
        this.m_stateListeners.forEach(fn => fn(this));
    }

    onTaskDone() {
        this.m_task = undefined;
        this.setState(SENDER_STATE_IDLE);
    }

    onTaskFailed() {
        this.pause();
    }

    onTaskProgress(progress) {
        this.m_progressListeners.forEach(fn => fn(progress));
    }
}

class SendTask {
    constructor(params) {
        if (!(params.range.first <= params.range.last)) {
            throw new Error(`Bad range: ${JSON.stringify(params.range)}`);
        }
        this.m_range = params.range;
        this.m_cursor = params.range.first;
        this.m_onDone = params.onDone;
        this.m_onError = params.onError;
        this.m_onProgress = params.onProgress;
        this.m_stopFlag = false;
    }

    getRange() {
        return this.m_range;
    }

    start() {
        if (!this.m_promise) {
            this.m_stopFlag = false;
            this.m_promise = this.sendLoop();
        }
    }

    async stop() {
        if (this.m_promise) {
            this.m_stopFlag = true;
            await this.m_promise;
            this.m_promise = undefined;
        }
    }

    isRetriableError(result) {
        return result.error && !![
            "Error 0x5: There is no any pack",
        ].find(err => err === result.error);
    }

    async sendLoop() {
        try {
            log("+ SEND LOOP");

            const rangeSize = this.m_range.last - this.m_range.first + 1;
            let err;
            let retriableErrorsCount = 0;
            while (this.m_cursor <= this.m_range.last && !this.m_stopFlag) {
                log(`sending ${this.m_cursor}  [${this.m_cursor - this.m_range.first + 1}/${rangeSize}] ...`, retriableErrorsCount ? "warn" : "info");

                try {
                    const result = await this.send(this.m_cursor);
                    if (result.error) {
                        if (!this.isRetriableError(result) || retriableErrorsCount >= 2) {
                            throw new Error(result.error);
                        }
                        ++retriableErrorsCount;
                        log(`sent    ${this.m_cursor}  [${this.m_cursor - this.m_range.first + 1}/${rangeSize}]: failed with retriable error: ${JSON.stringify(result, null, "  ")}`, "warn");
                        continue;
                    }

                } catch (e) {
                    const errMsg = formatExceptionLog(e, `Failed to send ${this.m_cursor}`);
                    log(errMsg, "error");
                    err = new Error(errMsg);
                    break;
                }

                log(`sent    ${this.m_cursor}  [${this.m_cursor - this.m_range.first + 1}/${rangeSize}]`);

                this.m_onProgress({sent: this.m_cursor});

                retriableErrorsCount = 0;
                ++this.m_cursor;
            }

            this.m_promise = undefined;

            if (err) {
                log(formatExceptionLog(err, "TASK FAILED"), "error");
                this.m_onError(err);
                return;
            }

            if (this.m_stopFlag) {
                log("TASK PAUSED");
                return;
            }

            log("TASK DONE");
            this.m_onDone();

        } finally {
            log("- SEND LOOP");
        }
    }

    async send(signalId) {
        return execSendCommand(signalId);
    }
}

function execSendCommand(signalId) {
    const signal = getConfig().signals[signalId];
    const req = {
        url: `/exec/ir/send?signalId=${signal.id}`,
        timeout: 10000,
        method: "POST",
        headers: {
            "Accept": "plain/text",
            "Content-Type": "plain/text",
        },
        body: signal.data,
    };
    return request(req);
}

function dummyRequest() {
    return new Promise((resolve, reject) => {
        setTimeout(resolve, 100);
        resolve({success: true});
    });
}

async function request(req) {
    const startTime = new Date().getTime();
    let id = `${startTime}`;
    id = id.substr(id.length - 7);

    const urlSearchParams = new URLSearchParams(req.url);
    const params = Object.fromEntries(urlSearchParams.entries());
    const values = Array.from(urlSearchParams.values());

    const logPrefix = `${id} | url='${req.url}' =>`;
    try {
        log(`${logPrefix} ...`, "progress");
        log(`\n${values[0]}`);
        const abortCtrl = new AbortController();
        const fetchTimer = setTimeout(() => {
            log(`${logPrefix} TIMEOUT. Aborting...`, "warn");
            abortCtrl.abort();
        }, req.timeout);

        const resp = await fetch(req.url, {
            method: req.method,
            headers: req.headers,
            body: req.body,
            signal: abortCtrl.signal
        });
        clearTimeout(fetchTimer);
        const logDuration = `${new Date().getTime() - startTime} ms`;

        const respText = await resp.text();
        if (!resp.ok) {
            throw new Error(`${resp.status} : ${resp.statusText} : ${respText} | ${logDuration}`);
        }

        log(`${logPrefix} ${resp.statusText} : ${respText} | ${logDuration}`);

        if (resp.status == 200) {
          return {
              success: true,
              statusCode: resp.status,
          };
        }

        if (/^Error 0x[0-9A-Fa-f]+:.*/.test(respText)) {
            const codeStart = respText.indexOf("0x") + 2;
            const codeEnd = respText.indexOf((":"));
            const codeStr = respText.substring(codeStart, codeEnd);
            const codeVal = parseInt(codeStr, 16);
            return {
                success: false,
                statusCode: resp.status,
                errorCode: codeVal,
                error: respText,
            };
        }

        return {
            success: false,
            statusCode: resp.status,
            error: respText,
        };

    } catch (err) {
        const logDuration = `${new Date().getTime() - startTime} ms`;
        const errMsg = formatExceptionLog(err, logPrefix) + ` | ${logDuration}`;
        log(errMsg, "error");
        throw err;
    }
}
