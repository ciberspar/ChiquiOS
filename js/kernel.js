import { initTerminal } from './terminal.js';
import { initBaco } from './baco.js';

class ChiquiOSKernel {
  constructor() {
    this.vfs = JSON.parse(localStorage.getItem('chiqui_vfs_tree')) || {
      "/": { type: "dir", content: ["bin", "home", "etc", "root"] },
      "/bin": { type: "dir", content: ["bacus", "ls", "cat", "echo", "touch", "mkdir", "rm", "pwd", "clear"] },
      "/home": { type: "dir", content: ["user"] },
      "/home/user": { type: "dir", content: ["welcome.txt", "juego.py", "app.py"] },
      "/home/user/welcome.txt": { type: "file", content: "Bienvenido a chiquiOS v4.0.\nUn sistema operativo web nativo, libre y modular." },
      "/home/user/juego.py": { type: "file", content: "# vscodebacus - Entorno para chiquiOS\ndef iniciar_juego():\n    print('¡Bienvenido a vscodebacus con Python!')\n    personaje = 'Baco'\n    print(f'Desarrollando apps y juegos con {personaje}')\n\niniciar_juego()" },
      "/home/user/app.py": { type: "file", content: "# App de prueba\nprint('Hola desde app.py en chiquiOS')" },
      "/etc": { type: "dir", content: ["os-release", "hostname"] },
      "/etc/os-release": { type: "file", content: "NAME=\"chiquiOS\"\nID=chiquios\nVERSION=\"4.0-native\"" },
      "/etc/hostname": { type: "file", content: "chiquios-station" },
      "/root": { type: "dir", content: [] }
    };

    this.packages = JSON.parse(localStorage.getItem('chiqui_pkgs')) || ["chiquios-core", "bacus-pm", "nanopad", "browsertortu", "vscodebacus"];
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
    
    if (appId === 'terminal') {
      win.style.width = '540px'; win.style.height = '380px';
    } else if (appId === 'browsertortu') {
      win.style.width = '580px'; win.style.height = '420px';
    } else if (appId === 'vscodebacus') {
      win.style.width = '640px'; win.style.height = '440px';
    } else {
      win.style.width = '420px'; win.style.height = '320px';
    }

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
    } else if (appId === 'vscodebacus') {
      title = "vscodebacus // IDE de Python & Apps de Baco";
      contentHtml = `<div style="display:flex; flex-direction:column; height:100%; background:#1e1e1e; font-family:monospace; color:#d4d4d4;">
        <div style="display:flex; background:#2d2d2d; padding:6px; gap:8px; align-items:center; border-bottom:1px solid #333;">
          <span style="color:#00f3ff; font-weight:bold;">🐢 vscodebacus</span>
          <select id="vsc-file-select" style="background:#1e1e1e; color:#fff; border:1px solid #444; padding:2px 6px;">
            <option value="/home/user/juego.py">juego.py</option>
            <option value="/home/user/app.py">app.py</option>
          </select>
          <button id="vsc-run" style="background:#0e639c; color:#fff; border:none; padding:4px 10px; cursor:pointer; border-radius:3px;">▶ Run Python</button>
        </div>
        <div style="display:flex;
