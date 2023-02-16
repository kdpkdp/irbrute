class InfoLineBlock
{
    constructor(root) {
        this.m_root = root;
        this.m_root.append(VIEW_INFO_LINE_BLOCK);
        this.m_info = this.m_root.find(".mode_info_line");
    }

    setInfo(info) {
        this.m_info.val(info);
    }
}

const VIEW_INFO_LINE_BLOCK = `
    <div class="mode_info_line_layout">
        <input class="mode_info_line" readonly>
    </div>
`;
