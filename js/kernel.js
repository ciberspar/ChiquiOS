import { initBaco } from './baco.js';

class ChiquiOSKernel {
  constructor() {
    this.vfs = JSON.parse(localStorage.getItem('chiqui_vfs_tree')) || {
      "/home/user/welcome.txt": "Bienvenido a chiquiOS con un Kernel de Linux real emulado por v86."
    };
    this.packages = JSON.parse(localStorage.getItem('chiqui_pkgs')) || ["alpine-base", "v86-emulator"];
    this.activeWindows = {};
    this.zIndexCounter = 100;
    this.emulatorInstance = null;
  }

  saveState() {
    localStorage.setItem('chiqui_vfs_tree', JSON.stringify(this.vfs));
    localStorage.setItem('chiqui_pkgs', JSON.stringify(this.packages));
  }

  openApp(appId) {
    if (this.activeWindows[appId]) {
      this.bringToFront(appId);
      document.getElementById(`win-${appId}`).classList.remove('minimized');
      return;
    }

    this.zIndexCounter++;
    const win = document.createElement('div');
    win.className = 'window';
    win.id = `win-${appId}`;
    win.style.zIndex = this.zIndexCounter;
    win.style.top = `${50 + (Object.keys(this.activeWindows).length * 20)}px`;
    win.style.left = `${80 + (Object.keys(this.activeWindows).length * 30)}px`;
    win.style.width = appId === 'terminal' ? '640px' : '420px';
    win.style.height = appId === 'terminal' ? '440px' : '320px';

    let title = "Ventana";
    let contentHtml = "";

    if (appId === 'terminal') {
      title = "root@chiquiOS: ~ (Alpine Linux Real - v86)";
      contentHtml = `<div id="v86-container" style="width:100%; height:100%; background:#000; display:flex; flex-direction:column; position:relative;">
        <div style="background:#111; color:#00ff66; padding:4px 8px; font-size:0.75rem; border-bottom:1px solid #00f3ff;">
          Cargando BIOS y Kernel x86 (Alpine Linux)... Aguarda un momento.
        </div>
        <div id="screen_container" style="flex:1; overflow:hidden; position:relative;">
          <div style="white-space: pre; font-family: monospace; line-height: 14px;" id="screen"></div>
        </div>
      </div>`;
    } else if (appId === 'baco') {
      title = "Baco Daemon // Monitor Oficial";
      contentHtml = initBaco();
    } else if (appId === 'texteditor') {
      title = "NanoPad // Editor VFS";
      contentHtml = `<div style="display:flex; flex-direction:column; height:100%; gap:8px;">
        <input type="text" id="nano-path" value="/home/user/welcome.txt" style="background:#02050b; border:1px solid #00f3ff; color:#fff; padding:4px; font-family:monospace;">
        <textarea id="nano-text" style="flex:1; background:#02050b; color:#00ff66; border:1px solid #00f3ff; padding:8px; font-family:monospace; resize:none;">${this.vfs['/home/user/welcome.txt'] || ''}</textarea>
        <button id="nano-save" style="background:rgba(0,243,255,0.2); border:1px solid #00f3ff; color:#00f3ff; padding:6px; cursor:pointer; font-family:monospace;">Guardar en VFS</button>
      </div>`;
    }

    win.innerHTML = `
      <div class="window-header" data-appid="${appId}">
        <span>${title}</span>
        <div class="window-controls">
          <button class="win-btn btn-min" data-action="min" data-appid="${appId}"></button>
          <button class="win-btn btn-max" data-action="max" data-appid="${appId}"></button>
          <button class="win-btn btn-close" data-action="close" data-appid="${appId}"></button>
        </div>
      </div>
      <div class="window-content" style="padding:0; overflow:hidden;">${contentHtml}</div>
    `;

    document.getElementById('desktop').appendChild(win);
    this.activeWindows[appId] = win;
    this.updateTaskbar();
    this.setupWindowEvents(appId);

    if (appId === 'terminal' && typeof V86Starter !== 'undefined') {
      // Inicializar v86 con una imagen de Alpine Linux optimizada para web
      try {
        this.emulatorInstance = new V86Starter({
          wasm_path: "https://cdn.jsdelivr.net/npm/v86@latest/build/v86.wasm",
          memory_size: 64 * 1024 * 1024, // 64MB RAM virtual
          vga_memory_size: 2 * 1024 * 1024,
          screen_container: document.getElementById("screen_container"),
          bios: { url: "https://cdn.jsdelivr.net/npm/v86@latest/bios/seabios.bin" },
          vga_bios: { url: "https://cdn.jsdelivr.net/npm/v86@latest/bios/vgabios.bin" },
          cdrom: { url: "https://copy.sh/v86/images/alpine.iso" }, // ISO oficial de Alpine de prueba
          autostart: true
        });
      } catch (err) {
        console.error("Error al iniciar v86:", err);
      }
    } else if (appId === 'texteditor') {
      document.getElementById('nano-save').onclick = () => {
        const path = document.getElementById('nano-path').value;
        const text = document.getElementById('nano-text').value;
        this.vfs[path] = text;
        this.saveState();
        alert("¡Archivo guardado!");
      };
    }
  }

  setupWindowEvents(appId) {
    const win = document.getElementById(`win-${appId}`);
    const header = win.querySelector('.window-header');

    header.onmousedown = (e) => {
      this.bringToFront(appId);
      let shiftX = e.clientX - win.getBoundingClientRect().left;
      let shiftY = e.clientY - win.getBoundingClientRect().top;

      function onMouseMove(event) {
        win.style.left = `${event.clientX - shiftX}px`;
        win.style.top = `${event.clientY - shiftY}px`;
      }
      function onMouseUp() {
        document.removeEventListener('mousemove', onMouseMove);
        document.removeEventListener('mouseup', onMouseUp);
      }
      document.addEventListener('mousemove', onMouseMove);
      document.addEventListener('mouseup', onMouseUp);
    };

    win.querySelectorAll('.win-btn').forEach(btn => {
      btn.onclick = () => {
        const action = btn.dataset.action;
        if (action === 'close') {
          if (appId === 'terminal' && this.emulatorInstance) {
            this.emulatorInstance.destroy();
            this.emulatorInstance = null;
          }
          this.closeApp(appId);
        }
        if (action === 'min') win.classList.add('minimized');
        if (action === 'max') {
          if (win.style.width === '100vw') {
            win.style.width = '640px'; win.style.height = '440px';
          } else {
            win.style.width = '100vw'; win.style.height = 'calc(100vh - 40px)';
            win.style.top = '0'; win.style.left = '0';
          }
        }
      };
    });
  }

  closeApp(appId) {
    if (this.activeWindows[appId]) {
      this.activeWindows[appId].remove();
      delete this.activeWindows[appId];
      this.updateTaskbar();
    }
  }

  bringToFront(appId) {
    this.zIndexCounter++;
    document.getElementById(`win-${appId}`).style.zIndex = this.zIndexCounter;
  }

  updateTaskbar() {
    const taskbarApps = document.getElementById('taskbar-apps');
    taskbarApps.innerHTML = '';
    Object.keys(this.activeWindows).forEach(appId => {
      const btn = document.createElement('button');
      btn.className = 'taskbar-app active';
      btn.textContent = appId.toUpperCase();
      btn.onclick = () => {
        const win = document.getElementById(`win-${appId}`);
        if (win.classList.contains('minimized')) {
          win.classList.remove('minimized');
          this.bringToFront(appId);
        } else {
          win.classList.add('minimized');
        }
      };
      taskbarApps.appendChild(btn);
    });
  }
}

window.chiquiOS = new ChiquiOSKernel();
