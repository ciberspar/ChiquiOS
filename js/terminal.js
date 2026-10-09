export function initTerminal(kernel) {
  const input = document.getElementById('term-in');
  const out = document.getElementById('term-out');
  const promptEl = document.getElementById('term-prompt');

  let currentPath = "/";

  function resolvePath(target) {
    if (!target) return currentPath;
    if (target.startsWith("/")) return target;
    if (target === "..") {
      const parts = currentPath.split("/").filter(Boolean);
      parts.pop();
      return "/" + parts.join("/");
    }
    return currentPath === "/" ? "/" + target : currentPath + "/" + target;
  }

  input.onkeydown = (e) => {
    if (e.key === 'Enter') {
      const cmdLine = input.value.trim();
      out.textContent += `\nroot@chiquiOS:${currentPath}# ${cmdLine}\n`;

      if (cmdLine !== '') {
        const args = cmdLine.split(' ');
        const mainCmd = args[0];
        const subCmd = args[1];
        const targetArg = args[2];

        if (mainCmd === 'help') {
          out.textContent += `Comandos del sistema chiquiOS:
  pwd                   Mostrar directorio actual
  ls [dir]              Listar contenido
  cd [dir]              Cambiar de directorio
  cat [file]            Leer archivo
  mkdir [dir]           Crear directorio
  touch [file]          Crear archivo
  rm [file]             Eliminar archivo
  bacus update          Actualizar repositorios de Baco
  bacus install [pkg]   Instalar utilidad nativa
  bacus list            Listar paquetes instalados
  clear                 Limpiar pantalla`;
        } else if (mainCmd === 'pwd') {
          out.textContent += currentPath + '\n';
        } else if (mainCmd === 'ls') {
          const targetDir = resolvePath(subCmd);
          if (kernel.vfs[targetDir] && kernel.vfs[targetDir].type === "dir") {
            out.textContent += kernel.vfs[targetDir].content.join("   ") + '\n';
          } else {
            out.textContent += `ls: ${subCmd || currentPath}: Directorio no encontrado\n`;
          }
        } else if (mainCmd === 'cd') {
          const targetDir = resolvePath(subCmd);
          if (kernel.vfs[targetDir] && kernel.vfs[targetDir].type === "dir") {
            currentPath = targetDir;
            promptEl.textContent = `root@chiquiOS:${currentPath}#`;
          } else {
            out.textContent += `cd: ${subCmd}: Directorio no encontrado\n`;
          }
        } else if (mainCmd === 'cat') {
          const targetFile = resolvePath(subCmd);
          if (kernel.vfs[targetFile] && kernel.vfs[targetFile].type === "file") {
            out.textContent += kernel.vfs[targetFile].content + '\n';
          } else {
            out.textContent += `cat: ${subCmd}: Archivo no encontrado\n`;
          }
        } else if (mainCmd === 'mkdir') {
          if (!subCmd) {
            out.textContent += `mkdir: falta el nombre del directorio\n`;
          } else {
            const newDir = resolvePath(subCmd);
            kernel.vfs[newDir] = { type: "dir", content: [] };
            if (!kernel.vfs[currentPath].content.includes(subCmd)) {
              kernel.vfs[currentPath].content.push(subCmd);
            }
            kernel.saveState();
            out.textContent += `Directorio '${subCmd}' creado con éxito.\n`;
          }
        } else if (mainCmd === 'touch') {
          if (!subCmd) {
            out.textContent += `touch: falta el nombre del archivo\n`;
          } else {
            const newFile = resolvePath(subCmd);
            kernel.vfs[newFile] = { type: "file", content: "" };
            const baseName = subCmd.split("/").pop();
            if (!kernel.vfs[currentPath].content.includes(baseName)) {
              kernel.vfs[currentPath].content.push(baseName);
            }
            kernel.saveState();
            out.textContent += `Archivo '${subCmd}' creado.\n`;
          }
        } else if (mainCmd === 'rm') {
          if (!subCmd) {
            out.textContent += `rm: falta especificar el archivo\n`;
          } else {
            const targetFile = resolvePath(subCmd);
            if (kernel.vfs[targetFile]) {
              delete kernel.vfs[targetFile];
              const baseName = subCmd.split("/").pop();
              const idx = kernel.vfs[currentPath].content.indexOf(baseName);
              if (idx !== -1) kernel.vfs[currentPath].content.splice(idx, 1);
              kernel.saveState();
              out.textContent += `Archivo '${subCmd}' eliminado.\n`;
            } else {
              out.textContent += `rm: ${subCmd}: No existe\n`;
            }
          }
        } 
        // GESTOR NATIVO BACUS
        else if (mainCmd === 'bacus') {
          const nativeRepo = {
            "matrix": "Efecto visual de código digital cayendo",
            "baco-tools": "Herramientas de diagnóstico de la tortuga",
            "net-tools": "Utilidades de red simuladas para chiquiOS",
            "neofetch-chiqui": "Información detallada del sistema chiquiOS"
          };

          if (subCmd === 'update') {
            out.textContent += `[bacus] Conectando con el repositorio oficial de chiquiOS...\n[bacus] Índices sincronizados correctamente [OK].\n`;
          } else if (subCmd === 'install' || subCmd === 'add') {
            const pkg = targetArg;
            if (!pkg) {
              out.textContent += `bacus error: especifica un paquete (ej: bacus install matrix)\n`;
            } else if (!nativeRepo[pkg] && !kernel.packages.includes(pkg)) {
              out.textContent += `bacus: Paquete '${pkg}' no encontrado en los repositorios.\n`;
            } else if (kernel.packages.includes(pkg)) {
              out.textContent += `[bacus] El paquete '${pkg}' ya está instalado.\n`;
            } else {
              kernel.packages.push(pkg);
              kernel.vfs[`/bin/${pkg}`] = { type: "file", content: `Binario nativo para ${pkg} - ${nativeRepo[pkg]}` };
              if (!kernel.vfs["/bin"].content.includes(pkg)) {
                kernel.vfs["/bin"].content.push(pkg);
              }
              kernel.saveState();
              out.textContent += `[bacus] Descargando e instalando '${pkg}'...\n[bacus] ¡Instalación completada con éxito!\n`;
            }
          } else if (subCmd === 'list') {
            out.textContent += `Paquetes instalados en chiquiOS:\n- ` + kernel.packages.join('\n- ') + '\n';
          } else {
            out.textContent += `bacus: comando '${subCmd}' desconocido. Escribí 'help'.\n`;
          }
        } else if (mainCmd === 'clear') {
          out.textContent = "chiquiOS v4.0-native (Terminal)\n";
        } else {
          out.textContent += `sh: ${mainCmd}: comando no encontrado. Escribí 'help'.\n`;
        }
      }

      input.value = '';
      const winContent = out.parentElement;
      winContent.scrollTop = winContent.scrollHeight;
      input.focus();
    }
  };
  input.focus();
}
