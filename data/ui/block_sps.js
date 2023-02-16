class SpsBlock {
    constructor(params) {
        this.m_sender = params.sender;
        this.m_onSend = params.onSend || function () {};
        this.m_root = params.root;
        this.m_root.append(VIEW_SPS_BLOCK);

        this.m_btnSend = this.m_root.find("#btn_send");
        this.m_btnSend.click((event) => this.onBtnSend(event));

        this.m_btnPause = this.m_root.find("#btn_pause");
        this.m_btnPause.click((event) => this.onBtnPause(event));

        this.m_btnStop = this.m_root.find("#btn_stop");
        this.m_btnStop.click((event) => this.onBtnStop(event));

        this.m_textPause = this.m_root.find("#text_pause");

        this.m_sender.addStateListener(() => this.updateView());

        this.updateView();
    }

    withoutSendBtn() {
        this.m_btnSend.parent().hide();
    }

    withoutPauseBtn() {
        this.m_btnPause.parent().hide();
    }

    withoutStopBtn() {
        this.m_btnStop.parent().hide();
    }

    onBtnSend(event) {
        event.stopPropagation();
        if (this.m_sender.canSend()) {
            this.m_onSend();
        }
    }

    onBtnPause(event) {
        event.stopPropagation();
        if (this.m_sender.canPause()) {
            this.m_sender.pause();
            return;
        }

        if (this.m_sender.canResume()) {
            this.m_sender.resume();
            return;
        }
    }

    onBtnStop(event) {
        event.stopPropagation();
        if (this.m_sender.canStop()) {
            this.m_sender.stop();
        }
    }

    updateView() {
        const canSend = this.m_sender.canSend();
        const canPause = this.m_sender.canPause() || this.m_sender.canResume();
        const canStop = this.m_sender.canStop();

        const pauseClass = this.m_sender.isPaused() ? "ctrl_btn bg_orange" : "ctrl_btn bg_yellow";
        const pauseText = this.m_sender.isPaused() ? "RESUME" : "PAUSE";

        this.m_btnSend.attr("class", canSend ? "ctrl_btn bg_green" : "ctrl_btn bg_disabled");
        this.m_btnPause.attr("class", canPause ? pauseClass : "ctrl_btn bg_disabled");
        this.m_btnStop.attr("class", canStop ? "ctrl_btn bg_red" : "ctrl_btn bg_disabled");
        this.m_textPause.text(pauseText);
    }
}

const VIEW_SPS_BLOCK = `
    <div class="hor_widgets_layout no_border">
        <div class="widget_layout">
            <div id="btn_send" class="ctrl_btn bg_green">
                <div class="ctrl_btn_text">
                    SEND
                </div>
            </div>
        </div>
        <div class="widget_layout">
            <div id="btn_pause" class="ctrl_btn bg_yellow">
                <div id="text_pause" class="ctrl_btn_text">
                    PAUSE
                </div>
            </div>
        </div>
        <div class="widget_layout">
            <div id="btn_stop" class="ctrl_btn bg_red">
                <div class="ctrl_btn_text">
                    STOP
                </div>
            </div>
        </div>
    </div>
`;
