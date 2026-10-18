-- main.lua
local Kernel = require("kernel")
local Viewport = require("viewport")
local GUI = require("gui")

function love.load()
    Kernel:boot()
    local w, h = love.graphics.getDimensions()
    Viewport:update_scale(w, h)
end

function love.resize(w, h)
    Viewport:update_scale(w, h)
end

function love.draw()
    love.graphics.clear(0, 0, 0)

    love.graphics.push()
    love.graphics.translate(Viewport.offsetX, Viewport.offsetY)
    love.graphics.scale(Viewport.scale, Viewport.scale)

    -- Dibujar la interfaz
    GUI.draw(Viewport)

    love.graphics.pop()
end

function love.mousereleased(x, y, button)
    local vx, vy = Viewport:to_virtual(x, y)
    GUI.handleClick(vx, vy, Viewport)
end
