# ExplorerBlurMicaFix

针对 [ExplorerBlurMica issue #278](https://github.com/Maplespe/ExplorerBlurMica/issues/278) 的社区修复：
**Windows 11 24H2/25H2 下，文件资源管理器最小化再恢复后背景变成纯透明、亚克力模糊消失。**

由于上游仓库未公开核心实现（`.cpp` 文件被 `.gitignore` 排除、从未提交），无法直接修改原
DLL 重新编译。本修复以**同机制伴生 DLL** 的方式实现：注册为同一个 Explorer 文件夹扩展
（FolderExtension），注入 explorer.exe 后在后台恢复被 DWM 重置的 backdrop。

---

## 根因（Windows 11 build 26200 实测验证）

1. `[V]` ExplorerBlurMica 2.0.1 只在 `CabinetWClass` 窗口**创建时**施加一次效果
   （`effect=1` 对应 `DWMWA_SYSTEMBACKDROP_TYPE = 4 / DWMSBT_TABBEDWINDOW`）。
2. `[V]` 最小化后恢复时，DWM 会重置窗口的 backdrop。
3. `[V]` 在 24H2/25H2 上，恢复后的资源管理器窗口**不再发送**
   `WM_SHOWWINDOW` / `WM_WINDOWPOSCHANGED` / `WM_SIZE` 等消息到 ExplorerBlurMica 的
   subclass（MinHook 探针实测：恢复期间 EBM 的 APPLY/UPDATE 调用次数为 0）。
4. `[V]` 因此 ExplorerBlurMica 基于 `WM_WINDOWPOSCHANGED` 的 UPDATE 重施逻辑永不执行，
   窗口失去背景绘制与模糊层 → 表现为完全透明。

## 修复原理

伴生 DLL 作为 FolderExtension 加载进 explorer.exe 后：

- 启动一个工作线程，每 400ms 枚举**本进程**的 `CabinetWClass` 窗口
  （先按 PID 过滤，避免跨进程 `GetClassNameW` 卡死其它窗口）；
- 对每个非最小化窗口重新设置 `DwmSetWindowAttribute(DWMWA_SYSTEMBACKDROP_TYPE, ...)`，
  取值按 EBM 的 effect 配置映射：
  `effect=0→3, 1→4, 2→2, 3→4, 4→5(尽力而为)`；
- 另注册 `EVENT_SYSTEM_MINIMIZESTART/MINIMIZEEND` WinEvent 钩子作为早期触发；
- 轮询覆盖"转换瞬间未被观察到"的情况（修复早期版本的教训：restore 时消息/事件可能
  全部静默，所以**不能依赖一次性转换检测**，每个轮询周期都幂等重设）。

## 已验证

- `[V]` Windows 11 专业版 26200 + ExplorerBlurMica 2.0.1 + DWMBlurGlass 2.3.1 同时启用。
- `[V]` 修复前：最小化→恢复后窗口完全透明（壁纸清晰透出）。
- `[V]` 修复后：同一操作后保持亚克力模糊（用户目视确认）。
- `[V]` 运行时日志确认转换检测与重设成功：
  `apply 0x... iconic=0 restored=1 hr=0x00000000 effect=1 backdrop=4`。
- `[T]` `effect=1` 之外的配置值未在真机逐一验证；映射表来自 EBM accent 路径的
  反汇编佐证（effect 0–3）与代码推断（effect 4）。

## 构建（mingw-w64）

```powershell
g++ -shared -O2 -DUNICODE -D_UNICODE -static-libgcc `
    FixExplorerBlurMica.c -o FixExplorerBlurMica.dll `
    -lole32 -lshlwapi -ldwmapi -luuid
```

## 注册

- **免管理员（当前用户）**：使用 `BuildAndRegister.ps1`，写入
  `HKCU\SOFTWARE\Classes` 的 CLSID 与 `Drive\shellex\FolderExtensions`。
- **管理员**：`regsvr32 FixExplorerBlurMica.dll`（写入 HKLM）。

注册后重启 explorer.exe 生效。卸载：`regsvr32 /u`（管理员注册时）或删除 HKCU 对应键。

## 文件说明

| 文件 | 作用 |
|------|------|
| `FixExplorerBlurMica.c` | 伴生修复 DLL 全部源码（LGPL-3.0） |
| `BuildAndRegister.ps1` | 一键编译 + 当前用户注册 + 重启 explorer |
| `config.ini`（可选） | 复制 EBM 的配置到 DLL 同目录；缺省按 `effect=1` |

## 许可

LGPL-3.0，与上游 ExplorerBlurMica 保持一致。
