-- kernel.lua
local Kernel = {
    version = "chiquiOS 1.0 Core",
    processes = {},

    boot = function(self)
        print("[" .. self.version .. "] Booting system...")
        self:create_process("Escritorio GUI", true)
        self:create_process("N-Net Browser", false)
    end,

    create_process = function(self, name, active)
        table.insert(self.processes, { name = name, running = active })
    end,

    -- Petición a Internet real
    http_get = function(self, url)
        local http = require("socket.http")
        local body, code = http.request(url)
        if code == 200 then
            return body
        else
            return "Error al conectar con " .. url
        end
    end
}

return Kernel
