class ModeBatchSequence {
    constructor(params) {
        this.m_root = $("#mode_batch_sequence");
        this.m_root.append(VIEW_MODE_BATCH_SEQUENCE);
        this.m_viewInfo = new InfoLineBlock(this.m_root.find("#info_line_root"));
        this.m_viewRange = new RangeBlock(this.m_root.find("#range_root"), {first: 0, last: 0}, true);
        this.m_viewSps = new SpsBlock({
            root: this.m_root.find("#sps_root"),
            sender: params.sender,
            onSend: () => params.sender.send(this.getSelectedBatch()),
        });
        this.m_viewSending = new SendingBlock({
            root: this.m_root.find("#sending_root"),
            sender: params.sender,
        });

        const maxSignalsPerBatch = params.maxSignalsPerBatch || 100;
        this.m_signalsCount = params.signalsCount;
        this.m_maxSignalsPerBatch = this.m_signalsCount < maxSignalsPerBatch ? this.m_signalsCount : maxSignalsPerBatch;
        this.m_currentBatch = 0;
        this.m_batches = [];
        for (let i = 0; i + this.m_maxSignalsPerBatch <= this.m_signalsCount; i += this.m_maxSignalsPerBatch) {
            this.m_batches.push({first: i, last: i + this.m_maxSignalsPerBatch - 1});
        }
        const tie = this.m_signalsCount % this.m_maxSignalsPerBatch;
        if (tie > 0) {
            this.m_batches.push({first: this.m_signalsCount - tie, last: this.m_signalsCount - 1});
        }

        this.m_viewRange.setRange({first: 1, last: this.m_batches.length});
        this.m_viewRange.setValueListener((value) => this.setBatch(value - 1));

        this.setBatch(0);
    }

    setBatch(idx) {
        let newBatch = idx;
        newBatch = newBatch < 0 ? this.m_batches.length - 1 : newBatch;
        newBatch = newBatch < this.m_batches.length ? newBatch : 0;
        this.m_currentBatch = newBatch;
        this.updateInfo();
    }

    hide() {
        this.m_root.hide();
    }

    show() {
        this.m_root.show();
    }

    updateInfo() {
        const batch = this.getSelectedBatch();
        let batchInfo = "";
        batchInfo += `Batch ${this.m_currentBatch + 1}/${this.m_batches.length}  [ ${batch.first} ... ${batch.last} ]`;
        batchInfo += ` = ${batch.last - batch.first + 1}/${this.m_signalsCount}`;
        this.m_viewInfo.setInfo(batchInfo);
    }

    getSelectedBatch() {
        const batch = this.m_batches[this.m_currentBatch];
        return {first: batch.first, last: batch.last};
    }
}

const VIEW_MODE_BATCH_SEQUENCE = `
  <div style="position: relative">
    <div style="height: 2em"></div>
    <div id="info_line_root"></div>
    <div style="height: 1em"></div>
    <div id="range_root"></div>
    <div style="height: 5em"></div>
    <div id="sps_root"></div>
    <div id="sending_root" class="sending_root"></div>
  </div>    
`;
