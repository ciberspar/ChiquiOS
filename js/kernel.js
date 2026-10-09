import { initTerminal } from './terminal.js';
import { initBaco } from './baco.js';

class ChiquiOSKernel {
  constructor() {
    this.vfs = JSON.parse(localStorage.getItem('chiqui_vfs_tree')) || {
      "/": { type: "dir", content: ["bin", "home", "etc", "root"] },
      "/bin": { type: "dir", content: ["bacus", "ls", "cat", "echo", "touch", "mkdir", "rm", "pwd", "clear"] },
      "/home": { type: "dir", content: ["user"] },
      "/home/user": { type: "dir", content: ["welcome.txt"] },
      "/home/user/welcome.txt": { type: "file", content: "Bienvenido a chiquiOS v4.0.\nUn sistema operativo web nativo, libre y modular." },
      "/etc": { type: "dir", content: ["os-release", "hostname"] },
      "/etc/os-release": { type: "file", content: "NAME=\"chiquiOS\"\nID=chiquios\nVERSION=\"4.0-native\"" },
      "/etc/hostname": { type: "file", content: "chiquios-station" },
      "/root": { type: "dir", content: [] }
    };

    this.packages = JSON.parse(localStorage.getItem('chiqui_pkgs')) || ["chiquios-core", "bacus-pm", "nanopad", "browsertortu"];
    this.activeWindows = {};
    this.zIndexCounter = 100;
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
    win.style.top = `${50 + (Object.keys(this.activeWindows).length * 25)}px`;
    win.style.left = `${80 + (Object.keys(this.activeWindows).length * 35)}px`;
    win.style.width = appId === 'terminal' ? '540px' : (appId === 'browsertortu' ? '580px' : '420px');
    win.style.height = appId === 'terminal' ? '380px' : (appId === 'browsertortu' ? '420px' : '320px');

    let title = "Ventana";
    let contentHtml = "";

    if (appId === 'terminal') {
      title = "root@chiquiOS: ~ (Terminal Nativa)";
      contentHtml = `<div class="terminal-output" id="term-out">chiquiOS v4.0-native (x86_64-web)\nEscribí 'help' para ver los comandos del sistema.\n</div>
                     <div class="terminal-line"><span id="term-prompt">root@chiquiOS:/#</span><input type="text" class="terminal-input" id="term-in" autofocus></div>`;
    } else if (appId === 'baco') {
      title = "Baco Daemon // Monitor Oficial";
      contentHtml = initBaco();
    } else if (appId === 'texteditor') {
      title = "NanoPad // Editor de Texto VFS";
      contentHtml = `<div style="display:flex; flex-direction:column; height:100%; gap:8px;">
        <input type="text" id="nano-path" value="/home/user/welcome.txt" style="background:#02050b; border:1px solid #00f3ff; color:#fff; padding:4px; font-family:monospace;">
        <textarea id="nano-text" style="flex:1; background:#02050b; color:#00ff66; border:1px solid #00f3ff; padding:8px; font-family:monospace; resize:none;">${this.vfs['/home/user/welcome.txt'] ? this.vfs['/home/user/welcome.txt'].content : ''}</textarea>
        <button id="nano-save" style="background:rgba(0,243,255,0.2); border:1px solid #00f3ff; color:#00f3ff; padding:6px; cursor:pointer; font-family:monospace;">Guardar en VFS</button>
      </div>`;
    } else if (appId === 'browsertortu') {
      title = "Browsertortu // Navegador Oficial de Baco";
      contentHtml = `<div style="display:flex; flex-direction:column; height:100%; background:#02050b;">
        <div style="display:flex; background:#0a1118; padding:6px; gap:6px; border-bottom:1px solid #00f3ff; align-items:center;">
          <span style="font-size:1.1rem;">🐢</span>
          <input type="text" id="tortu-url" value="https://example.com" style="flex:1; background:#02050b; border:1px solid #00f3ff; color:#00ff66; padding:6px; font-family:monospace; border-radius:4px;">
          <button id="tortu-go" style="background:rgba(0,243,255,0.2); border:1px solid #00f3ff; color:#00f3ff; padding:6px 12px; cursor:pointer; font-family:monospace; border-radius:4px;">Navegar</button>
        </div>
        <iframe id="tortu-frame" src="https://example.com" style="flex:1; border:none; background:#fff;"></iframe>
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
      <div class="window-content" style="${appId === 'browsertortu' ? 'padding:0; overflow:hidden;' : ''}">${contentHtml}</div>
    `;

    document.getElementById('desktop').appendChild(win);
    this.activeWindows[appId] = win;
    this.updateTaskbar();
    this.setupWindowEvents(appId);

    if (appId === 'terminal') {
      initTerminal(this);
    } else if (appId === 'texteditor') {
      document.getElementById('nano-save').onclick = () => {
        const path = document.getElementById('nano-path').value;
        const text = document.getElementById('nano-text').value;
        this.vfs[path] = { type: "file", content: text };
        this.saveState();
        alert("¡Archivo guardado en el VFS de chiquiOS!");
      };
    } else if (appId === 'browsertortu') {
      document.getElementById('tortu-go').onclick = () => {
        let targetUrl = document.getElementById('tortu-url').value;
        if (!targetUrl.startsWith('http')) {
          targetUrl = 'https://' + targetUrl;
        }
        document.getElementById('tortu-frame').src = targetUrl;
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
        if (action === 'close') this.closeApp(appId);
        if (action === 'min') win.classList.add('minimized');
        if (action === 'max') {
          if (win.style.width === '100vw') {
            win.style.width = appId === 'terminal' ? '540px' : (appId === 'browsertortu' ? '580px' : '420px');
            win.style.height = appId === 'terminal' ? '380px' : (appId === 'browsertortu' ? '420px' : '320px');
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
