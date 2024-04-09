# (richpresence.nvim) - Discord Rich Presence Integration for Neovim (WIP)

- WARNING: This plugin is still under development and unstable.

- This plugin only available on UNIX based platform because it uses Discord's
 GameSDK to interact with the Discord process running in a user machine. The 
 GameSDK are a bundle of C/C++ shared libraries so each platform / architecture 
 needs to use a version of GameSDK that is compiled toward that platform / 
 architecture. The alternative way I found for this is to interact Discord 
 process using RPC on top of WebSocket but this feature is currently in closed 
 beta. Discord only allows developers that are invited to use this feature. The 
 current way of integrate Discord rich presence is suggested in Discord Developer 
 Portal but if you have better way of doing this I'm more than welcome to 
 take your suggestion and rework my implementation.
- I will also add support for Windows because I use dual-boot in my daily 
workflow. MacOS will also be supported but I don't have a MacOS based machine to
test out compile and build process.
 
## TODO (Requirement)

- Build process
- Message passing between richpresence.nvim and middleware
- Allow users to configure how status of rich presence should be display in 
Discord rich presence
- Refactor and address any potential bad practices and performance bad spot 
in Lua and C
    - Encapsulate state in Classes in Lua
    - Swap out `select` as a primary polling mechanism in C
        - `libevent`?
    - partial send
    - timing
    - message queue
- Support for other operating systems

## TODO (Future Plans)

- Users can request interaction with a user using Neovim
    - Share configuration?

## Development Notes

### `vim.fn.jobstart`

- Callbacks for `stdout` and `stderr` passed into jobstart will not run unless 
the middleware flushed `stdout` and `stderr` respectively
    - Flush is also required when writing into the log file.
    - Content is unavailable until middleware is closed
- Callbacks may receive partial (incomplete lines)
