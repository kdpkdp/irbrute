class SendingBlock {
    constructor(params) {
        this.m_root = params.root;
        this.m_root.append(VIEW_SENDING_BLOCK);
        this.m_viewSps = new SpsBlock({
            root: this.m_root.find("#sending_sps_root"),
            sender: params.sender,
        });
        this.m_viewSps.withoutSendBtn();
        this.m_rangeLine = this.m_root.find("#range_line");
        this.m_stateLine = this.m_root.find("#state_line");
        this.m_sentLine = this.m_root.find("#sent_line");

        params.sender.addStateListener((sender) => this.onSenderStateChanged(sender));
        params.sender.addProgressListener((progress) => this.onProgress(progress));

        this.m_root.click((event) => {
            if (params.sender.canPause()) {
                this.m_viewSps.onBtnPause(event);
            }
        });

        this.m_root.hide();
    }

    onSenderStateChanged(sender) {
        const range = sender.getRange();
        this.m_rangeLine.text(`range: [${range.first} ... ${range.last}]`);
        this.m_stateLine.text(`state: ${sender.getState()}`);
        if (sender.isIdle()) {
            this.m_sentLine.text("sent:");
            this.m_root.hide();
        } else {
            this.m_root.show();
        }
    }

    onProgress(progress) {
        this.m_sentLine.text(`sent: ${progress.sent}`);
    }
}

const VIEW_SENDING_BLOCK = `
    <div class="hor_widgets_layout no_border">
      <pre id="state_line" class="sending_info_line">state:</pre>
      <pre id="range_line" class="sending_info_line">state:</pre>
      <pre id="sent_line" class="sending_info_line">sent:</pre>
      <div id="sending_sps_root"></div>
    </div>
`;
