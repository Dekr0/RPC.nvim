local M = {}

function M.cwd()
    return vim.fs.basename(vim.fn.getcwd())
end

function M.open_as_workplace()
    return vim.fn.getcwd() == vim.api.nvim_buf_get_name(0)
end

function M.cf()
    local full_name = vim.api.nvim_buf_get_name(0)
    if (string.len(full_name) == 0) then return "", "" end
    local raw = vim.fs.basename(full_name)
    local l, h = string.find(raw, "%.")
    while h do
      h, _ = string.find(raw, "%.", l + 1)
      if h then l = h else break end
    end
    local filename = ""
    local ext = ""
    if l then
        filename = string.sub(raw, 0, l - 1)
        ext = string.sub(raw, l + 1)
    else
        filename = raw
    end

    return filename, ext
end

return M
