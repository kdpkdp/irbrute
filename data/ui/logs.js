class LogLine {
    constructor() {
        this.m_root = $("#log_line_root");
        this.m_root.append(VIEW_LOG_LINE);
    }
}

class LogsWindow {
    constructor() {
        this.m_root = $("#logs_window_root");
        this.m_root.append(VIEW_LOGS_WINDOW);
    }
}

const VIEW_LOG_LINE = `
    <div class="hor_widgets_layout">
        <div class="widget_layout">
            <div class="log_line_layout">
                <input class="log_line" readonly>
            </div>
        </div>
    </div>
`;

const VIEW_LOGS_WINDOW = `
    <div class="logs_window">
        <div class="logs_btns_block">
            <div class="log_btns_layout">
                <div class="widget_layout">
                    <div id="btn_clear_logs" class="ctrl_btn bg_red">
                        <div class="ctrl_btn_text logs_panel_btn">
                            Clear
                        </div>
                    </div>
                </div>
                <div class="widget_layout">
                    <div id="btn_hide_logs" class="ctrl_btn bg_green">
                        <div class="ctrl_btn_text logs_panel_btn">
                            OK
                        </div>
                    </div>
                </div>
            </div>
        </div>
    
        <div class="logs_list_block">
            <div class="logs_list_layout">
                <div class="logs_list_scroll">
                    <div class="logs_list">
                    </div>
                </div>
            </div>
        </div>
    </div>
`;

function formatExceptionLog(err, msg) {
    let result = `${msg} | ${err}`;
    if (err.stack) {
        result += `\n${err.stack}`;
    }
    return result;
}

class Logger {
    constructor(maxBufferedLogs) {
        this.m_maxBufferedLogs = maxBufferedLogs;
        this.m_levels = {
            "info": {level: 0, clazz: "log_line"},
            "warn": {level: 1, clazz: "log_line_warning"},
            "error": {level: 2, clazz: "log_line_error"},
        };
        this.m_level = 0;
        this.m_logId = 1;
        this.m_logItems = [];
        this.m_logFn = (what, type = "log") => {
            let logFn = console[type];
            if (logFn) {
                logFn(what);
                return;
            }
            console.warn(`${type}:`, what);
        };

        this.init();
    }

    init() {
        this.m_root = $("#logs_window_root");
        this.m_elLogsRoot = this.m_root[0];
        this.m_elLogsList = this.m_root.find(".logs_list")[0];
        this.m_elLogLine = $("#log_line_root").find(".log_line")[0];
        this.m_elLogLine.addEventListener("click", () => {
            this.showLogs();
        });

        this.m_elBtnHideLogs = this.m_root.find("#btn_hide_logs")[0];
        this.m_elBtnHideLogs.addEventListener("click", () => {
            this.hideLogs();
        });

        this.m_elBtnClearLogs = this.m_root.find("#btn_clear_logs")[0];
        this.m_elBtnClearLogs.addEventListener("click", () => {
            this.clearLogs();
        });

        const oldLogFn = this.m_logFn;
        this.m_logFn = (...args) => {
            oldLogFn(...args);
            this.addLogItem(...args);
        };
    }

    log(...args) {
        this.m_logFn(...args);
    }

    isLogsVisible() {
        return this.m_elLogsRoot.style.visibility === "visible";
    }

    showLogs() {
        if (!this.isLogsVisible()) {
            this.updateLogsUi();
            this.m_elLogsRoot.style.visibility = "visible";
        }
    }

    hideLogs() {
        this.m_elLogsRoot.style.visibility = "hidden";
    }

    clearLogs() {
        this.m_logItems = [];
        this.m_logId = 1;
        this.clearLogLine();
        if (this.isLogsVisible()) {
            this.updateLogsUi();
        }
    }

    addLogItem(what, type) {
        const id = this.m_logId;
        this.m_logId += 1;
        const logItem = {id, what, type};
        this.updateLogLine(logItem);
        this.m_logItems.push(logItem);
        if (this.m_logItems.length > this.m_maxBufferedLogs) {
            this.m_logItems.shift();
        }
        if (this.isLogsVisible()) {
            this.addLogItemToUi(logItem);
        }
    }

    addLogItemToUi(logItem) {
        const elem = document.createElement('div');
        elem.innerText = this.formatLogMsg(logItem);
        elem.classList.add("logs_list_item");
        const style = this.getStyleForLogItem(logItem);
        Object.keys(style).forEach((key) => {
            elem.style[key] = style[key];
        });
        if (this.m_elLogsList.firstChild) {
            this.m_elLogsList.insertBefore(elem, this.m_elLogsList.firstChild);
        } else {
            this.m_elLogsList.appendChild(elem);
        }
    }

    updateLogsUi(logItem) {
        this.m_elLogsList.innerHTML = "";
        for (let i = 0, end = this.m_logItems.length; i != end; ++i) {
            this.addLogItemToUi(this.m_logItems[i]);
        }
    }

    clearLogLine(logItem) {
        this.m_level = 0;
        $(this.m_elLogLine).attr("class", this.m_levels["info"].clazz);
        this.m_elLogLine.value = "";
    }

    updateLogLine(logItem) {
        const levelAttrs = this.m_levels[logItem.type || "info"] || this.m_levels["info"];
        if (this.m_level < levelAttrs.level) {
            this.m_level = levelAttrs.level;
            $(this.m_elLogLine).attr("class", levelAttrs.clazz);
        }

        const msg = this.formatLogMsg(logItem);
        this.m_elLogLine.value = msg;
        const style = this.getStyleForLogLine(logItem);
        Object.keys(style).forEach((key) => {
            this.m_elLogLine.style[key] = style[key];
        });
    }

    getStyleForLogLine(logItem) {
        const logType = logItem.type || "info";
        const computedStyle = getComputedStyle(this.m_elLogLine);
        return {
            color: computedStyle.getPropertyValue(`--${logType}-txt-color`) || "#fff",
        };
    }

    getStyleForLogItem(logItem) {
        const style = this.getStyleForLogLine(logItem);
        return {color: style.color};
    }

    formatLogMsg(logItem) {
        const now = new Date();
        const nowFmt = `${this.ensureXX(now.getHours())}:${this.ensureXX(now.getMinutes())}:${this.ensureXX(now.getSeconds())}.${this.ensureXXX(now.getMilliseconds())}`;
        return `${nowFmt} | ${logItem.id} | ${logItem.what}`;
    }

    ensureXX = (val) => {
        if (val < 10) {
            return `0${val}`;
        }
        return `${val}`
    }

    ensureXXX = (val) => {
        if (val < 10) {
            return `00${val}`;
        }
        if (val < 100) {
            return `0${val}`;
        }
        return `${val}`
    }
}
