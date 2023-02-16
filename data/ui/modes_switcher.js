class ModesSwitcher {
    constructor(modes) {
        this.m_root = $("#modes_switcher_root");
        this.m_root.append(VIEW_MODE_SWITCHER);

        this.m_modes = [
            {btn: this.m_root.find("#btn_mode_batch"), mode: modes.batch, name: "BATCH"},
            {btn: this.m_root.find("#btn_mode_batch_sequence"), mode: modes.batchSequence, name: "BATCH SEQUENCE"},
        ];

        this.m_modes.forEach(mode => {
            mode.btn.click(() => {
                this.switchToMode(mode);
            });
        });

        this.switchToMode(this.m_modes[0]);
    }

    switchToMode(mode) {
        this.offAllModes();
        mode.btn.attr("class", "ctrl_btn bg_lblue");
        mode.mode.show();
        log(`Switch to the '${mode.name}' mode`);
    }

    offAllModes() {
        Object.keys(this.m_modes).forEach(key => {
            const mode = this.m_modes[key];
            mode.btn.attr("class", "ctrl_btn bg_dblue");
            mode.mode.hide();
        });
    }
}

const VIEW_MODE_SWITCHER = `
<div class="hor_widgets_layout">
    <div class="widget_layout">
        <div id="btn_mode_batch" class="ctrl_btn bg_lblue">
            <div class="ctrl_btn_text">
                Batch
            </div>
        </div>
    </div>
    <div class="widget_layout">
        <div id="btn_mode_batch_sequence" class="ctrl_btn bg_lblue">
            <div class="ctrl_btn_text">
                Batch Sequence
            </div>
        </div>
    </div>
</div>
`;
