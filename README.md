# `RPC.nvim` - Discord Rich Presence For Neovim

- WARNING! This plugin is still work in progress despite I have used for a few months.

- This plugin only work within one instance of Neovim. In other word, if there are multiple instance of Neovim, it will disrupt communication inside the pipe / unix domain socket with Discord.

- This is my take on implementation of Discard Rich Presence Integration for Neovim.

## Table of Contents

- [Installation & Usage](#installation-and-usage)
    - [Prerequisites](#prerequisites)
        - [Why C](#why-c)
    - [Installation Instructions](#installation-instructions) 
        - [ipc_path](#ipc-path) (Important !!!)

## Installation and Usage <a name="installation-and-usage"></a>

- Currently, I haven't implement any integration for popular plugin managers such as `lazy.nvim` yet.
- Thus, installation would be tedious and error-prone.

### Prerequisites <a name="prerequisites"></a>

- Make sure you local machine have following tools install
    - `cmake`
    - `make`
    - `gcc`
- They are used for compiling C source code into C modules that can be imported directly into Lua.
    - The extension of C module for Lua is `.so` in Linux and `.dll` in Windows.
- Make sure you local machine have Lua 5.1 install.
    - It comes with some header files and library codes for developing extension for Lua.

#### Why C? (Skip if you want to) <a name="why-c"></a>

- The communication between Discord and a program you want to integrate Rich Presence happen in a pipe (Windows) / a unix domain socket (Linux).
- Thus, the communication uses binary message format where all data need to be serialized (packed) into a series of bytes.
- In order to communicate with Discord via a pipe / unix domain socket, there's a simple messaging protocol needed to follow.
- Discord process only recognizes by the following binary message structure. It is presented represented via a single byte array.
```
| opcode (32 bit unsigned integer in little endian) | length of stringify JSON data (32 bit unsigned integer in little endian) | stringify JSON data |
```
- There's already a builtin function to encode a table into string JSON data in Neovim.
- However, since Neovim use LuaJIT which does not have builtin binary serialization function in Lua 5.3 (`string.pack` and `string.unpack`), thus serializing other Lua data type into the right binary form in the right byte order is a problem.
- There are some Lua library available in open source that attempt to implement these binary serialization from Lua 5.3, which written purely in Lua.
- However, I still decide to write my implementation since the following.
    - It make more sense and more convenience to perform binary serialization in lower level due to the access of bit operators on byte and byte array.
    - I only need to (de)serialize 32 bit unsigned integer. I don't need the extra serializations for other data type which make the binary serialization implementation larger than my use case.

- Secondly, Discord process requires each message to include UUID4 in each JSON payload.
- Again, it's more convenience to implement UUID4 based on steps RFC9562 mention in lower level due to the access of bit operators on byte and byte array.

- In terms of performance wise, I won't make a conclusion until a benchmark is done. 

### Installation Instructions <a name="installation-instructions"></a>

- Below instruction are all done in a terminal shell.

1. Clone the main branch of this repository into a place you want to store it.
    - As an example and for the sake of clarification, let say you clone it to your home directory.
    - Path to this repository is `~/RPC.nvim`. I will use this path as an example throughout the installation instruction. Replace it if yours is somewhere else.
2. `cd ~/RPC.nvim`
3. `make` to compile two necessary C modules for Lua, `pack.so` (`pack.dll`) and `uuid.so` (`uuid.dll`) respectively.
    - These two C modules are in the `RPC.nvim/lua` folder.
    - Please don't move them around. Otherwise, the Lua source code for this plugin won't be able to locate them.
- If you're using `lazy.nvim`, you can install this plugin by specifying the following
```lua
{
    "Dekr0/RPC.nvim",
    dir = "~/RPC.nvim", -- Important!!! Please replace this with the absolute path of this repository in your local machine
    lazy = false,
    config = function()
        local RPC = require("RPC")

        RPC:setup({
            apm               = false, -- Not yet implemented
            auto_update       = true,
            auto_update_timer = 10000, -- Must be greater 5000 (5 seconds)
            ipc_path          = "/run/user/1000/discord-ipc-0",
            log_level         = 50     -- 50 => Fatal
        })

        vim.keymap.set("n", "<leader>rl", function () RPC:show_log() end)

        -- Pausing the update loop
        vim.keymap.set("n", "<leader>rp", function () RPC:pause_update() end)

        -- Resuming the update loop
        vim.keymap.set("n", "<leader>rr", function () RPC:resume_update() end)
    end
}
```
- If you're using other plugins manager or no plugins manager, the configuration should be identical.
- The only difference is that you need ensure the plugin manager or Neovim know where the module / source code locate at so that it can be imported.
    - For `lazy.nvim`, it will try to locate the `lua` folder inside the specify repository directory.
        - Please consult with official documentation of `lazy.nvim` for guaranteed correctness.
    - As an example, the given repository directory is `~/RPC.nvim`, then it will look for `~/RPC.nvim/lua`. 

#### `ipc_path` <a name="ipc-path"></a> 

- The location of the pipe / unix domain socket to communicate with Discord largely varied from operate systems.
- Typically, the pipe / unix domain socket will have name of `discord-ipc-x` where `x` range from `0` to `9`.
- Please do it local search in your disk to locate this file.
- In Archlinux, one possible location is in `/run/usr/1000/` folder.

