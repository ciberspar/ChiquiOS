export function initTerminal(kernel) {
  const input = document.getElementById('term-in');
  const out = document.getElementById('term-out');

  input.onkeydown = (e) => {
    if (e.key === 'Enter') {
      const cmd = input.value.trim();
      out.textContent += `\nchiquios:~# ${cmd}\n`;

      const args = cmd.split(' ');
      const mainCmd = args[0];

      if (mainCmd === 'help') {
        out.textContent += `Comandos del sistema chiquiOS:\n  ls                  Listar archivos virtuales\n  cat [path]          Leer un archivo del VFS\n  bacus update        Sincronizar repositorios de Baco\n  bacus install [pkg] Instalar utilidad (ej: htop, matrix, neofetch)\n  bacus list          Listar paquetes instalados con bacus\n  baco                Estado actual de la mascota\n  clear               Limpiar la consola\n  reboot              Reiniciar el SO`;
      } else if (mainCmd === 'ls') {
        out.textContent += Object.keys(kernel.vfs).join('\n');
      } else if (mainCmd === 'cat') {
        const path = args[1];
        if (kernel.vfs[path] !== undefined) {
          out.textContent += kernel.vfs[path];
        } else {
          out.textContent += `cat: ${path || 'archivo no especificado'}: No existe`;
        }
      } else if (cmd === 'bacus update') {
        out.textContent += `[bacus] Conectando a los repositorios oficiales de Baco...\n[bacus] Descargando índices de paquetes estables...\n[bacus] Repositorios actualizados con éxito [OK].`;
      } else if (mainCmd === 'bacus' && args[1] === 'install') {
        const pkg = args[2];
        if (!pkg) {
          out.textContent += `bacus error: Debes especificar un paquete (ej: bacus install matrix)`;
        } else if (kernel.packages.includes(pkg)) {
          out.textContent += `[bacus] El paquete '${pkg}' ya se encuentra instalado.`;
        } else {
          kernel.packages.push(pkg);
          kernel.vfs[`/usr/bin/${pkg}`] = `Binary executable for ${pkg} (Bacus Package Manager)`;
          kernel.saveState();
          out.textContent += `[bacus] Resolviendo dependencias para '${pkg}'...\n[bacus] Compilando y empaquetando en VFS...\n[bacus] ¡Paquete '${pkg}' instalado correctamente!`;
        }
      } else if (mainCmd === 'bacus' && args[1] === 'list') {
        out.textContent += `Paquetes gestionados por bacus:\n` + kernel.packages.join('\n');
      } else if (mainCmd === 'clear') {
        out.textContent = "";
      } else if (cmd === 'baco') {
        out.textContent += "🐢 Baco: Monitoreando el balcón. Temperatura óptima, sistema estable.";
      } else if (cmd === 'reboot') {
        location.reload();
      } else if (cmd !== '') {
        out.textContent += `sh: ${mainCmd}: comando no encontrado. Escribí 'help'.`;
      }

      input.value = '';
      const winContent = out.parentElement;
      winContent.scrollTop = winContent.scrollHeight;
      input.focus();
    }
  };
  input.focus();
}
