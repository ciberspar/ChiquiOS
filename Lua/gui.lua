-- gui.lua
local GUI = {
    menuInicioAbierto = false,
    ventanas = {
        browser = {
            titulo = "N-Net Browser (Web Real)",
            x = 40,
            y = 30,
            w = 300,
            h = 180,
            abierta = true,
            url = "net://chiquios.hub",
            contenido = "Bienvenido a chiquiOS!\nSistema operativo de fantasia cargado."
        }
    }
}

function GUI.draw(Viewport)
    -- Fondo Azul Escritorio
    love.graphics.setColor(0.15, 0.25, 0.4)
    love.graphics.rectangle("fill", 0, 0, Viewport.virtual_w, Viewport.virtual_h)

    -- Icono: Mi PC
    love.graphics.setColor(0.8, 0.8, 0.8)
    love.graphics.rectangle("fill", 15, 15, 24, 20)
    love.graphics.setColor(1, 1, 1)
    love.graphics.print("Mi PC", 12, 38)

    -- Icono: N-Net
    love.graphics.setColor(0.2, 0.6, 0.9)
    love.graphics.rectangle("fill", 15, 65, 24, 20)
    love.graphics.setColor(1, 1, 1)
    love.graphics.print("N-Net", 12, 88)

    -- Ventana del Navegador
    local v = GUI.ventanas.browser
    if v.abierta then
        -- Sombra
        love.graphics.setColor(0, 0, 0, 0.3)
        love.graphics.rectangle("fill", v.x + 4, v.y + 4, v.w, v.h)

        -- Cuerpo
        love.graphics.setColor(0.85, 0.85, 0.85)
        love.graphics.rectangle("fill", v.x, v.y, v.w, v.h)

        -- Barra de título
        love.graphics.setColor(0.1, 0.2, 0.6)
        love.graphics.rectangle("fill", v.x, v.y, v.w, 16)
        love.graphics.setColor(1, 1, 1)
        love.graphics.print(v.titulo, v.x + 4, v.y + 2)

        -- Botón Cerrar [X]
        love.graphics.setColor(0.8, 0.2, 0.2)
        love.graphics.rectangle("fill", v.x + v.w - 14, v.y + 2, 12, 12)
        love.graphics.setColor(1, 1, 1)
        love.graphics.print("x", v.x + v.w - 11, v.y)

        -- Barra URL
        love.graphics.setColor(1, 1, 1)
        love.graphics.rectangle("fill", v.x + 8, v.y + 22, v.w - 16, 14)
        love.graphics.setColor(0, 0, 0)
        love.graphics.print(v.url, v.x + 12, v.y + 22)

        -- Contenido
        love.graphics.setColor(0.95, 0.95, 0.95)
        love.graphics.rectangle("fill", v.x + 8, v.y + 40, v.w - 16, v.h - 48)
        love.graphics.setColor(0.2, 0.2, 0.2)
        love.graphics.printf(v.contenido, v.x + 12, v.y + 44, v.w - 24)
    end

    -- Barra de tareas
    love.graphics.setColor(0.7, 0.7, 0.75)
    love.graphics.rectangle("fill", 0, Viewport.virtual_h - 20, Viewport.virtual_w, 20)

    -- Botón START
    love.graphics.setColor(0.2, 0.7, 0.3)
    love.graphics.rectangle("fill", 2, Viewport.virtual_h - 18, 45, 16)
    love.graphics.setColor(1, 1, 1)
    love.graphics.print("START", 6, Viewport.virtual_h - 16)

    -- Menú Inicio
    if GUI.menuInicioAbierto then
        love.graphics.setColor(0.9, 0.9, 0.9)
        love.graphics.rectangle("fill", 2, Viewport.virtual_h - 100, 90, 80)
        love.graphics.setColor(0, 0, 0)
        love.graphics.print("chiquiOS v1.0\n\n- Apps\n- Kernel\n- Apagar", 6, Viewport.virtual_h - 94)
    end
end

function GUI.handleClick(vx, vy, Viewport)
    -- Botón Start
    if vx >= 2 and vx <= 47 and vy >= (Viewport.virtual_h - 18) and vy <= (Viewport.virtual_h - 2) then
        GUI.menuInicioAbierto = not GUI.menuInicioAbierto
    end

    -- Botón Cerrar Ventana [X]
    local v = GUI.ventanas.browser
    if v.abierta then
        if vx >= (v.x + v.w - 14) and vx <= (v.x + v.w - 2) and vy >= (v.y + 2) and vy <= (v.y + 14) then
            v.abierta = false
        end
    end
end

return GUI
