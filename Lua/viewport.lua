-- viewport.lua
local Viewport = {
    virtual_w = 480,
    virtual_h = 320,
    scale = 1,
    offsetX = 0,
    offsetY = 0,

    update_scale = function(self, real_w, real_h)
        local scaleX = real_w / self.virtual_w
        local scaleY = real_h / self.virtual_h
        
        self.scale = math.min(scaleX, scaleY)
        self.offsetX = (real_w - (self.virtual_w * self.scale)) / 2
        self.offsetY = (real_h - (self.virtual_h * self.scale)) / 2
    end,

    to_virtual = function(self, screen_x, screen_y)
        local vx = (screen_x - self.offsetX) / self.scale
        local vy = (screen_y - self.offsetY) / self.scale
        return vx, vy
    end
}

return Viewport
