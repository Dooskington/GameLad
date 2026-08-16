# GameLad Emulator Fix Summary

Checkpoint date: **2026-08-16**

Review branch: **`ai-audit-and-fixes`**

This document summarizes the review branch state validated by
`GAMEBOY_TEST_ROM_RESULTS.md`. It is organized by subsystem so later changes can
update one section and rerun the same validation gates.

## Validation checkpoint

- Unit tests: **298/298**
- ROM manifest: **4,528** unique ROMs, **6,680** executions per sweep
- Clean full sweep: **PASS 4,128**, **FAIL 2,288**,
  **UNSUPPORTED_MODEL 169**, **UNVERIFIABLE_NO_ORACLE 95**
- Model pass totals: **DMG 1,828**, **CGB 2,300**
- Two additional 12-worker full sweeps had identical non-timeout membership.
  Their three affected cases passed in the clean six-worker sweep; the two
  repeatedly affected long CPU cases also passed in isolation.
- No crash, load-error, runner-error, timeout, or missing-result executions.

The reporting checkpoints distinguish source behavior from harness coverage:

| Checkpoint | Source change | Harness/oracle change | Objective pass total |
|---|---|---|---:|
| Residual CGB v7 | OAM-DMA, DMA-source, KEY1, CPU/APU/PPU fixes | Existing scorer | 4,059 |
| Oracle completion | None | Added six objectively scoreable outcomes | 4,065 |
| Serial v6 | Divider-edge serial model and CPU hooks | None | 4,084 |
| CGB wave3 | Chunked speed-switch progression and selective HDMA behavior | None | 4,105 |
| Final CGB residual | GDMA setup/debt timing and HDMA termination window | None | 4,122 |
| PCM register correction | Corrected zero-based PCM12/PCM34 channel routing | None | 4,128 |

The final residual source delta is **+17/-0** against PASS4105, all in CGB mode;
the PCM correction is a further **+6/-0**, also in CGB mode. DMG membership is
unchanged. Across the complete integration, the ten reviewed wave3 losses
remain: eight speed-switch PPU/STAT phase cases and two HDMA boundary cases.
The hardware-grounded implementation was retained instead of adding
ROM-specific exceptions.

## Implemented fixes

### CPU, scheduler, interrupts, HALT, and STOP

- Replaced whole-instruction peripheral catch-up with M-cycle-visible hardware
  advancement so memory reads, writes, stack operations, interrupt entry, DMA,
  timer, serial, PPU, and APU events occur in bus order.
- Corrected read, write, and read-modify-write strobes, including leading-strobe
  wave-RAM reads and writes.
- Added explicit internal machine cycles for `PUSH`, `RST`, `CALL`, taken
  conditional `CALL`, and conditional `RET`.
- Implemented delayed `EI`, `DI` cancellation, interrupt-entry timing, IF
  acknowledgement ordering, and interrupt-priority resampling after the upper
  PC stack write can alter IE at `FFFF`.
- Implemented HALT wake behavior, the HALT bug, pending-disabled interrupt
  behavior, and correctly timed interrupt service from HALT.
- Implemented irreversible silicon lock for the eleven undefined LR35902
  opcodes; hardware continues to advance but interrupts and joypad edges cannot
  wake the CPU.
- Implemented two-byte STOP behavior. Ordinary STOP resets DIV/serial phase and
  waits for a joypad edge. CGB KEY1 STOP switches speed, stalls for 8,196 cycles
  after the 4-cycle fetch, freezes the timer/serial counter for 8,188 cycles,
  releases it for eight cycles, then resets DIV.
- Advances the speed-switch stall in 4-cycle chunks so PPU mode transitions,
  APU, cartridge, and DMA remain synchronized across the stall.
- Corrected skip-boot register state, including IF reading as `E1`.

### Timer

- Rebuilt DIV/TIMA around the divider-bit falling-edge circuit.
- Corrected TAC enable/frequency handling and DIV/TAC write glitches.
- Added delayed TIMA overflow reload and interrupt timing, including writes
  during overflow/reload windows.
- Preserved timer counter freeze/release semantics across CGB speed switches and
  correct double-speed divider taps.

### PPU, FIFO, STAT, and DMA

- Enforced LCD-off state, LCD enable restart, VRAM/OAM access restrictions,
  LY/STAT writable bits, and line-153 LY/STAT behavior.
- Replaced fixed Mode 3 rendering with resumable per-pixel FIFO/fetch events,
  including fetch stalls, sprite stalls, window WY latching, WX trigger timing,
  variable Mode 3 length, and right-edge behavior.
- Preserved color-index metadata for DMG priority and carried CGB tile
  attributes, bank, flips, palette, and priority through the FIFO.
- Implemented combined STAT-line edge triggering and internal Mode 2 event
  queueing, including double-speed timing.
- Made OAM DMA a timed bus participant with startup delay, CPU bus blocking,
  source-domain arbitration, next-byte snooping, and CGB open-bus behavior.
  Writes remain blocked during contention.
- Implemented GDMA/HDMA source validation, per-HBlank block transfer, CPU stall
  accounting, cancellation/readback, and speed-switch handling. A speed switch
  terminates only an HBlank transfer that has not completed a block; a transfer
  with a completed first block survives.
- Added GDMA's fixed CPU-domain setup cost (7 cycles at single speed, 4 at
  double speed), paid DMA bus debt in 4-cycle chunks, and kept the final HBlank
  block active through its unpaid-bus termination window.

### MBC, RTC, memory safety, and boot overlay

- Added deterministic cartridge/header validation and rejected ROM-size
  mismatches instead of constructing unsafe controllers.
- Corrected MBC1 lower/upper bank wiring and advanced banking mode.
- Corrected MBC2 address decoding and internal nibble RAM behavior.
- Implemented MBC3/MBC30 RTC registers, latching, halt/carry behavior, elapsed
  time progression, and persistence.
- Corrected MBC5 RAM enable and bank selection.
- Bounded ROM/RAM accesses to backing storage and added missing RAM-size
  support and safe save allocation.
- Corrected boot-ROM overlay disable behavior and prevented cartridge writes
  from being swallowed while the overlay is active.
- Removed scalar deletion of array allocations and initialized hardware state
  deterministically, eliminating the baseline's nondeterministic crashes.

### Serial and joypad

- Replaced the serial test-output stub with divider falling-edge transfers.
- Implemented DMG/CGB normal clock, CGB fast clock, external clocking, SC
  restart/forced-edge behavior, preboot phase, FF04/STOP phase reset, and
  speed-aware interrupt acknowledgement.
- Corrected JOYP upper-bit masks, row selection, combined-row reads, and
  falling-edge interrupt generation.

### APU and audio

- Implemented square, wave, and noise channels; frame sequencer; length,
  envelope, sweep, DAC gating, mixing, panning, volume, and SDL queued stereo
  output.
- Added register masks, NR52 power behavior, powered-off write rules, and
  model-specific length clearing (CGB clears; DMG preserves).
- Added DMG wave-RAM locking/retrigger corruption behavior and CGB active-wave
  alias behavior.
- Added CH3 initial-fetch delay, short-period behavior, two-cycle wave-RAM
  aperture, and retrigger overlap.
- Added CGB PCM amplitude registers while preserving DMG behavior, with PCM12
  reporting CH1/CH2 and PCM34 reporting CH3/CH4 in their documented nibbles.

### CGB model, speed, palettes, and DMA

- Added explicit DMG, CGB, and CGB-compatibility modes resolved from cartridge
  flags, with a force-model API for test runners.
- Implemented KEY1 and double-speed base-domain conversion.
- Implemented CGB WRAM/VRAM banking, palette index/data registers, RGB555
  rendering, tile attributes, OBJ priority mode, and compatibility-mode
  register visibility.
- Added CGB three-domain OAM-DMA arbitration and model-specific WaveChannel,
  APU power-off, timer, serial, and PPU behavior.
- Implemented GDMA/HDMA register masks, invalid external-source open bus,
  remaining-length readback, and selective survival across speed switching.

### Harness and objective oracles

- Added an isolated optimized headless runner with one process per execution.
- Supports serial/A000 Blargg results, Mooneye Fibonacci registers, GBMicro FF82
  verdicts, normalized framebuffer screenshots, Gambatte hexadecimal glyphs,
  input scripts, RTC subtests, and generated-audio checks.
- Added CGB-aware execution, corrected Gambatte `F` glyph scoring, 41-frame
  `mbc3-tester` scoring, bare-name Gambatte screenshots, and the Blargg CGB
  `interrupt_time` oracle.
- Classifies incompatible hardware targets before launch and separates
  bounded utilities or ROMs without machine-readable verdicts from failures.
- The complete objective failure and exclusion lists are in
  `GAMEBOY_TEST_ROM_RESULTS.md`.

## Key regression gates

- CPU: Blargg instruction/timing ROMs, Mooneye call/push/return timing,
  undefined-opcode lock, IE-push interrupt resampling, and postboot IF.
- Timer: 107 objective timer-name executions at the final checkpoint.
- Serial: **83/83** objective executions (**DMG 37**, **CGB 46**).
- MBC/RTC: 61 objective MBC-name passes, 3 objective RTC executions, and exact
  `mbc3-tester` output.
- Audio: 284 objective sound-name passes; DMG and CGB register/power/wave gates
  remain enabled.
- Graphics: exact `dmg-acid2` and `cgb-acid2`; `cgb-acid-hell` remains a known
  objective failure.
- Full corpus: one clean 6,680-execution result set at PASS4128, corroborated by
  two full contention-limited runs and isolated reruns of their host timeouts.

## Rejected regression-prone approaches

- Whole-instruction hardware advancement and post-instruction cycle catch-up:
  totals can look correct while bus events occur in the wrong order.
- Bulk 8,196-cycle KEY1 advancement: the PPU resolves only one transition per
  `Step`, leaving LY/STAT and pixel transfer permanently behind.
- Relaxing blocked CGB OAM-DMA writes: this regressed ROM/SRAM bank-change and
  busy-push cases; blocked writes remain rejected.
- A global per-T-cycle scheduler rewrite: discarded because it regressed the
  accepted FIFO/PPU and APU access phases. Only the proven local CH3 and
  speed-switch timing changes were retained.
- Wholesale replacement of CPU/GPU/APU files from subsystem candidates:
  feature hunks were ported manually to preserve stronger integrated behavior.
- Older APU/PPU snapshots that improved one gate while reducing the accepted
  audio or graphics pass set.
- ROM-specific HDMA or STAT exceptions to recover reviewed wave3 losses; the
  general hardware rule remains authoritative.
- Scaling the line-153 STAT acknowledgement suppression to 12 raw CPU cycles at
  double speed: it added no passes and lost three late-retrigger LYC cases. The
  empirically validated four-cycle double-speed window remains.

## Known remaining hardware-trace limits

- Some sub-machine-cycle PPU/STAT interactions remain unresolved, especially
  Mode 3 speed-switch boundaries, line-specific STAT timing, sprite/window
  fetch arbitration, and exact CGB priority behavior.
- Two HDMA speed-switch boundary cases disagree with the current general rule;
  resolving them requires a finer transfer-start/PPU trace rather than a
  case-name exception.
- APU edge cases still fail in sweep families that depend on exact analog or
  sub-cycle channel behavior.
- Boot ROM execution and the CGB boot-ROM title palette are not modeled by the
  normal skip-boot frontend.
- SGB, AGB-specific, and MGB-only expectations are not implemented. They are
  classified separately when the suite metadata identifies the target.
- Logic-analyzer ROMs, interactive utilities, and visual ROMs without bundled
  reference output remain no-oracle rather than pass or fail.
- The full remaining objective mismatch list is intentionally preserved in
  `GAMEBOY_TEST_ROM_RESULTS.md` for trace-driven follow-up.

## Manual build and play

The following commands use the Visual Studio 18 MSVC environment and an
existing x64 vcpkg checkout at `E:\Git\microsoft\vcpkg`:

```bat
call "C:\Program Files\Microsoft Visual Studio\18\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
set "CMAKE=C:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "VCPKG_ROOT=E:\Git\microsoft\vcpkg"

"%CMAKE%" -S . -B build-debug -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows
"%CMAKE%" --build build-debug

"%CMAKE%" -S . -B build-release -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows
"%CMAKE%" --build build-release
```

Run unit tests:

```bat
build-debug\bin\gb-emu-tests.exe
```

Launch a ROM:

```bat
build-release\bin\gb-emu.exe 3 "E:\ROMs\Tetris.gb"
```

The first argument is an integer window scale and the second is the ROM path.
If omitted, scale defaults to 2 and the frontend loads its built-in test path.
The desktop frontend uses automatic model selection: a CGB-capable cartridge
header selects CGB; other cartridges select DMG. Explicit force-DMG/force-CGB
selection is available to programmatic runners through `ModelPreference`, not
as a desktop CLI argument.
