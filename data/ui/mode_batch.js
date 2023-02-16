class ModeBatch {
    constructor(params) {
        this.m_signalsCount = params.signalsCount;
        this.m_range = {first: 0, last: this.m_signalsCount - 1};
        this.m_root = $("#mode_batch");
        this.m_root.append(VIEW_MODE_BATCH);
        this.m_viewInfo = new InfoLineBlock(this.m_root.find("#info_line_root"));
        this.m_viewFirst = new RangeBlock(this.m_root.find("#batch_first_value_root"), this.m_range, false);
        this.m_viewLast = new RangeBlock(this.m_root.find("#batch_last_value_root"), this.m_range, false);
        this.m_viewSps = new SpsBlock({
            root: this.m_root.find("#sps_root"),
            sender: params.sender,
            onSend: () => params.sender.send(this.getSelectedBatch()),
        });
        this.m_viewSending = new SendingBlock({
            root: this.m_root.find("#sending_root"),
            sender: params.sender,
        });

        this.m_viewFirst.setValueListener(() => this.onFirstValueChanged());
        this.m_viewLast.setValueListener(() => this.onLastValueChanged());

        this.updateInfo();
    }

    onFirstValueChanged() {
        const batch = this.getSelectedBatch();
        if (batch.first > batch.last) {
            this.m_viewLast.setValue(batch.first);
        }
        this.updateInfo();
    }

    onLastValueChanged() {
        const batch = this.getSelectedBatch();
        if (batch.last < batch.first) {
            this.m_viewFirst.setValue(batch.last);
        }
        this.updateInfo();
    }

    updateInfo() {
        const first = this.m_viewFirst.getValue();
        const last = this.m_viewLast.getValue();
        const batch = this.getSelectedBatch();
        let batchInfo = "";
        batchInfo += `Batch [ ${batch.first} ... ${batch.last} ]`;
        batchInfo += ` = ${batch.last - batch.first + 1}/${this.m_signalsCount}`;
        this.m_viewInfo.setInfo(batchInfo);
    }

    getSelectedBatch() {
        return {
            first: this.m_viewFirst.getValue(),
            last: this.m_viewLast.getValue(),
        };
    }

    hide() {
        this.m_root.hide();
    }

    show() {
        this.m_root.show();
    }
}

const VIEW_MODE_BATCH = `
  <div style="position: relative">
    <div style="height: 2em"></div>
    <div id="info_line_root"></div>
    <div style="height: 1em"></div>
    <div id="batch_first_value_root"></div>
    <div style="height: 1em"></div>
    <div id="batch_last_value_root"></div>
    <div style="height: 1em"></div>    
    <div id="sps_root"></div>
    <div id="sending_root" class="sending_root"></div>
  <div style="position: relative">
`;
