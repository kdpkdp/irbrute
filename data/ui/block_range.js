class RangeBlock {
    constructor(parent, range, ringed) {
        this.m_root = parent;
        this.m_ringed = ringed;
        this.m_root.append(VIEW_RANGE_BLOCK);

        this.m_viewValue = this.m_root.find("#range_value");
        this.m_viewSlider = this.m_root.find("#range_slider");
        this.m_viewSlider.on("input", () => this.onSliderInput());
        this.m_root.find("#btn_prev_value").click(() => this.updateValue(-1));
        this.m_root.find("#btn_next_value").click(() => this.updateValue(1));

        this.setRange(range);
    }

    getValue() {
        return this.m_value;
    }

    setValueListener(fn) {
        this.m_valueListener = fn;
    }

    setRange(range) {
        this.m_range = range;
        this.setValue(this.m_range.first);
        this.m_viewSlider.prop("min", this.m_range.first);
        this.m_viewSlider.prop("max", this.m_range.last);
    }

    setValue(value) {
        if (!(this.m_range.first <= value && value <= this.m_range.last)) {
            return;
        }

        //this.m_viewValue.text(`${value} / [${this.m_range.first} ... ${this.m_range.last}]`);
        this.m_viewValue.text(`${value}`);

        if (value != parseInt(this.m_viewSlider.prop("value"), 10)) {
            this.m_viewSlider.prop("value", value);
        }

        if (value != this.m_value) {
            this.m_value = value;
            this.notifyValueChanged();
        }
    }

    updateValue(dV) {
        let value = this.m_value + dV;

        if (this.m_ringed) {
            value = value >= this.m_range.first ? value : this.m_range.last;
            value = value <= this.m_range.last ? value : this.m_range.first;
        } else {
            value = value >= this.m_range.first ? value : this.m_range.first;
            value = value <= this.m_range.last ? value : this.m_range.last;
        }

        this.setValue(value);
    }

    onSliderInput() {
        this.setValue(parseInt(this.m_viewSlider.prop("value"), 10));
    }

    notifyValueChanged() {
        if (this.m_valueListener) {
            this.m_valueListener(this.m_value);
        }
    }
}

const VIEW_RANGE_BLOCK = `
    <div class="hor_widgets_layout no_border">
        <div class="widget_layout">
            <div id="btn_prev_value" class="ctrl_btn bg_grayblue">
                <div class="ctrl_btn_text"> < </div>
            </div>
        </div>
        <div class="widget_layout">
          <div class="slidecontainer">
            <div id="range_value" class="slider_value"></div>
            <input id="range_slider" title="" type="range" min="0" max="1" value="0" step="1" class="slider">
          </div>
        </div>
        <div class="widget_layout">
            <div id="btn_next_value" class="ctrl_btn bg_grayblue">
                <div class="ctrl_btn_text"> > </div>
            </div>
        </div>
    </div>
`;
