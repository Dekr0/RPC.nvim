local M = {}

function M.cwd()
    return vim.fs.basename(vim.fn.getcwd())
end

function M.open_as_workplace()
    return vim.fn.getcwd() == vim.api.nvim_buf_get_name(0)
end

-- Extract the name and extension of a file
-- If a file does not extension, name will be the same as the original filename,
--  and extension will be an empty string.
---@return string
---@return string
function M.get_name_ext()
    local full_name = vim.api.nvim_buf_get_name(0)
    if (string.len(full_name) == 0) then return "", "" end
    local basename = vim.fs.basename(full_name)

    -- Find the last matched of the "." character since it's common to see 
    -- files to have multiple dots in their names
    -- We need an extra temporary variable to keep tract the previous "." 
    -- character if the next iteration find no more "." character and set our
    -- index to nil.
    local prev_idx, next_idx = string.find(basename, "%.")
    while next_idx do
      next_idx, _ = string.find(basename, "%.", prev_idx + 1)
      if next_idx then prev_idx = next_idx else break end
    end

    local filename = ""
    local ext = ""
    if prev_idx then
        filename = string.sub(basename, 0, prev_idx - 1)
        ext = string.sub(basename, prev_idx + 1)
    else
        filename = basename
    end

    return filename, ext
end

return M
