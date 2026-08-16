# Game Boy Test ROM Results

Checkpoint date: **2026-08-16**

Review branch: **`ai-audit-and-fixes`**

Base repository commit: `cbd8d1a5f403def76e65b85ebb98a61aba88adf6`

This report describes the validated review branch state. The same 6,680-case
manifest was run three times after clean Debug and Release builds. The
authoritative six-worker run completed without host timeouts; two 12-worker
confirmation runs had identical non-timeout membership. Their three affected
cases passed in the clean run, and the two repeatedly affected long CPU cases
also passed in isolation.

## Summary

- Discovered ROM files: **4528**
- Executions: **6680** (2152 additional rtc3test subtest executions)
- Full-sweep runs: **3** (**20040** total isolated executions)
- Unit tests: **298/298**
- Objectively passed: **4128**
- Objective failures/timeouts/errors: **2288**
- Executed but excluded as unsupported or unverifiable: **264**
- Missing executions: **0**
- Hardware-model passes: **DMG 1828**, **CGB 2300**
- Crash/load/runner errors, timeouts, and missing results: **0**
- Repeatability: **identical non-host-timeout membership across all runs; all
  affected cases passed in the clean run**
- Authoritative clean-run wall time: **199.8 seconds**
- ROM manifest SHA-256: `09d020b235f53cf3ccaba9303f8334ca03e3940db85176b350a13a43047a8dba`

| Result | Executions |
|---|---:|
| PASS | 4128 |
| FAIL | 2288 |
| UNSUPPORTED_MODEL | 169 |
| UNVERIFIABLE_NO_ORACLE | 95 |

Every `.gb` and `.gbc` file under `res/tests` was launched through an optimized,
temporary headless runner built from the current emulator core. The runner and
raw JSONL results were stored outside the repository. The sweep itself did not
modify emulator source.

### Source behavior versus harness coverage

Pass totals are reporting totals, so source changes and newly objective oracles
are recorded separately:

| Checkpoint | Change type | Pass | DMG | CGB |
|---|---|---:|---:|---:|
| Residual CGB v7 | Source | 4059 | 1817 | 2242 |
| Oracle completion | Harness only (+6, no source change) | 4065 | 1820 | 2245 |
| Serial v6 | Source (+19/-0) | 4084 | 1828 | 2256 |
| CGB wave3 | Source (+31/-10) | 4105 | 1828 | 2277 |
| Final CGB residual | Source (+17/-0) | 4122 | 1828 | 2294 |
| PCM register correction | Source (+6/-0) | 4128 | 1828 | 2300 |

The six oracle-completion gains came from existing emulator behavior that the
earlier harness could not score. Later totals are emulator behavior changes
under the same oracle-complete harness. The final residual adds 17 CGB DMA
passes, and corrected PCM12/PCM34 channel routing adds six CGB APU passes. Both
changes have no former-pass loss and no DMG membership change.

## Methodology

- Boot mode: no boot ROM; the emulator's existing post-boot initialization path was used.
- DMG clock: 4,194,304 T-cycles/s; frame: 70,224 T-cycles.
- Isolation: one process per execution, with a 30-second host timeout, so a ROM crash could not stop the corpus.
- Blargg/root ROMs: serial `Passed`/`Failed`, then the A000 status plus `DE:B0:61` signature, with bundled screenshot fallback where available.
- Current Mooneye, SameSuite, and automatic AGE tests: stop at `LD B,B`; the wilbertpol Mooneye fork stops at illegal opcode `ED`; success registers `B,C,D,E,H,L = 3,5,8,13,21,34`.
- GBMicrotest: binaries containing the suite's compiled `LDH ($82),A` verdict write use `FF82` (`01` pass, `FF` fail); legacy visual/testbench ROMs without that write are unasserted.
- Screenshot suites: compare all 23,040 pixels after normalizing the emulator's DMG palette to four shade indices.
- Gambatte `_out<hex>`: compare the documented top-left 8x8 glyph pattern after 1,053,360 T-cycles; authored bare-name legacy PNGs use the same framebuffer comparison as revision-suffixed PNGs.
- Input-driven tests: all buttons were pulsed for `tellinglys`; rtc3test was run separately for basic, range, and sub-second cases.
- CGB/MGB/SGB/AGB-only expectations are classified `UNSUPPORTED_MODEL` before launch, not as DMG regressions.
- Audio protocols use the emulator's generated sample stream and documented
  register/hex or waveform expectations. Audio ROMs without a stable objective
  oracle are not promoted to passes.
- ROMs without a bundled or documented machine-readable oracle were classified `UNVERIFIABLE_NO_ORACLE`.

### Repeatability

The authoritative six-worker sweep produced exactly **4128 PASS**, **2288
FAIL**, **169 UNSUPPORTED_MODEL**, and **95 UNVERIFIABLE_NO_ORACLE**, with no
host timeout or emulator error. Two 12-worker confirmation sweeps produced the
same status for every execution that completed; two long CPU cases timed out
in both and one timing case timed out once under host contention. The long CPU
cases passed in isolation, the timing case passed in the other confirmation
run, and all three pass in the clean six-worker sweep.

### Protocol inventory

| Protocol | Executions |
|---|---:|
| audio | 220 |
| fib | 651 |
| gambatte_hex | 4674 |
| gbmicro | 482 |
| no_oracle | 104 |
| screenshot | 459 |
| serial_or_screenshot | 90 |

## Per-suite totals

| Suite | Runs | Pass | Fail | Timeout | No result | Crash/load/runner | Unsupported | Unverifiable |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| (root) | 18 | 17 | 1 | 0 | 0 | 0 | 0 | 0 |
| age-test-roms | 91 | 5 | 53 | 0 | 0 | 0 | 33 | 0 |
| blargg | 72 | 51 | 7 | 0 | 0 | 0 | 14 | 0 |
| bully | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 0 |
| cgb-acid-hell | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 0 |
| cgb-acid2 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 |
| dmg-acid2 | 2 | 1 | 0 | 0 | 0 | 0 | 1 | 0 |
| gambatte | 5320 | 3451 | 1817 | 0 | 0 | 0 | 0 | 52 |
| gbmicrotest | 513 | 311 | 171 | 0 | 0 | 0 | 0 | 31 |
| little-things-gb | 2 | 0 | 2 | 0 | 0 | 0 | 0 | 0 |
| mbc3-tester | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 |
| mealybug-tearoom-tests | 63 | 0 | 52 | 0 | 0 | 0 | 9 | 2 |
| mooneye-test-suite | 207 | 158 | 26 | 0 | 0 | 0 | 21 | 2 |
| mooneye-test-suite-wilbertpol | 218 | 106 | 81 | 0 | 0 | 0 | 25 | 6 |
| rtc3test | 3 | 3 | 0 | 0 | 0 | 0 | 0 | 0 |
| same-suite | 156 | 17 | 73 | 0 | 0 | 0 | 66 | 0 |
| scribbltests | 8 | 4 | 2 | 0 | 0 | 0 | 0 | 2 |
| strikethrough | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 0 |
| turtle-tests | 2 | 2 | 0 | 0 | 0 | 0 | 0 | 0 |

## Hardware-model totals

| Model | Runs | Pass | Fail | Timeout | No result | Crash/load/runner | Unsupported | Unverifiable |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| cgb | 3769 | 2300 | 1428 | 0 | 0 | 0 | 1 | 40 |
| dmg | 2911 | 1828 | 860 | 0 | 0 | 0 | 168 | 55 |

### DMG-mode per-suite totals

| Suite | Runs | Pass | Fail | Timeout | No result | Crash/load/runner | Unsupported | Unverifiable |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| (root) | 18 | 17 | 1 | 0 | 0 | 0 | 0 | 0 |
| age-test-roms | 47 | 2 | 12 | 0 | 0 | 0 | 33 | 0 |
| blargg | 58 | 37 | 7 | 0 | 0 | 0 | 14 | 0 |
| bully | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 0 |
| dmg-acid2 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 |
| gambatte | 1907 | 1313 | 578 | 0 | 0 | 0 | 0 | 16 |
| gbmicrotest | 513 | 311 | 171 | 0 | 0 | 0 | 0 | 31 |
| little-things-gb | 2 | 0 | 2 | 0 | 0 | 0 | 0 | 0 |
| mbc3-tester | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 |
| mealybug-tearoom-tests | 35 | 0 | 24 | 0 | 0 | 0 | 9 | 2 |
| mooneye-test-suite | 115 | 80 | 13 | 0 | 0 | 0 | 21 | 1 |
| mooneye-test-suite-wilbertpol | 121 | 53 | 40 | 0 | 0 | 0 | 25 | 3 |
| rtc3test | 3 | 3 | 0 | 0 | 0 | 0 | 0 | 0 |
| same-suite | 78 | 4 | 8 | 0 | 0 | 0 | 66 | 0 |
| scribbltests | 8 | 4 | 2 | 0 | 0 | 0 | 0 | 2 |
| strikethrough | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 0 |
| turtle-tests | 2 | 2 | 0 | 0 | 0 | 0 | 0 | 0 |

### CGB-mode per-suite totals

| Suite | Runs | Pass | Fail | Timeout | No result | Crash/load/runner | Unsupported | Unverifiable |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| age-test-roms | 44 | 3 | 41 | 0 | 0 | 0 | 0 | 0 |
| blargg | 14 | 14 | 0 | 0 | 0 | 0 | 0 | 0 |
| cgb-acid-hell | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 0 |
| cgb-acid2 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 |
| dmg-acid2 | 1 | 0 | 0 | 0 | 0 | 0 | 1 | 0 |
| gambatte | 3413 | 2138 | 1239 | 0 | 0 | 0 | 0 | 36 |
| mealybug-tearoom-tests | 28 | 0 | 28 | 0 | 0 | 0 | 0 | 0 |
| mooneye-test-suite | 92 | 78 | 13 | 0 | 0 | 0 | 0 | 1 |
| mooneye-test-suite-wilbertpol | 97 | 53 | 41 | 0 | 0 | 0 | 0 | 3 |
| same-suite | 78 | 13 | 65 | 0 | 0 | 0 | 0 | 0 |

## Objective failures

Passes are omitted. This table contains every execution with an objective mismatch, missing terminal result, load/crash error, or timeout.

| ROM / case | Result | Evidence |
|---|---|---|
| `oam_bug.gb` | FAIL | Blargg A000 status=01; signature=A001:A003 DE:B0:61 |
| `age-test-roms/halt/halt-m0-interrupt-dmgC-cgbBCE.gb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/halt/halt-m0-interrupt-dmgC-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/lcd-align-ly/lcd-align-ly-cgbBC.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/lcd-align-ly/lcd-align-ly-cgbE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/ly/ly-cgbE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/ly/ly-dmgC-cgbBC.gb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/ly/ly-dmgC-cgbBC.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/ly/ly-ncmBC.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 25, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/ly/ly-ncmE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 25, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/m3-bg-bgp/m3-bg-bgp.gb` | FAIL | pixel diff=262/23040 vs res/tests/age-test-roms/m3-bg-bgp/m3-bg-bgp-dmgC.png |
| `age-test-roms/m3-bg-bgp/m3-bg-bgp.gb::cgb` | FAIL | BCDEHL=(228, 71, 0, 8, 29, 240); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/m3-bg-lcdc/m3-bg-lcdc-ds.gb::cgb` | FAIL | pixel diff=3584/23040 vs res/tests/age-test-roms/m3-bg-lcdc/m3-bg-lcdc-ds-cgbBCE.png |
| `age-test-roms/m3-bg-lcdc/m3-bg-lcdc-nocgb.gb::cgb` | FAIL | pixel diff=23040/23040 vs res/tests/age-test-roms/m3-bg-lcdc/m3-bg-lcdc-nocgb-ncmBCE.png |
| `age-test-roms/m3-bg-lcdc/m3-bg-lcdc.gb` | FAIL | pixel diff=1024/23040 vs res/tests/age-test-roms/m3-bg-lcdc/m3-bg-lcdc-dmgC.png |
| `age-test-roms/m3-bg-lcdc/m3-bg-lcdc.gb::cgb` | FAIL | pixel diff=768/23040 vs res/tests/age-test-roms/m3-bg-lcdc/m3-bg-lcdc-cgbBCE.png |
| `age-test-roms/m3-bg-scx/m3-bg-scx-ds.gb::cgb` | FAIL | pixel diff=140/23040 vs res/tests/age-test-roms/m3-bg-scx/m3-bg-scx-ds-cgbBCE.png |
| `age-test-roms/m3-bg-scx/m3-bg-scx-nocgb.gb::cgb` | FAIL | pixel diff=23040/23040 vs res/tests/age-test-roms/m3-bg-scx/m3-bg-scx-nocgb-ncmBCE.png |
| `age-test-roms/m3-bg-scx/m3-bg-scx.gb` | FAIL | pixel diff=112/23040 vs res/tests/age-test-roms/m3-bg-scx/m3-bg-scx-dmgC.png |
| `age-test-roms/m3-bg-scx/m3-bg-scx.gb::cgb` | FAIL | pixel diff=140/23040 vs res/tests/age-test-roms/m3-bg-scx/m3-bg-scx-cgbBCE.png |
| `age-test-roms/oam/oam-read-cgbE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/oam/oam-read-dmgC-cgbBC.gb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/oam/oam-read-dmgC-cgbBC.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/oam/oam-read-ncmBC.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 25, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/oam/oam-read-ncmE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 25, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/oam/oam-write-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/oam/oam-write-dmgC.gb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/oam/oam-write-ncmBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 25, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/speed-switch/caution/spsw-interrupts-cgbBC.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/speed-switch/caution/spsw-interrupts-cgbE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/speed-switch/spsw-ch2-lc-delay-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/speed-switch/spsw-mode0-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/speed-switch/spsw-stop-prefetch-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/speed-switch/spsw-tima-cgbBC.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/speed-switch/spsw-tima-cgbE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-interrupt/stat-int-dmgC-cgbBCE.gb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-interrupt/stat-int-dmgC-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-interrupt/stat-int-ncmBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 25, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode-sprites/stat-mode-sprites-dmgC-cgbBCE.gb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode-sprites/stat-mode-sprites-dmgC-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode-sprites/stat-mode-sprites-ds-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode-window/stat-mode-window-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode-window/stat-mode-window-dmgC.gb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode-window/stat-mode-window-ds-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode-window/stat-mode-window-ncmBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode/stat-mode-cgbE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode/stat-mode-dmgC-cgbBC.gb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode/stat-mode-dmgC-cgbBC.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode/stat-mode-ds-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode/stat-mode-ncmBC.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 25, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/stat-mode/stat-mode-ncmE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 25, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/vram/vram-read-cgbBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/vram/vram-read-dmgC.gb` | FAIL | BCDEHL=(0, 107, 20, 6, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `age-test-roms/vram/vram-read-ncmBCE.gb::cgb` | FAIL | BCDEHL=(0, 107, 20, 25, 152, 16); expected=(3, 5, 8, 13, 21, 34) |
| `blargg/oam_bug/oam_bug.gb` | FAIL | Blargg A000 status=01; signature=A001:A003 DE:B0:61 |
| `blargg/oam_bug/rom_singles/1-lcd_sync.gb` | FAIL | Blargg A000 status=03; signature=A001:A003 DE:B0:61 |
| `blargg/oam_bug/rom_singles/2-causes.gb` | FAIL | Blargg A000 status=02; signature=A001:A003 DE:B0:61 |
| `blargg/oam_bug/rom_singles/4-scanline_timing.gb` | FAIL | Blargg A000 status=03; signature=A001:A003 DE:B0:61 |
| `blargg/oam_bug/rom_singles/5-timing_bug.gb` | FAIL | Blargg A000 status=02; signature=A001:A003 DE:B0:61 |
| `blargg/oam_bug/rom_singles/7-timing_effect.gb` | FAIL | Blargg A000 status=01; signature=A001:A003 DE:B0:61 |
| `blargg/oam_bug/rom_singles/8-instr_effect.gb` | FAIL | Blargg A000 status=02; signature=A001:A003 DE:B0:61 |
| `bully/bully.gb` | FAIL | pixel diff=290/23040 vs res/tests/bully/bully.png |
| `cgb-acid-hell/cgb-acid-hell.gbc::cgb` | FAIL | pixel diff=2/23040 vs res/tests/cgb-acid-hell/cgb-acid-hell.png |
| `gambatte/bgen/bgoff_bgon_sprite_below_window.gbc::cgb` | FAIL | pixel diff=24/23040 vs res/tests/gambatte/bgen/bgoff_bgon_sprite_below_window_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx08_1.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx08_1_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx08_1.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx08_1_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx08_2.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx08_2_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx08_2.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx08_2_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx08_3.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx08_3_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx08_3.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx08_3_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx08_4.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx08_4_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx08_4.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx08_4_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx08_ds_3.gbc::cgb` | FAIL | pixel diff=1280/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx08_ds_3_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx08_ds_4.gbc::cgb` | FAIL | pixel diff=2304/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx08_ds_4_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx09_1.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx09_1_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx09_1.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx09_1_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx09_2.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx09_2_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx09_3.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx09_3_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx09_3.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx09_3_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx09_4.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx09_4_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx09_ds_1.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx09_ds_1_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx09_ds_2.gbc::cgb` | FAIL | pixel diff=256/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx09_ds_2_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx09_ds_3.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx09_ds_3_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx09_ds_4.gbc::cgb` | FAIL | pixel diff=2304/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx09_ds_4_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx0A_1.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0A_1_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx0A_1.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0A_1_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx0A_2.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0A_2_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx0A_2.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0A_2_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx0A_3.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0A_3_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx0A_3.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0A_3_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx0A_4.gbc` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0A_4_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx0A_4.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0A_4_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx0B_1.gbc` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0B_1_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx0B_2.gbc` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0B_2_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx0B_2.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0B_2_cgb04c.png |
| `gambatte/bgtiledata/bgtiledata_spx0B_3.gbc` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0B_3_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx0B_4.gbc` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0B_4_dmg08.png |
| `gambatte/bgtiledata/bgtiledata_spx0B_4.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/bgtiledata/bgtiledata_spx0B_4_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx08_1.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_1_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx08_1.gbc::cgb` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_1_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx08_2.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_2_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx08_2.gbc::cgb` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_2_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx08_3.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_3_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx08_3.gbc::cgb` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_3_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx08_4.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_4_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx08_4.gbc::cgb` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_4_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx08_ds_1.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_ds_1_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx08_ds_2.gbc::cgb` | FAIL | pixel diff=128/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_ds_2_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx08_ds_3.gbc::cgb` | FAIL | pixel diff=128/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_ds_3_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx08_ds_4.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx08_ds_4_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx09_1.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_1_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx09_1.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_1_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx09_2.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_2_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx09_2.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_2_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx09_3.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_3_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx09_3.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_3_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx09_4.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_4_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx09_4.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_4_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx09_ds_1.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_ds_1_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx09_ds_2.gbc::cgb` | FAIL | pixel diff=128/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_ds_2_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx09_ds_3.gbc::cgb` | FAIL | pixel diff=128/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_ds_3_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx09_ds_4.gbc::cgb` | FAIL | pixel diff=1272/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx09_ds_4_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx0A_1.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0A_1_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx0A_1.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0A_1_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx0A_2.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0A_2_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx0A_2.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0A_2_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx0A_3.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0A_3_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx0A_3.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0A_3_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx0A_4.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0A_4_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx0A_4.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0A_4_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx0B_1.gbc` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0B_1_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx0B_1.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0B_1_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx0B_2.gbc` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0B_2_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx0B_2.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0B_2_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx0B_3.gbc` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0B_3_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx0B_3.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0B_3_cgb04c.png |
| `gambatte/bgtilemap/bgtilemap_spx0B_4.gbc` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0B_4_dmg08.png |
| `gambatte/bgtilemap/bgtilemap_spx0B_4.gbc::cgb` | FAIL | pixel diff=120/23040 vs res/tests/gambatte/bgtilemap/bgtilemap_spx0B_4_cgb04c.png |
| `gambatte/cgbpal_m3/cgbpal_m3end_1_cgb04c_out7.gbc::cgb` | FAIL | expected top-left glyphs 7; glyph pixel diff=16 |
| `gambatte/cgbpal_m3/cgbpal_m3end_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_ds_1_cgb04c_out7.gbc::cgb` | FAIL | expected top-left glyphs 7; glyph pixel diff=16 |
| `gambatte/cgbpal_m3/cgbpal_m3end_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_ds_3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx2_1_cgb04c_out7.gbc::cgb` | FAIL | expected top-left glyphs 7; glyph pixel diff=16 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx2_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx2_3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx3_1_cgb04c_out7.gbc::cgb` | FAIL | expected top-left glyphs 7; glyph pixel diff=16 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx3_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx3_3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx5_1_cgb04c_out7.gbc::cgb` | FAIL | expected top-left glyphs 7; glyph pixel diff=16 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx5_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx5_3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx5_ds_1_cgb04c_out7.gbc::cgb` | FAIL | expected top-left glyphs 7; glyph pixel diff=16 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx5_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3end_scx5_ds_3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3start_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_m3start_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_read_m3start_2_cgb04c_outFF.gbc::cgb` | FAIL | expected top-left glyphs FF; glyph pixel diff=30 |
| `gambatte/cgbpal_m3/cgbpal_read_m3start_ds_2_cgb04c_outFF.gbc::cgb` | FAIL | expected top-left glyphs FF; glyph pixel diff=30 |
| `gambatte/cgbpal_m3/cgbpal_read_m3start_ds_lcdoffset1_2_cgb04c_outFF.gbc::cgb` | FAIL | expected top-left glyphs FF; glyph pixel diff=30 |
| `gambatte/cgbpal_m3/cgbpal_read_m3start_lcdoffset1_2_cgb04c_outFF.gbc::cgb` | FAIL | expected top-left glyphs FF; glyph pixel diff=30 |
| `gambatte/cgbpal_m3/cgbpal_write_m3start_2_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_write_m3start_ds_2_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_write_m3start_ds_lcdoffset1_2_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=27 |
| `gambatte/cgbpal_m3/cgbpal_write_m3start_lcdoffset1_2_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=27 |
| `gambatte/display_startstate/ly_dmg08_out00_cgb04c_out90.gbc` | FAIL | expected top-left glyphs 00; glyph pixel diff=34 |
| `gambatte/display_startstate/ly_dmg08_out00_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=27 |
| `gambatte/display_startstate/stat_1_cgb04c_out87.gbc::cgb` | FAIL | expected top-left glyphs 87; glyph pixel diff=17 |
| `gambatte/display_startstate/stat_1_dmg08_out85.gb` | FAIL | expected top-left glyphs 85; glyph pixel diff=24 |
| `gambatte/display_startstate/stat_2_cgb04c_out84.gbc::cgb` | FAIL | expected top-left glyphs 84; glyph pixel diff=14 |
| `gambatte/display_startstate/stat_2_dmg08_out84.gb` | FAIL | expected top-left glyphs 84; glyph pixel diff=21 |
| `gambatte/display_startstate/stat_scx2_1_cgb04c_out87.gbc::cgb` | FAIL | expected top-left glyphs 87; glyph pixel diff=17 |
| `gambatte/display_startstate/stat_scx2_2_cgb04c_out84.gbc::cgb` | FAIL | expected top-left glyphs 84; glyph pixel diff=14 |
| `gambatte/display_startstate/stat_scx3_1_cgb04c_out87.gbc::cgb` | FAIL | expected top-left glyphs 87; glyph pixel diff=17 |
| `gambatte/display_startstate/stat_scx3_2_cgb04c_out84.gbc::cgb` | FAIL | expected top-left glyphs 84; glyph pixel diff=14 |
| `gambatte/display_startstate/stat_scx5_1_cgb04c_out87.gbc::cgb` | FAIL | expected top-left glyphs 87; glyph pixel diff=17 |
| `gambatte/display_startstate/stat_scx5_2_cgb04c_out84.gbc::cgb` | FAIL | expected top-left glyphs 84; glyph pixel diff=14 |
| `gambatte/div/start_inc_1_cgb04c_out1E.gbc::cgb` | FAIL | expected top-left glyphs 1E; glyph pixel diff=28 |
| `gambatte/div/start_inc_2_cgb04c_out1F.gbc::cgb` | FAIL | expected top-left glyphs 1F; glyph pixel diff=34 |
| `gambatte/dma/dma_dst_wrap_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_cycles_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/dma/hdma_cycles_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/dma/hdma_cycles_scx2_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/dma/hdma_cycles_scx3_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/dma/hdma_cycles_scx5_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/dma/hdma_cycles_scx5_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/dma/hdma_disable_display_1_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/dma/hdma_disabled_display_1_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/dma/hdma_ei_m3halt_m0unhalt_ly_2_cgb04c_out03.gbc::cgb` | FAIL | expected top-left glyphs 03; glyph pixel diff=5 |
| `gambatte/dma/hdma_late_destl_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_late_disable_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_late_disable_scx5_2_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/dma/hdma_late_ei_m3halt_m2unhalt_ly_scx1_2_cgb04c_out03.gbc::cgb` | FAIL | expected top-left glyphs 03; glyph pixel diff=5 |
| `gambatte/dma/hdma_late_ei_m3halt_m2unhalt_ly_scx1_4_cgb04c_out03.gbc::cgb` | FAIL | expected top-left glyphs 03; glyph pixel diff=5 |
| `gambatte/dma/hdma_late_ei_m3halt_m2unhalt_ly_scx1_5_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=5 |
| `gambatte/dma/hdma_late_enable_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_late_enable_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_late_enable_ds_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_late_enable_lcdoffset3_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_late_if_and_ie_halt_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=9 |
| `gambatte/dma/hdma_late_length_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_late_m0halt_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m0halt_ds_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m0halt_ds_lcdoffset1_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m0halt_lcdoffset3_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m0unhalt_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m0unhalt_ds_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m3halt_m2unhalt_inc_scx1_2_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=26 |
| `gambatte/dma/hdma_late_m3halt_m2unhalt_inc_scx2_2_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=26 |
| `gambatte/dma/hdma_late_m3halt_m2unhalt_ly_scx1_2_cgb04c_out03.gbc::cgb` | FAIL | expected top-left glyphs 03; glyph pixel diff=5 |
| `gambatte/dma/hdma_late_m3halt_m2unhalt_ly_scx1_4_cgb04c_out03.gbc::cgb` | FAIL | expected top-left glyphs 03; glyph pixel diff=5 |
| `gambatte/dma/hdma_late_m3halt_m2unhalt_ly_scx1_5_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=5 |
| `gambatte/dma/hdma_late_m3halt_m2unhalt_ly_scx2_2_cgb04c_out03.gbc::cgb` | FAIL | expected top-left glyphs 03; glyph pixel diff=5 |
| `gambatte/dma/hdma_late_m3halt_m2unhalt_ly_scx2_4_cgb04c_out03.gbc::cgb` | FAIL | expected top-left glyphs 03; glyph pixel diff=5 |
| `gambatte/dma/hdma_late_m3halt_m2unhalt_ly_scx2_6_cgb04c_out03.gbc::cgb` | FAIL | expected top-left glyphs 03; glyph pixel diff=5 |
| `gambatte/dma/hdma_late_m3halt_m2unhalt_scx1_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m3halt_m2unhalt_scx2_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m3speedchange_hdma5_scx1_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=11 |
| `gambatte/dma/hdma_late_m3speedchange_hdma5_scx1_ds_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=11 |
| `gambatte/dma/hdma_late_m3speedchange_hdma5_scx2_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=11 |
| `gambatte/dma/hdma_late_m3speedchange_hdma5_scx2_2_cgb04c_out80.gbc::cgb` | FAIL | expected top-left glyphs 80; glyph pixel diff=29 |
| `gambatte/dma/hdma_late_m3speedchange_hdma5_scx2_ds_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m3speedchange_inc_scx1_2_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=26 |
| `gambatte/dma/hdma_late_m3speedchange_ly_scx1_1_cgb04c_out92.gbc::cgb` | FAIL | expected top-left glyphs 92; glyph pixel diff=21 |
| `gambatte/dma/hdma_late_m3speedchange_ly_scx1_2_cgb04c_out93.gbc::cgb` | FAIL | expected top-left glyphs 93; glyph pixel diff=24 |
| `gambatte/dma/hdma_late_m3speedchange_ly_scx1_3_cgb04c_out92.gbc::cgb` | FAIL | expected top-left glyphs 92; glyph pixel diff=21 |
| `gambatte/dma/hdma_late_m3speedchange_ly_scx1_4_cgb04c_out93.gbc::cgb` | FAIL | expected top-left glyphs 93; glyph pixel diff=24 |
| `gambatte/dma/hdma_late_m3speedchange_ly_scx1_5_cgb04c_out92.gbc::cgb` | FAIL | expected top-left glyphs 92; glyph pixel diff=21 |
| `gambatte/dma/hdma_late_m3speedchange_ly_scx1_6_cgb04c_out93.gbc::cgb` | FAIL | expected top-left glyphs 93; glyph pixel diff=24 |
| `gambatte/dma/hdma_late_m3speedchange_read_hdmadst00_scx1_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m3speedchange_read_hdmadst00_scx1_2_cgb04c_out9F.gbc::cgb` | FAIL | expected top-left glyphs 9F; glyph pixel diff=12 |
| `gambatte/dma/hdma_late_m3speedchange_read_hdmadst00_scx2_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_late_m3speedchange_read_hdmadst00_scx2_ds_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=22 |
| `gambatte/dma/hdma_late_m3speedchange_tima_scx1_ds_3_cgb04c_outF6.gbc::cgb` | FAIL | expected top-left glyphs F6; glyph pixel diff=4 |
| `gambatte/dma/hdma_late_m3speedchange_tima_scx1_ds_4_cgb04c_outF7.gbc::cgb` | FAIL | expected top-left glyphs F7; glyph pixel diff=20 |
| `gambatte/dma/hdma_late_speedchange_inc_scx1_ds_2_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=26 |
| `gambatte/dma/hdma_late_wrambank_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_m0halt_late_m3unhalt_scx1_2_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_m0speedchange_late_m3wakeup_scx1_2_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_m0speedchange_late_m3wakeup_scx2_2_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_m3halt_m0unhalt_ly_2_cgb04c_out03.gbc::cgb` | FAIL | expected top-left glyphs 03; glyph pixel diff=5 |
| `gambatte/dma/hdma_m3halt_m1unhalt_hdma5_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_m3speedchange_late_m0wakeup_1_cgb04c_outFF.gbc::cgb` | FAIL | expected top-left glyphs FF; glyph pixel diff=29 |
| `gambatte/dma/hdma_m3speedchange_late_m0wakeup_2_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=11 |
| `gambatte/dma/hdma_pc_7ffe_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=20 |
| `gambatte/dma/hdma_start_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_start_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_start_ly0_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_start_scx2_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_start_scx3_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_start_scx5_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=19 |
| `gambatte/dma/hdma_start_scx5_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/dma/hdma_transition_7fffhalt_inc_m3unhalt_cgb04c_out01.gbc::cgb` | FAIL | expected top-left glyphs 01; glyph pixel diff=27 |
| `gambatte/dma/hdma_transition_ei_halt_late_unhalt_ldaaimm_hdma_scx1_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=29 |
| `gambatte/dma/hdma_transition_ei_halt_late_unhalt_ldaaimm_hdma_scx1_2_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=21 |
| `gambatte/dma/hdma_transition_ei_halt_late_unhalt_scx1_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_transition_halt_late_unhalt_ldaaimm_hdma_scx1_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=27 |
| `gambatte/dma/hdma_transition_halt_late_unhalt_scx1_1_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=30 |
| `gambatte/dma/hdma_transition_halt_m0unhalt_ldaaimm_scx1_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=26 |
| `gambatte/dma/hdma_transition_oamdma_1_cgb04c_out509E529C.gbc::cgb` | FAIL | expected top-left glyphs 509E529C; glyph pixel diff=51 |
| `gambatte/dma/hdma_transition_oamdma_2_cgb04c_out67.gbc::cgb` | FAIL | expected top-left glyphs 67; glyph pixel diff=22 |
| `gambatte/dma/hdma_transition_speedchange_7fffstop_inc_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=26 |
| `gambatte/dma/hdma_transition_speedchange_ldaaimm_scx1_cgb04c_outFF.gbc::cgb` | FAIL | expected top-left glyphs FF; glyph pixel diff=37 |
| `gambatte/dma/hdma_transition_speedchange_ldaaimm_scx1_ds_cgb04c_out03.gbc::cgb` | FAIL | expected top-left glyphs 03; glyph pixel diff=35 |
| `gambatte/dma/hdma_transition_speedchange_oamdma_cgb04c_out71.gbc::cgb` | FAIL | expected top-left glyphs 71; glyph pixel diff=54 |
| `gambatte/dma/late_gdma_pc_7ffe_1_cgb04c_out02.gbc::cgb` | FAIL | expected top-left glyphs 02; glyph pixel diff=26 |
| `gambatte/dmgpalette_during_m3/dmgpalette_during_m3_2.gb` | FAIL | pixel diff=429/23040 vs res/tests/gambatte/dmgpalette_during_m3/dmgpalette_during_m3_2_dmg08.png |
| `gambatte/dmgpalette_during_m3/dmgpalette_during_m3_3.gb` | FAIL | pixel diff=431/23040 vs res/tests/gambatte/dmgpalette_during_m3/dmgpalette_during_m3_3_dmg08.png |
| `gambatte/dmgpalette_during_m3/dmgpalette_during_m3_4.gb` | FAIL | pixel diff=860/23040 vs res/tests/gambatte/dmgpalette_during_m3/dmgpalette_during_m3_4_dmg08.png |
| `gambatte/dmgpalette_during_m3/dmgpalette_during_m3_5.gb` | FAIL | pixel diff=860/23040 vs res/tests/gambatte/dmgpalette_during_m3/dmgpalette_during_m3_5_dmg08.png |
| `gambatte/dmgpalette_during_m3/dmgpalette_during_m3_scx1_4.gb` | FAIL | pixel diff=860/23040 vs res/tests/gambatte/dmgpalette_during_m3/dmgpalette_during_m3_scx1_4_dmg08.png |
| `gambatte/dmgpalette_during_m3/dmgpalette_during_m3_scx2_1.gb` | FAIL | pixel diff=143/23040 vs res/tests/gambatte/dmgpalette_during_m3/dmgpalette_during_m3_scx2_1_dmg08.png |
| `gambatte/dmgpalette_during_m3/lycint_dmgpalette_during_m3_1.gb` | FAIL | pixel diff=143/23040 vs res/tests/gambatte/dmgpalette_during_m3/lycint_dmgpalette_during_m3_1_dmg08.png |
| `gambatte/dmgpalette_during_m3/lycint_dmgpalette_during_m3_2.gb` | FAIL | pixel diff=143/23040 vs res/tests/gambatte/dmgpalette_during_m3/lycint_dmgpalette_during_m3_2_dmg08.png |
| `gambatte/dmgpalette_during_m3/lycint_dmgpalette_during_m3_3.gb` | FAIL | pixel diff=286/23040 vs res/tests/gambatte/dmgpalette_during_m3/lycint_dmgpalette_during_m3_3_dmg08.png |
| `gambatte/dmgpalette_during_m3/lycint_dmgpalette_during_m3_4.gb` | FAIL | pixel diff=286/23040 vs res/tests/gambatte/dmgpalette_during_m3/lycint_dmgpalette_during_m3_4_dmg08.png |
| `gambatte/dmgpalette_during_m3/scx3/dmgpalette_during_m3_1.gb` | FAIL | pixel diff=286/23040 vs res/tests/gambatte/dmgpalette_during_m3/scx3/dmgpalette_during_m3_1_dmg08.png |
| `gambatte/dmgpalette_during_m3/scx3/dmgpalette_during_m3_2.gb` | FAIL | pixel diff=430/23040 vs res/tests/gambatte/dmgpalette_during_m3/scx3/dmgpalette_during_m3_2_dmg08.png |
| `gambatte/dmgpalette_during_m3/scx3/dmgpalette_during_m3_3.gb` | FAIL | pixel diff=430/23040 vs res/tests/gambatte/dmgpalette_during_m3/scx3/dmgpalette_during_m3_3_dmg08.png |
| `gambatte/dmgpalette_during_m3/scx3/dmgpalette_during_m3_4.gb` | FAIL | pixel diff=574/23040 vs res/tests/gambatte/dmgpalette_during_m3/scx3/dmgpalette_during_m3_4_dmg08.png |
| `gambatte/dmgpalette_during_m3/scx3/dmgpalette_during_m3_5.gb` | FAIL | pixel diff=860/23040 vs res/tests/gambatte/dmgpalette_during_m3/scx3/dmgpalette_during_m3_5_dmg08.png |
| `gambatte/enable_display/disable_display_regs_1_dmg08_cgb04c_out66e46666009266666666.gbc` | FAIL | expected top-left glyphs 66E46666009266666666; glyph pixel diff=18 |
| `gambatte/enable_display/disable_display_regs_1_dmg08_cgb04c_out66e46666009266666666.gbc::cgb` | FAIL | expected top-left glyphs 66E46666009266666666; glyph pixel diff=18 |
| `gambatte/enable_display/disable_display_regs_3_dmg08_cgb04c_out91e06666006666666666.gbc` | FAIL | expected top-left glyphs 91E06666006666666666; glyph pixel diff=9 |
| `gambatte/enable_display/disable_display_regs_3_dmg08_cgb04c_out91e06666006666666666.gbc::cgb` | FAIL | expected top-left glyphs 91E06666006666666666; glyph pixel diff=9 |
| `gambatte/enable_display/enable_display_ly0_m2irq_dmg08_out0_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/enable_display/enable_display_ly0_m2irq_dmg08_out0_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/enable_display/enable_display_ly0_oambusy_read_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=19 |
| `gambatte/enable_display/enable_display_ly0_oambusy_read_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=19 |
| `gambatte/enable_display/enable_display_ly0_oambusy_read_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=19 |
| `gambatte/enable_display/enable_display_ly0_sprites_m0stat_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/enable_display/enable_display_ly0_sprites_m0stat_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/enable_display/frame0_ly_count_2_dmg08_cgb04c_out9A.gbc` | FAIL | expected top-left glyphs 9A; glyph pixel diff=28 |
| `gambatte/enable_display/frame0_ly_count_2_dmg08_cgb04c_out9A.gbc::cgb` | FAIL | expected top-left glyphs 9A; glyph pixel diff=28 |
| `gambatte/enable_display/frame0_ly_count_ds_2_cgb04c_out9A.gbc::cgb` | FAIL | expected top-left glyphs 9A; glyph pixel diff=28 |
| `gambatte/enable_display/frame0_m0irq_count_scx2_2_dmg08_cgb04c_out90.gbc` | FAIL | expected top-left glyphs 90; glyph pixel diff=34 |
| `gambatte/enable_display/frame0_m0irq_count_scx2_2_dmg08_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=34 |
| `gambatte/enable_display/frame0_m0irq_count_scx2_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=34 |
| `gambatte/enable_display/frame0_m0irq_count_scx3_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=34 |
| `gambatte/enable_display/frame0_m1stat_ds_2_cgb04c_out81.gbc::cgb` | FAIL | expected top-left glyphs 81; glyph pixel diff=27 |
| `gambatte/enable_display/frame0_m2irq_count_1_dmg08_cgb04c_out98.gbc` | FAIL | expected top-left glyphs 98; glyph pixel diff=31 |
| `gambatte/enable_display/frame0_m2irq_count_1_dmg08_cgb04c_out98.gbc::cgb` | FAIL | expected top-left glyphs 98; glyph pixel diff=31 |
| `gambatte/enable_display/frame0_m2irq_count_2_dmg08_cgb04c_out91.gbc` | FAIL | expected top-left glyphs 91; glyph pixel diff=33 |
| `gambatte/enable_display/frame0_m2irq_count_2_dmg08_cgb04c_out91.gbc::cgb` | FAIL | expected top-left glyphs 91; glyph pixel diff=33 |
| `gambatte/enable_display/frame0_m2irq_count_ds_1_cgb04c_out98.gbc::cgb` | FAIL | expected top-left glyphs 98; glyph pixel diff=31 |
| `gambatte/enable_display/frame0_m2irq_count_ds_2_cgb04c_out91.gbc::cgb` | FAIL | expected top-left glyphs 91; glyph pixel diff=33 |
| `gambatte/enable_display/frame0_m2stat_count_1_dmg08_cgb04c_out91.gbc` | FAIL | expected top-left glyphs 91; glyph pixel diff=7 |
| `gambatte/enable_display/frame0_m2stat_count_1_dmg08_cgb04c_out91.gbc::cgb` | FAIL | expected top-left glyphs 91; glyph pixel diff=7 |
| `gambatte/enable_display/frame0_m2stat_count_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=34 |
| `gambatte/enable_display/frame0_m3stat_count_1_dmg08_cgb04c_out90.gbc` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/frame0_m3stat_count_1_dmg08_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/frame0_m3stat_count_ds_1_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/frame0_m3stat_count_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=34 |
| `gambatte/enable_display/frame1_ly_count_1_dmg08_cgb04c_out99.gbc` | FAIL | expected top-left glyphs 99; glyph pixel diff=8 |
| `gambatte/enable_display/frame1_ly_count_1_dmg08_cgb04c_out99.gbc::cgb` | FAIL | expected top-left glyphs 99; glyph pixel diff=8 |
| `gambatte/enable_display/frame1_ly_count_2_dmg08_cgb04c_out9A.gbc` | FAIL | expected top-left glyphs 9A; glyph pixel diff=27 |
| `gambatte/enable_display/frame1_ly_count_2_dmg08_cgb04c_out9A.gbc::cgb` | FAIL | expected top-left glyphs 9A; glyph pixel diff=27 |
| `gambatte/enable_display/frame1_ly_count_ds_2_cgb04c_out9A.gbc::cgb` | FAIL | expected top-left glyphs 9A; glyph pixel diff=27 |
| `gambatte/enable_display/frame1_m0irq_count_scx2_2_dmg08_cgb04c_out90.gbc` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/frame1_m0irq_count_scx2_2_dmg08_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/frame1_m0irq_count_scx2_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/frame1_m0irq_count_scx3_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/frame1_m1stat_ds_2_cgb04c_out81.gbc::cgb` | FAIL | expected top-left glyphs 81; glyph pixel diff=27 |
| `gambatte/enable_display/frame1_m2irq_count_2_dmg08_cgb04c_out91.gbc` | FAIL | expected top-left glyphs 91; glyph pixel diff=7 |
| `gambatte/enable_display/frame1_m2irq_count_2_dmg08_cgb04c_out91.gbc::cgb` | FAIL | expected top-left glyphs 91; glyph pixel diff=7 |
| `gambatte/enable_display/frame1_m2irq_count_ds_2_cgb04c_out91.gbc::cgb` | FAIL | expected top-left glyphs 91; glyph pixel diff=34 |
| `gambatte/enable_display/frame1_m2stat_count_1_dmg08_cgb04c_out91.gbc` | FAIL | expected top-left glyphs 91; glyph pixel diff=34 |
| `gambatte/enable_display/frame1_m2stat_count_1_dmg08_cgb04c_out91.gbc::cgb` | FAIL | expected top-left glyphs 91; glyph pixel diff=34 |
| `gambatte/enable_display/frame1_m2stat_count_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/frame1_m3stat_count_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/frame2_m0irq_count_scx2_2_dmg08_cgb04c_out90.gbc` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/frame2_m0irq_count_scx2_2_dmg08_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/enable_display/lcdcenable_lyc0irq_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/enable_display/lcdcenable_lyc0irq_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/enable_display/ly0_late_cgbpr_2_cgb04c_outFF.gbc::cgb` | FAIL | expected top-left glyphs FF; glyph pixel diff=20 |
| `gambatte/enable_display/ly0_late_cgbpr_ds_2_cgb04c_outFF.gbc::cgb` | FAIL | expected top-left glyphs FF; glyph pixel diff=20 |
| `gambatte/enable_display/ly0_late_cgbpw_2_cgb04c_out55.gbc::cgb` | FAIL | expected top-left glyphs 55; glyph pixel diff=38 |
| `gambatte/enable_display/ly0_late_cgbpw_ds_2_cgb04c_out55.gbc::cgb` | FAIL | expected top-left glyphs 55; glyph pixel diff=38 |
| `gambatte/enable_display/ly0_late_oamw_1_dmg08_cgb04c_outAA.gbc` | FAIL | expected top-left glyphs AA; glyph pixel diff=38 |
| `gambatte/enable_display/ly0_late_oamw_1_dmg08_cgb04c_outAA.gbc::cgb` | FAIL | expected top-left glyphs AA; glyph pixel diff=38 |
| `gambatte/enable_display/ly0_late_oamw_ds_1_cgb04c_outAA.gbc::cgb` | FAIL | expected top-left glyphs AA; glyph pixel diff=38 |
| `gambatte/enable_display/ly0_late_scx7_m3stat_scx0_1_dmg08_cgb04c_out87.gbc` | FAIL | expected top-left glyphs 87; glyph pixel diff=21 |
| `gambatte/enable_display/ly0_late_scx7_m3stat_scx0_1_dmg08_cgb04c_out87.gbc::cgb` | FAIL | expected top-left glyphs 87; glyph pixel diff=21 |
| `gambatte/enable_display/ly0_late_scx7_m3stat_scx0_2_dmg08_out87_cgb04c_out84.gbc` | FAIL | expected top-left glyphs 87; glyph pixel diff=21 |
| `gambatte/enable_display/ly0_late_scx7_m3stat_scx1_2_dmg08_cgb04c_out84.gbc` | FAIL | expected top-left glyphs 84; glyph pixel diff=21 |
| `gambatte/enable_display/ly0_late_scx7_m3stat_scx1_2_dmg08_cgb04c_out84.gbc::cgb` | FAIL | expected top-left glyphs 84; glyph pixel diff=21 |
| `gambatte/enable_display/ly0_late_scx7_m3stat_scx3_1_dmg08_cgb04c_out87.gbc` | FAIL | expected top-left glyphs 87; glyph pixel diff=21 |
| `gambatte/enable_display/ly0_late_scx7_m3stat_scx3_1_dmg08_cgb04c_out87.gbc::cgb` | FAIL | expected top-left glyphs 87; glyph pixel diff=21 |
| `gambatte/enable_display/ly0_late_vramr_2_dmg08_outFF_cgb04c_out55.gbc::cgb` | FAIL | expected top-left glyphs 55; glyph pixel diff=20 |
| `gambatte/enable_display/ly0_late_vramw_2_dmg08_out55_cgb04c_outAA.gbc::cgb` | FAIL | expected top-left glyphs AA; glyph pixel diff=38 |
| `gambatte/enable_display/ly1_late_cgbpw_2_cgb04c_out55.gbc::cgb` | FAIL | expected top-left glyphs 55; glyph pixel diff=38 |
| `gambatte/enable_display/nextstat_1_dmg08_cgb04c_out84.gbc` | FAIL | expected top-left glyphs 84; glyph pixel diff=15 |
| `gambatte/enable_display/nextstat_1_dmg08_cgb04c_out84.gbc::cgb` | FAIL | expected top-left glyphs 84; glyph pixel diff=15 |
| `gambatte/enable_display/stat_dmg08_cgb04c_out84.gbc` | FAIL | expected top-left glyphs 84; glyph pixel diff=15 |
| `gambatte/enable_display/stat_dmg08_cgb04c_out84.gbc::cgb` | FAIL | expected top-left glyphs 84; glyph pixel diff=15 |
| `gambatte/halt/ifandie_ei_halt_m2int_m0stat_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/ifandie_ei_halt_m2int_m0stat_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx2_1a_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx2_1a_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx2_2a_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx2_2a_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx2_3a_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx2_3a_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx2_4a_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx2_4a_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx3_1b_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx3_3b_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0int_halt_m0stat_scx3_4b_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_dec_scx2_2_dmg08_cgb04c_out6.gbc` | FAIL | expected top-left glyphs 6; glyph pixel diff=22 |
| `gambatte/halt/late_m0irq_halt_dec_scx2_2_dmg08_cgb04c_out6.gbc::cgb` | FAIL | expected top-left glyphs 6; glyph pixel diff=22 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx2_1a_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx2_1a_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx2_2a_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx2_2a_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx2_3a_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx2_3a_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx2_4a_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx2_4a_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx3_1b_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx3_3a_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx3_3a_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx3_4a_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/late_m0irq_halt_m0stat_scx3_4a_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/lycint_dmgpalette_during_m3_1.gb` | FAIL | pixel diff=286/23040 vs res/tests/gambatte/halt/lycint_dmgpalette_during_m3_1.png |
| `gambatte/halt/lycint_dmgpalette_during_m3_2.gb` | FAIL | pixel diff=286/23040 vs res/tests/gambatte/halt/lycint_dmgpalette_during_m3_2.png |
| `gambatte/halt/lycint_dmgpalette_during_m3_3.gb` | FAIL | pixel diff=572/23040 vs res/tests/gambatte/halt/lycint_dmgpalette_during_m3_3.png |
| `gambatte/halt/lycint_dmgpalette_during_m3_4.gb` | FAIL | pixel diff=572/23040 vs res/tests/gambatte/halt/lycint_dmgpalette_during_m3_4.png |
| `gambatte/halt/lycirq_m2stat_2_dmg08_out2_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=5 |
| `gambatte/halt/m0int_m0stat_scx2_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0int_m0stat_scx2_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0int_m0stat_scx3_2_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0int_m0stat_scx4_2_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0int_m0stat_scx5_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0int_m0stat_scx5_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0irq_m0stat_scx2_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0irq_m0stat_scx2_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0irq_m0stat_scx3_2_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0irq_m0stat_scx4_2_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0irq_m0stat_scx5_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m0irq_m0stat_scx5_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/m1int_ly_2_dmg08_out90_cgb04c_out91.gbc::cgb` | FAIL | expected top-left glyphs 91; glyph pixel diff=27 |
| `gambatte/halt/noime_ifandie_m2int_m0stat_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/noime_ifandie_m2int_m0stat_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/noime_m2irq_m0stat_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/halt/noime_m2irq_m0stat_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/irq_precedence/hdma_vs_m0_scx2_cgb04c_out0183.gbc::cgb` | FAIL | expected top-left glyphs 0183; glyph pixel diff=76 |
| `gambatte/irq_precedence/late_hdma_vs_ei_scx1_1_cgb04c_out102E.gbc::cgb` | FAIL | expected top-left glyphs 102E; glyph pixel diff=31 |
| `gambatte/irq_precedence/late_hdma_vs_ei_scx2_1_cgb04c_out102F.gbc::cgb` | FAIL | expected top-left glyphs 102F; glyph pixel diff=27 |
| `gambatte/irq_precedence/late_hdma_vs_ie_scx1_1_cgb04c_out102E.gbc::cgb` | FAIL | expected top-left glyphs 102E; glyph pixel diff=31 |
| `gambatte/irq_precedence/late_hdma_vs_ie_scx2_1_cgb04c_out102F.gbc::cgb` | FAIL | expected top-left glyphs 102F; glyph pixel diff=27 |
| `gambatte/irq_precedence/late_hdma_vs_tima_scx1_2_cgb04c_out11E9.gbc::cgb` | FAIL | expected top-left glyphs 11E9; glyph pixel diff=46 |
| `gambatte/irq_precedence/late_hdma_vs_tima_scx1_halt_2_cgb04c_out11C9.gbc::cgb` | FAIL | expected top-left glyphs 11C9; glyph pixel diff=54 |
| `gambatte/irq_precedence/late_hdma_vs_tima_scx2_2_cgb04c_out11E9.gbc::cgb` | FAIL | expected top-left glyphs 11E9; glyph pixel diff=46 |
| `gambatte/irq_precedence/late_hdma_vs_tima_scx2_halt_2_cgb04c_out11C9.gbc::cgb` | FAIL | expected top-left glyphs 11C9; glyph pixel diff=54 |
| `gambatte/irq_precedence/late_m0irq_retrigger_2_dmg08_cgb04c_outE0.gbc` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/irq_precedence/late_m0irq_retrigger_2_dmg08_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/irq_precedence/late_m0irq_retrigger_scx1_2_dmg08_cgb04c_outE0.gbc` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/irq_precedence/late_m0irq_retrigger_scx1_2_dmg08_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/irq_precedence/late_m0irq_retrigger_scx1_ds_2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/irq_precedence/late_m0irq_vs_tima_scx2_1_dmg08_cgb04c_out4.gbc` | FAIL | expected top-left glyphs 4; glyph pixel diff=17 |
| `gambatte/irq_precedence/late_m0irq_vs_tima_scx2_1_dmg08_cgb04c_out4.gbc::cgb` | FAIL | expected top-left glyphs 4; glyph pixel diff=17 |
| `gambatte/irq_precedence/late_m0irq_vs_tima_scx2_halt_1_dmg08_cgb04c_out4.gbc` | FAIL | expected top-left glyphs 4; glyph pixel diff=17 |
| `gambatte/irq_precedence/late_m0irq_vs_tima_scx2_halt_1_dmg08_cgb04c_out4.gbc::cgb` | FAIL | expected top-left glyphs 4; glyph pixel diff=17 |
| `gambatte/irq_precedence/late_m0irq_vs_tima_scx3_1_dmg08_cgb04c_out4.gbc` | FAIL | expected top-left glyphs 4; glyph pixel diff=17 |
| `gambatte/irq_precedence/late_m0irq_vs_tima_scx3_1_dmg08_cgb04c_out4.gbc::cgb` | FAIL | expected top-left glyphs 4; glyph pixel diff=17 |
| `gambatte/irq_precedence/late_m0irq_vs_tima_scx3_halt_1_dmg08_cgb04c_out4.gbc` | FAIL | expected top-left glyphs 4; glyph pixel diff=17 |
| `gambatte/irq_precedence/late_m0irq_vs_tima_scx3_halt_1_dmg08_cgb04c_out4.gbc::cgb` | FAIL | expected top-left glyphs 4; glyph pixel diff=17 |
| `gambatte/lcd_offset/offset1_lyc8fint_m1stat_ds_1_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=18 |
| `gambatte/lcd_offset/offset1_lyc98int_ly_count_2_cgb04c_out9A.gbc::cgb` | FAIL | expected top-left glyphs 9A; glyph pixel diff=27 |
| `gambatte/lcd_offset/offset1_lyc98int_ly_count_ds_2_cgb04c_out9A.gbc::cgb` | FAIL | expected top-left glyphs 9A; glyph pixel diff=27 |
| `gambatte/lcd_offset/offset1_lyc98int_ly_count_ds_3_cgb04c_out9A.gbc::cgb` | FAIL | expected top-left glyphs 9A; glyph pixel diff=27 |
| `gambatte/lcd_offset/offset1_lyc99int_m0irq_count_scx1_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/lcd_offset/offset1_lyc99int_m0stat_count_scx1_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/lcd_offset/offset1_lyc99int_m2irq_count_ds_2_cgb04c_out91.gbc::cgb` | FAIL | expected top-left glyphs 91; glyph pixel diff=7 |
| `gambatte/lcd_offset/offset1_lyc99int_m2stat_count_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/lcd_offset/offset1_lyc99int_m3stat_count_ds_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/lcd_offset/offset2_lyc98int_ly_count_2_cgb04c_out9A.gbc::cgb` | FAIL | expected top-left glyphs 9A; glyph pixel diff=27 |
| `gambatte/lcd_offset/offset2_lyc99int_m0stat_count_scx1_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/lcd_offset/offset3_lyc8fint_m1stat_1_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=18 |
| `gambatte/lcd_offset/offset3_lyc98int_ly_count_1_cgb04c_out99.gbc::cgb` | FAIL | expected top-left glyphs 99; glyph pixel diff=14 |
| `gambatte/lcd_offset/offset3_lyc98int_ly_count_2_cgb04c_out9A.gbc::cgb` | FAIL | expected top-left glyphs 9A; glyph pixel diff=27 |
| `gambatte/lcd_offset/offset3_lyc99int_m0stat_count_scx0_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/lcd_offset/offset3_lyc99int_m0stat_count_scx1_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/lcd_offset/offset3_lyc99int_m2irq_count_2_cgb04c_out91.gbc::cgb` | FAIL | expected top-left glyphs 91; glyph pixel diff=7 |
| `gambatte/lcd_offset/offset3_lyc99int_m2stat_count_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/lcd_offset/offset3_lyc99int_m3stat_count_2_cgb04c_out90.gbc::cgb` | FAIL | expected top-left glyphs 90; glyph pixel diff=7 |
| `gambatte/lcdirq_precedence/lcdirqprecedence_lycirq_ly44_lcdstat58_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lcdirq_precedence/lcdirqprecedence_lycirq_ly44_lcdstat58_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lcdirq_precedence/lycirq_ly44_lcdstat48_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lcdirq_precedence/lycirq_ly44_lcdstat48_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lcdirq_precedence/m1irq_lcdstat50_lyc8f_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/lcdirq_precedence/m1irq_lcdstat50_lyc8f_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/ly0/lycint152_ly0stat_2_dmg08_cgb04c_outC0.gbc` | FAIL | expected top-left glyphs C0; glyph pixel diff=27 |
| `gambatte/ly0/lycint152_ly0stat_2_dmg08_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=27 |
| `gambatte/ly0/lycint152_ly153_2_dmg08_cgb04c_out99.gbc` | FAIL | expected top-left glyphs 99; glyph pixel diff=8 |
| `gambatte/ly0/lycint152_ly153_2_dmg08_cgb04c_out99.gbc::cgb` | FAIL | expected top-left glyphs 99; glyph pixel diff=8 |
| `gambatte/ly0/lycint152_ly153_3_dmg08_cgb04c_out00.gbc` | FAIL | expected top-left glyphs 00; glyph pixel diff=14 |
| `gambatte/ly0/lycint152_ly153_3_dmg08_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=14 |
| `gambatte/ly0/lycint152_ly153_ds_2_cgb04c_out99.gbc::cgb` | FAIL | expected top-left glyphs 99; glyph pixel diff=8 |
| `gambatte/ly0/lycint152_ly153_ds_5_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=14 |
| `gambatte/ly0/lycint152_ly1_m2irq_2_dmg08_cgb04c_outE2.gbc` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/ly0/lycint152_ly1_m2irq_2_dmg08_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/ly0/lycint152_lyc0flag_4_dmg08_cgb04c_outC0.gbc` | FAIL | expected top-left glyphs C0; glyph pixel diff=18 |
| `gambatte/ly0/lycint152_lyc0flag_4_dmg08_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=18 |
| `gambatte/ly0/lycint152_lyc153flag_3_dmg08_cgb04c_outC1.gbc` | FAIL | expected top-left glyphs C1; glyph pixel diff=24 |
| `gambatte/ly0/lycint152_lyc153flag_3_dmg08_cgb04c_outC1.gbc::cgb` | FAIL | expected top-left glyphs C1; glyph pixel diff=24 |
| `gambatte/lycEnable/ff41_disable_2_dmg08_out0_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lycEnable/ff41_disable_3_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lycEnable/ff41_disable_3_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lycEnable/ff45_disable_2_dmg08_out1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/lycEnable/ff45_enable_weirdpoint_2_dmg08_out3_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/lycEnable/ff45_enable_weirdpoint_3_dmg08_out1_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/lycEnable/ff45_enable_weirdpoint_ds_2_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/lycEnable/ff45_enable_weirdpoint_ds_3_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/lycEnable/ff45_enable_weirdpoint_ds_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/lycEnable/ff45_enable_weirdpoint_ds_lcdoffset1_3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/lycEnable/ff45_enable_weirdpoint_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/lycEnable/late_ff41_enable_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/lycEnable/late_ff41_enable_after_m2int_disable_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lycEnable/late_ff41_enable_after_m2int_disable_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lycEnable/late_ff41_enable_after_m2int_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lycEnable/late_ff41_enable_after_m2int_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lycEnable/late_ff41_enable_ds_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/lycEnable/late_ff45_enable_2_dmg08_out3_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/lycEnable/late_ff45_enable_3_dmg08_cgb04c_out1.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/lycEnable/late_ff45_enable_3_dmg08_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/lycEnable/late_ff45_enable_after_m2int_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lycEnable/late_ff45_enable_after_m2int_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lycEnable/late_ff45_enable_ds_2_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/lycEnable/late_ff45_enable_ds_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/lycEnable/late_ff45_enable_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/lycEnable/lcdoff_lycirqen_1_dmg08_cgb04c_outE2.gbc` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/lycEnable/lcdoff_lycirqen_1_dmg08_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/lycEnable/lcdoff_lycirqen_4_dmg08_outE2_cgb04c_outE0.gbc` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc0_ff41_disable_1_dmg08_outE2_cgb04c_outE0.gbc` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc0_ff41_disable_2_dmg08_cgb04c_outE2.gbc` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc0_ff41_disable_2_dmg08_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc0_ff45_disable_2_dmg08_outE0_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc0_late_ff45_enable_2_dmg08_outE2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc0_late_ff45_enable_3_dmg08_cgb04c_outE0.gbc` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc0_late_ff45_enable_3_dmg08_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc0_m1disable_2_dmg08_outE2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_enable_m1disable_2_dmg08_outE2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff41_enable_2_dmg08_outE2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff41_enable_ds_lcdoffset1_2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff45_enable_2_dmg08_outE2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff45_enable_3_dmg08_outE0_cgb04c_outE2.gbc` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff45_enable_4_dmg08_outE2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff45_enable_5_dmg08_cgb04c_outE0.gbc` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff45_enable_5_dmg08_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff45_enable_ds_2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff45_enable_ds_3_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff45_enable_ds_6_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff45_enable_ds_lcdoffset1_2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_ff45_enable_lcdoffset1_2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc153_late_m1disable_2_dmg08_outE2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=9 |
| `gambatte/lycEnable/lyc_ff45_disable2_2_dmg08_out1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/lycEnable/lyc_ff45_trigger_delay_2_dmg08_out0_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/lycEnable/lycwirq_trigger_ly00_stat50_2_dmg08_outE0_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/lycEnable/lycwirq_trigger_ly00_stat50_ds_lcdoffset1_2_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/lycint_ly/lycint_ly_2_dmg08_cgb04c_out6.gbc` | FAIL | expected top-left glyphs 6; glyph pixel diff=4 |
| `gambatte/lycint_ly/lycint_ly_2_dmg08_cgb04c_out6.gbc::cgb` | FAIL | expected top-left glyphs 6; glyph pixel diff=4 |
| `gambatte/lycint_ly/lycint_ly_ds_2_cgb04c_out6.gbc::cgb` | FAIL | expected top-left glyphs 6; glyph pixel diff=4 |
| `gambatte/lycint_lycflag/lycint_lycflag_4_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=18 |
| `gambatte/lycint_lycflag/lycint_lycflag_4_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=18 |
| `gambatte/lycm2int/lycm2int_m0stat_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/lycm2int/lycm2int_m0stat_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/lycm2int/lycm2int_m0stat_2_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=26 |
| `gambatte/lycm2int/lycm2int_m0stat_2_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=26 |
| `gambatte/lycm2int/lycm2int_m2irq_2_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/lycm2int/lycm2int_m2irq_2_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/lycm2int/m2irq_before_lycint_1_dmg08_cgb04c_out1.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/lycm2int/m2irq_before_lycint_1_dmg08_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/lycm2int/m2irq_before_lycint_2_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/lycm2int/m2irq_before_lycint_2_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m0enable/disable_2_dmg08_out0_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/disable_scx1_2_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/disable_scx1_2_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/disable_scx2_2_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/disable_scx2_2_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/disable_scx3_2_dmg08_out0_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/disable_scx4_2_dmg08_out0_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/disable_scx5_2_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/disable_scx5_2_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/disable_scx5_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m0enable/disable_scx7_2_dmg08_out0_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/enable_wxA6_2x_spxA7_ds_1_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m0enable/late_enable_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/late_enable_ds_2_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/m0enable/late_enable_ds_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff41_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff41_scx1_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff41_scx1_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff41_scx1_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff41_scx2_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff41_scx2_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff41_scx3_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_3_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_scx1_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_scx1_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_scx1_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_scx1_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_scx2_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_scx2_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_scx2_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_scx3_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_scx3_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0enable/lycdisable_ff45_scx3_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0int_m0stat/m0int_m0stat_scx2_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m0int_m0stat/m0int_m0stat_scx2_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m1/ly143_late_m0enable_2_dmg08_out3_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/m1/ly143_late_m0enable_ds_lcdoffset1_2_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/m1/ly143_late_m2enable_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/ly143_late_m2enable_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/ly143_late_m2enable_2_dmg08_out3_cgb04c_out1.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/ly143_late_m2enable_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/ly143_late_m2enable_ds_lcdoffset1_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lyc143_late_m0enable_lycdisable_3_dmg08_out3_cgb04c_out1.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lyc143_late_m2enable_lycdisable_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lyc143_late_m2enable_lycdisable_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lyc143_late_m2enable_lycdisable_3_dmg08_out3_cgb04c_out1.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lyc143_late_m2enable_lycdisable_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lycint143_m1irq_2_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lycint143_m1irq_2_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lycint143_m1irq_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lycint143_m1irq_ifw_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lycint143_m1irq_ifw_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lycint143_m1irq_ifw_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lycint143_m1irq_late_retrigger_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lycint143_m1irq_late_retrigger_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lycint143_m1irq_late_retrigger_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/lycint_m1intirq_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m1/lycint_m1intirq_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m1/lycint_m1intirq_2_dmg08_cgb04c_out1.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/m1/lycint_m1intirq_2_dmg08_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/m1/lycint_vblankirq_late_retrigger_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/m1/lycint_vblankirq_late_retrigger_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/m1/lycint_vblankirq_late_retrigger_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/m1/m1irq_disable_1_dmg08_out3_cgb04c_out1.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/m1irq_enable_after_lyc144_2_dmg08_out1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/m1irq_late_enable_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m1/m1irq_late_enable_ds_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m1/m1irq_m0disable_2_dmg08_out3_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/m1/m1irq_m2disable_lycdisable_2_dmg08_out3_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/m1/m1irq_m2enable_lyc_2_dmg08_out1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/m1irq_m2enable_lyc_3_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/m1irq_m2enable_lyc_3_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/m1irq_m2enable_lyc_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m1/m2m1irq_ifw_2_dmg08_cgb04c_out1.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/m1/m2m1irq_ifw_2_dmg08_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/m1/m2m1irq_ifw_ds_2_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/m2enable/disable_1_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m2enable/disable_ly0_1_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m2enable/enable_after_lycint_disable_2_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m2enable/enable_after_lycint_disable_2_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/m2enable/late_enable_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_after_lycint_2_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_after_lycint_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_after_lycint_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_after_lycint_4_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_after_lycint_4_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_after_lycint_disable_2_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_after_lycint_disable_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_after_lycint_disable_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_ds_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_lcdoffset2_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_lcdoffset3_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_ly0_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_ly0_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_ly0_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_ly0_ds_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_ly0_lcdoffset2_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_m0disable_2_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_m0disable_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_m0disable_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_m0disable_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_m1disable_ly0_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_m1disable_ly0_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_m1disable_ly0_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_enable_m1disable_ly0_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/late_m1disable_ly0_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc0_late_m2enable_lycdisable_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_late_m2enable_lycdisable_1_dmg08_out0_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_late_m2enable_lycdisable_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_late_m2enable_lycdisable_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_late_m2enable_lycdisable_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_m2irq_late_lyc255_2_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_m2irq_late_lyc255_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_m2irq_late_lyc255_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_m2irq_late_lyc255_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_m2irq_late_lycdisable_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_m2irq_late_lycdisable_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/lyc1_m2irq_late_lycdisable_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/m2_late_m0disable_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/m2_late_m0disable_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2enable/m2_late_m0disable_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_ds_1_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx3_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx3_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx3_di_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=11 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx3_di_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=11 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx3_ei_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx3_ei_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx3_ie_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=11 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx3_ie_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=11 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx3_reti_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx3_reti_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx4_ifw_1_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx4_ifw_1_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx4_ifw_3_dmg08_cgb04c_out8.gbc` | FAIL | expected top-left glyphs 8; glyph pixel diff=11 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx4_ifw_3_dmg08_cgb04c_out8.gbc::cgb` | FAIL | expected top-left glyphs 8; glyph pixel diff=11 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx4_ifw_ds_1_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/m2int_m0irq/m2int_m0irq_scx5_ds_1_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=25 |
| `gambatte/m2int_m0stat/m2int_m0stat_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0stat/m2int_m0stat_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m0stat/m2int_m0stat_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m2irq/m2int_m2irq_late_retrigger_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m2irq/m2int_m2irq_late_retrigger_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m2irq/m2int_m2irq_late_retrigger_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/m2int_m2stat/m2int_m2stat_1_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=5 |
| `gambatte/m2int_m2stat/m2int_m2stat_1_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=5 |
| `gambatte/m2int_m2stat/m2int_m2stat_ds_1_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=5 |
| `gambatte/m2int_m2stat/m2int_scx4_m2stat_ds_1_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=5 |
| `gambatte/m2int_m3stat/m2int_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/m2int_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/m2int_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/nobg/m2int_nobg_m3stat_1_cgb04c_out3.gb::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=59 |
| `gambatte/m2int_m3stat/nobg/m2int_nobg_m3stat_2_cgb04c_out0.gb::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=64 |
| `gambatte/m2int_m3stat/nobg/m2int_nobg_scx7_m3stat_1_cgb04c_out3.gb::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=64 |
| `gambatte/m2int_m3stat/scx/late_scx4_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/scx/late_scx4_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/scx/late_scx4_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/scx/m2int_scx1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/scx/m2int_scx2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/scx/m2int_scx3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/scx/m2int_scx4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/scx/m2int_scx5_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/scx/m2int_scx6_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/scx/m2int_scx7_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/m2int_m3stat/scx/m2int_scx8_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/miscmstatirq/lycflag_statwirq_1_dmg08_out2.gb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycflag_statwirq_2_dmg08_out2.gb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycflag_statwirq_3_dmg08_out2.gb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_00_00_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_00_bf_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_bf_00_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_bf_40_dmg08_out2_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_bf_40_dmg08_out2_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_bf_bf_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_bf_ff_dmg08_out2_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_bf_ff_dmg08_out2_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_ly00_10_50_ds_lcdoffset1_2_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_ly00_10_50_lcdoffset3_2_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycstatwirq_trigger_m0_late_ly44_lyc44_08_40_ds_3_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_1_dmg08_cgb04c_outE2.gbc` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_1_dmg08_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_2_dmg08_cgb04c_outE2.gbc` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_2_dmg08_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_3_dmg08_cgb04c_outE2.gbc` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_3_dmg08_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_4_dmg08_cgb04c_outE2.gbc` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_4_dmg08_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_ds_1_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_ds_2_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/lycwirq_trigger_m0_late_ly44_lyc45_ds_3_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m0statwirq_1_dmg08_out2.gb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m0statwirq_4_dmg08_out2.gb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m0statwirq_scx2_2_dmg08_out2.gb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m0statwirq_scx3_2_dmg08_out2.gb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m0statwirq_scx5_2_dmg08_out2.gb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m0statwirq_trigger_00_00_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m0statwirq_trigger_00_f7_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m0statwirq_trigger_f7_00_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m0statwirq_trigger_f7_f7_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m1statwirq_1_dmg08_out3.gb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/miscmstatirq/m1statwirq_2_dmg08_out3.gb` | FAIL | expected top-left glyphs 3; glyph pixel diff=25 |
| `gambatte/miscmstatirq/m1statwirq_3_dmg08_out2.gb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m1statwirq_trigger_00_00_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m1statwirq_trigger_00_ef_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m1statwirq_trigger_ef_00_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m1statwirq_trigger_ef_ef_dmg08_out2_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m1statwirq_trigger_ly94_lyc94_40_50_2_dmg08_outE0_cgb04c_outE2.gbc::cgb` | FAIL | expected top-left glyphs E2; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m2statwirq_trigger_00_20_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m2statwirq_trigger_00_20_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m2statwirq_trigger_00_ff_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m2statwirq_trigger_00_ff_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m2statwirq_trigger_df_20_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m2statwirq_trigger_df_20_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m2statwirq_trigger_df_ff_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/miscmstatirq/m2statwirq_trigger_df_ff_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/oam_access/10spritesprline_postread_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oam_access/10spritesprline_postread_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oam_access/midwrite_2_dmg08_out1_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/oam_access/postread_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oam_access/postread_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oam_access/postread_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oam_access/postread_scx5_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oam_access/postwrite_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/oam_access/postwrite_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/oam_access/postwrite_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/oam_access/postwrite_scx1_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/oam_access/preread_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oam_access/preread_ds_lcdoffset1_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oam_access/prewrite_2_dmg08_out1_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/oam_access/prewrite_ds_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/oamdma/late_sp00x_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp00x_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp00x_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp00y_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp00y_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp00y_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp01x_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp01x_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp01x_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp01y_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp01y_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp01y_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp02x_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp02x_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp02y_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp02y_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp39x_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp39x_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp39x_4_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp39x_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp39y_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp39y_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/late_sp39y_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oamdma/oamdma_late_halt_stat_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/oamdma_late_halt_stat_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/oamdma/oamdma_late_speedchange_stat_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oamdma/oamdma_src0000_busycallAFFF_dmg08_cgb04c_outFF8F.gbc` | FAIL | expected top-left glyphs FF8F; glyph pixel diff=54 |
| `gambatte/oamdma/oamdma_src0000_busycallAFFF_dmg08_cgb04c_outFF8F.gbc::cgb` | FAIL | expected top-left glyphs FF8F; glyph pixel diff=54 |
| `gambatte/oamdma/oamdma_src0000_busyint0002_dmg08_cgb04c_outFF941234.gbc` | FAIL | expected top-left glyphs FF941234; glyph pixel diff=53 |
| `gambatte/oamdma/oamdma_src0000_busyint0002_dmg08_cgb04c_outFF941234.gbc::cgb` | FAIL | expected top-left glyphs FF941234; glyph pixel diff=53 |
| `gambatte/oamdma/oamdma_src0000_busypopDFFF_dmg08_out65766576_cgb04c_out657655AA.gbc::cgb` | FAIL | expected top-left glyphs 657655AA; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_src0000_busypopEFFF_dmg08_out65766576_cgb04c_out657655AA.gbc::cgb` | FAIL | expected top-left glyphs 657655AA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_src0000_busypopFDFF_dmg08_out657665FF_cgb04c_out657655FF.gbc::cgb` | FAIL | expected top-left glyphs 657655FF; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_src0000_busypush0001_dmg08_cgb04c_out5576AA34.gbc` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src0000_busypush0001_dmg08_cgb04c_out5576AA34.gbc::cgb` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src0000_busypush8001_dmg08_cgb04c_out65AA1255.gbc` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src0000_busypush8001_dmg08_cgb04c_out65AA1255.gbc::cgb` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src0000_busypushA001_2_dmg08_cgb04c_out5576AAFF.gbc` | FAIL | expected top-left glyphs 5576AAFF; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src0000_busypushA001_2_dmg08_cgb04c_out5576AAFF.gbc::cgb` | FAIL | expected top-left glyphs 5576AAFF; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src0000_busypushA001_dmg08_cgb04c_out5576AA34.gbc` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src0000_busypushA001_dmg08_cgb04c_out5576AA34.gbc::cgb` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src0000_busypushC001_2_dmg08_out55AAFF34_cgb04c_out65AAFF55.gbc` | FAIL | expected top-left glyphs 55AAFF34; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_src0000_busypushC001_2_dmg08_out55AAFF34_cgb04c_out65AAFF55.gbc::cgb` | FAIL | expected top-left glyphs 65AAFF55; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src0000_busypushC001_dmg08_out55AA1234_cgb04c_out65AA1255.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_src0000_busypushC001_dmg08_out55AA1234_cgb04c_out65AA1255.gbc::cgb` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src0000_busypushE001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_src0000_busypushE001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc::cgb` | FAIL | expected top-left glyphs 6576AA55; glyph pixel diff=38 |
| `gambatte/oamdma/oamdma_src0000_busypushF001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_src0000_busypushF001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc::cgb` | FAIL | expected top-left glyphs 6576AA55; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_src0000_busypushFE01_dmg08_out65AA1298_cgb04c_out6576AA98.gbc` | FAIL | expected top-left glyphs 65AA1298; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src0000_busypushFE01_dmg08_out65AA1298_cgb04c_out6576AA98.gbc::cgb` | FAIL | expected top-left glyphs 6576AA98; glyph pixel diff=38 |
| `gambatte/oamdma/oamdma_src0000_busypushFEA1_dmg08_out65768700_cgb04c_out65768734.gbc::cgb` | FAIL | expected top-left glyphs 65768734; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_src0000_busypushFF01_dmg08_out657600DF_cgb04c_out657612DF.gbc::cgb` | FAIL | expected top-left glyphs 657612DF; glyph pixel diff=36 |
| `gambatte/oamdma/oamdma_src0000_busyread0000_ds_1_cgb04c_out4.gbc::cgb` | FAIL | expected top-left glyphs 4; glyph pixel diff=21 |
| `gambatte/oamdma/oamdma_src0000_busyread0000_ds_2_cgb04c_out5.gbc::cgb` | FAIL | expected top-left glyphs 5; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_src0000_busyrst0002_dmg08_cgb04c_outFF8DFA9E.gbc` | FAIL | expected top-left glyphs FF8DFA9E; glyph pixel diff=43 |
| `gambatte/oamdma/oamdma_src0000_busyrst0002_dmg08_cgb04c_outFF8DFA9E.gbc::cgb` | FAIL | expected top-left glyphs FF8DFA9E; glyph pixel diff=43 |
| `gambatte/oamdma/oamdma_src7F00_busypopBFFF_2_dmg08_out65766576_cgb04c_out657665AA.gbc::cgb` | FAIL | expected top-left glyphs 657665AA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_src7F00_busypopBFFF_dmg08_out65766576_cgb04c_out657665AA.gbc::cgb` | FAIL | expected top-left glyphs 657665AA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_src7F00_busypopDFFF_dmg08_out65766576_cgb04c_out657655AA.gbc::cgb` | FAIL | expected top-left glyphs 657655AA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_src7F00_busypopEFFF_dmg08_out65766576_cgb04c_out657655AA.gbc::cgb` | FAIL | expected top-left glyphs 657655AA; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_src7F00_busypush0001_dmg08_cgb04c_out5576AA34.gbc` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src7F00_busypush0001_dmg08_cgb04c_out5576AA34.gbc::cgb` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src7F00_busypush8001_dmg08_cgb04c_out65AA1255.gbc` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src7F00_busypush8001_dmg08_cgb04c_out65AA1255.gbc::cgb` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src7F00_busypushA001_2_dmg08_cgb04c_out5576AAFF.gbc` | FAIL | expected top-left glyphs 5576AAFF; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src7F00_busypushA001_2_dmg08_cgb04c_out5576AAFF.gbc::cgb` | FAIL | expected top-left glyphs 5576AAFF; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src7F00_busypushA001_dmg08_cgb04c_out5576AA34.gbc` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src7F00_busypushA001_dmg08_cgb04c_out5576AA34.gbc::cgb` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src7F00_busypushC001_2_dmg08_out55AAFF34_cgb04c_out65AAFF55.gbc` | FAIL | expected top-left glyphs 55AAFF34; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_src7F00_busypushC001_2_dmg08_out55AAFF34_cgb04c_out65AAFF55.gbc::cgb` | FAIL | expected top-left glyphs 65AAFF55; glyph pixel diff=64 |
| `gambatte/oamdma/oamdma_src7F00_busypushC001_dmg08_out55AA1234_cgb04c_out65AA1255.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_src7F00_busypushC001_dmg08_out55AA1234_cgb04c_out65AA1255.gbc::cgb` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=64 |
| `gambatte/oamdma/oamdma_src7F00_busypushE001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_src7F00_busypushE001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc::cgb` | FAIL | expected top-left glyphs 6576AA55; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_src7F00_busypushF001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_src7F00_busypushF001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc::cgb` | FAIL | expected top-left glyphs 6576AA55; glyph pixel diff=38 |
| `gambatte/oamdma/oamdma_src7F00_busypushFE01_dmg08_out65AA1298_cgb04c_out6576AA98.gbc` | FAIL | expected top-left glyphs 65AA1298; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src7F00_busypushFEA1_dmg08_out65768700_cgb04c_out65768734.gbc::cgb` | FAIL | expected top-left glyphs 65768734; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_src7F00_busypushFF01_dmg08_out657600DF_cgb04c_out657612DF.gbc::cgb` | FAIL | expected top-left glyphs 657612DF; glyph pixel diff=36 |
| `gambatte/oamdma/oamdma_src8000_busypop7FFF_dmg08_out65765576_cgb04c_out65005576.gbc::cgb` | FAIL | expected top-left glyphs 65005576; glyph pixel diff=26 |
| `gambatte/oamdma/oamdma_src8000_busypop9FFF_2_dmg08_out657665FF_cgb04c_out007665FF.gbc::cgb` | FAIL | expected top-left glyphs 007665FF; glyph pixel diff=18 |
| `gambatte/oamdma/oamdma_src8000_busypop9FFF_dmg08_out657665AA_cgb04c_out007665AA.gbc::cgb` | FAIL | expected top-left glyphs 007665AA; glyph pixel diff=18 |
| `gambatte/oamdma/oamdma_src8000_busypush8001_dmg08_out55761234_cgb04c_out00761234.gbc` | FAIL | expected top-left glyphs 55761234; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src8000_busypush8001_dmg08_out55761234_cgb04c_out00761234.gbc::cgb` | FAIL | expected top-left glyphs 00761234; glyph pixel diff=18 |
| `gambatte/oamdma/oamdma_src8000_busypushA001_2_dmg08_out65AA12FF_cgb04c_out650012FF.gbc` | FAIL | expected top-left glyphs 65AA12FF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src8000_busypushA001_2_dmg08_out65AA12FF_cgb04c_out650012FF.gbc::cgb` | FAIL | expected top-left glyphs 650012FF; glyph pixel diff=26 |
| `gambatte/oamdma/oamdma_src8000_busypushA001_dmg08_out65AA1255_cgb04c_out65001255.gbc` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src8000_busypushA001_dmg08_out65AA1255_cgb04c_out65001255.gbc::cgb` | FAIL | expected top-left glyphs 65001255; glyph pixel diff=26 |
| `gambatte/oamdma/oamdma_src8000_busypushFEA1_dmg08_out65768700_cgb04c_out65768734.gbc::cgb` | FAIL | expected top-left glyphs 65768734; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_src8000_busypushFF01_dmg08_out657600DF_cgb04c_out657612DF.gbc::cgb` | FAIL | expected top-left glyphs 657612DF; glyph pixel diff=36 |
| `gambatte/oamdma/oamdma_src8000_busywrite8000_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=18 |
| `gambatte/oamdma/oamdma_src8000_busywrite8000_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=18 |
| `gambatte/oamdma/oamdma_src8000_srcchange0000_busyinc_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/oamdma/oamdma_src8000_srcchange0000_busyinc_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=27 |
| `gambatte/oamdma/oamdma_src8000_vrambankchange_2_cgb04c_out4.gbc::cgb` | FAIL | expected top-left glyphs 4; glyph pixel diff=18 |
| `gambatte/oamdma/oamdma_src8000_vrambankchange_4_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/oamdma/oamdma_src9F00_busypop7FFF_dmg08_out65765576_cgb04c_out65005576.gbc::cgb` | FAIL | expected top-left glyphs 65005576; glyph pixel diff=26 |
| `gambatte/oamdma/oamdma_src9F00_busypop9FFF_2_dmg08_out657665FF_cgb04c_out007665FF.gbc::cgb` | FAIL | expected top-left glyphs 007665FF; glyph pixel diff=18 |
| `gambatte/oamdma/oamdma_src9F00_busypop9FFF_dmg08_out657665AA_cgb04c_out007665AA.gbc::cgb` | FAIL | expected top-left glyphs 007665AA; glyph pixel diff=18 |
| `gambatte/oamdma/oamdma_src9F00_busypush8001_dmg08_out55761234_cgb04c_out00761234.gbc` | FAIL | expected top-left glyphs 55761234; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_src9F00_busypush8001_dmg08_out55761234_cgb04c_out00761234.gbc::cgb` | FAIL | expected top-left glyphs 00761234; glyph pixel diff=18 |
| `gambatte/oamdma/oamdma_src9F00_busypushA001_2_dmg08_out65AA12FF_cgb04c_out650012FF.gbc` | FAIL | expected top-left glyphs 65AA12FF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src9F00_busypushA001_2_dmg08_out65AA12FF_cgb04c_out650012FF.gbc::cgb` | FAIL | expected top-left glyphs 650012FF; glyph pixel diff=26 |
| `gambatte/oamdma/oamdma_src9F00_busypushA001_dmg08_out65AA1255_cgb04c_out65001255.gbc` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_src9F00_busypushA001_dmg08_out65AA1255_cgb04c_out65001255.gbc::cgb` | FAIL | expected top-left glyphs 65001255; glyph pixel diff=26 |
| `gambatte/oamdma/oamdma_src9F00_busypushFEA1_dmg08_out65768700_cgb04c_out65768734.gbc::cgb` | FAIL | expected top-left glyphs 65768734; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_src9F00_busypushFF01_dmg08_out657600DF_cgb04c_out657612DF.gbc::cgb` | FAIL | expected top-left glyphs 657612DF; glyph pixel diff=36 |
| `gambatte/oamdma/oamdma_srcA000_busypopDFFF_dmg08_out65766576_cgb04c_out657655AA.gbc::cgb` | FAIL | expected top-left glyphs 657655AA; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_srcA000_busypopEFFF_dmg08_out65766576_cgb04c_out657655AA.gbc::cgb` | FAIL | expected top-left glyphs 657655AA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_srcA000_busypopFDFF_dmg08_out657665FF_cgb04c_out657655FF.gbc::cgb` | FAIL | expected top-left glyphs 657655FF; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_srcA000_busypush0001_dmg08_cgb04c_out5576AA34.gbc` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_srcA000_busypush0001_dmg08_cgb04c_out5576AA34.gbc::cgb` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_srcA000_busypush8001_dmg08_cgb04c_out65AA1255.gbc` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcA000_busypush8001_dmg08_cgb04c_out65AA1255.gbc::cgb` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcA000_busypushA001_2_dmg08_cgb04c_out55FFAAFF.gbc` | FAIL | expected top-left glyphs 55FFAAFF; glyph pixel diff=20 |
| `gambatte/oamdma/oamdma_srcA000_busypushA001_2_dmg08_cgb04c_out55FFAAFF.gbc::cgb` | FAIL | expected top-left glyphs 55FFAAFF; glyph pixel diff=20 |
| `gambatte/oamdma/oamdma_srcA000_busypushA001_dmg08_cgb04c_out5576AA34.gbc` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_srcA000_busypushA001_dmg08_cgb04c_out5576AA34.gbc::cgb` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_srcA000_busypushC001_2_dmg08_out55AAFF34_cgb04c_outFFAAFF55.gbc` | FAIL | expected top-left glyphs 55AAFF34; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcA000_busypushC001_2_dmg08_out55AAFF34_cgb04c_outFFAAFF55.gbc::cgb` | FAIL | expected top-left glyphs FFAAFF55; glyph pixel diff=26 |
| `gambatte/oamdma/oamdma_srcA000_busypushC001_dmg08_out55AA1234_cgb04c_out65AA1255.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcA000_busypushC001_dmg08_out55AA1234_cgb04c_out65AA1255.gbc::cgb` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcA000_busypushE001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcA000_busypushE001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc::cgb` | FAIL | expected top-left glyphs 6576AA55; glyph pixel diff=38 |
| `gambatte/oamdma/oamdma_srcA000_busypushF001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcA000_busypushF001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc::cgb` | FAIL | expected top-left glyphs 6576AA55; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_srcA000_busypushFE01_dmg08_out65AA1298_cgb04c_out6576AA98.gbc` | FAIL | expected top-left glyphs 65AA1298; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcA000_busypushFE01_dmg08_out65AA1298_cgb04c_out6576AA98.gbc::cgb` | FAIL | expected top-left glyphs 6576AA98; glyph pixel diff=38 |
| `gambatte/oamdma/oamdma_srcA000_busypushFEA1_dmg08_out65768700_cgb04c_out65768734.gbc::cgb` | FAIL | expected top-left glyphs 65768734; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcA000_busypushFF01_dmg08_out657600DF_cgb04c_out657612DF.gbc::cgb` | FAIL | expected top-left glyphs 657612DF; glyph pixel diff=36 |
| `gambatte/oamdma/oamdma_srcA000_busywrite4000_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=17 |
| `gambatte/oamdma/oamdma_srcA000_busywrite4000_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=17 |
| `gambatte/oamdma/oamdma_srcBF00_busypopBFFF_2_dmg08_outFFFFFFFF_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_srcBF00_busypopBFFF_dmg08_out65766576_cgb04c_out657665AA.gbc::cgb` | FAIL | expected top-left glyphs 657665AA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_srcBF00_busypopDFFF_dmg08_out65766576_cgb04c_out657655AA.gbc::cgb` | FAIL | expected top-left glyphs 657655AA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_srcBF00_busypopEFFF_dmg08_out65766576_cgb04c_out657655AA.gbc::cgb` | FAIL | expected top-left glyphs 657655AA; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_srcBF00_busypush0001_dmg08_cgb04c_out5576AA34.gbc` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_srcBF00_busypush0001_dmg08_cgb04c_out5576AA34.gbc::cgb` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_srcBF00_busypush8001_dmg08_cgb04c_out65AA1255.gbc` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcBF00_busypush8001_dmg08_cgb04c_out65AA1255.gbc::cgb` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcBF00_busypushA001_2_dmg08_cgb04c_out55FFAAFF.gbc` | FAIL | expected top-left glyphs 55FFAAFF; glyph pixel diff=20 |
| `gambatte/oamdma/oamdma_srcBF00_busypushA001_2_dmg08_cgb04c_out55FFAAFF.gbc::cgb` | FAIL | expected top-left glyphs 55FFAAFF; glyph pixel diff=20 |
| `gambatte/oamdma/oamdma_srcBF00_busypushA001_dmg08_cgb04c_out5576AA34.gbc` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_srcBF00_busypushA001_dmg08_cgb04c_out5576AA34.gbc::cgb` | FAIL | expected top-left glyphs 5576AA34; glyph pixel diff=4 |
| `gambatte/oamdma/oamdma_srcBF00_busypushC001_2_dmg08_out55AAFF34_cgb04c_outFFAAFF55.gbc` | FAIL | expected top-left glyphs 55AAFF34; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcBF00_busypushC001_2_dmg08_out55AAFF34_cgb04c_outFFAAFF55.gbc::cgb` | FAIL | expected top-left glyphs FFAAFF55; glyph pixel diff=48 |
| `gambatte/oamdma/oamdma_srcBF00_busypushC001_dmg08_out55AA1234_cgb04c_out65AA1255.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcBF00_busypushC001_dmg08_out55AA1234_cgb04c_out65AA1255.gbc::cgb` | FAIL | expected top-left glyphs 65AA1255; glyph pixel diff=64 |
| `gambatte/oamdma/oamdma_srcBF00_busypushE001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcBF00_busypushE001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc::cgb` | FAIL | expected top-left glyphs 6576AA55; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_srcBF00_busypushF001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc` | FAIL | expected top-left glyphs 55AA1234; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcBF00_busypushF001_dmg08_out55AA1234_cgb04c_out6576AA55.gbc::cgb` | FAIL | expected top-left glyphs 6576AA55; glyph pixel diff=38 |
| `gambatte/oamdma/oamdma_srcBF00_busypushFE01_dmg08_out65AA1298_cgb04c_out6576AA98.gbc` | FAIL | expected top-left glyphs 65AA1298; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcBF00_busypushFEA1_dmg08_out65768700_cgb04c_out65768734.gbc::cgb` | FAIL | expected top-left glyphs 65768734; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcBF00_busypushFF01_dmg08_out657600DF_cgb04c_out657612DF.gbc::cgb` | FAIL | expected top-left glyphs 657612DF; glyph pixel diff=36 |
| `gambatte/oamdma/oamdma_srcC000_busypush0001_dmg08_out4576AA34_cgb04c_out6576AA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcC000_busypush8001_dmg08_out65221255_cgb04c_out65761255.gbc` | FAIL | expected top-left glyphs 65221255; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcC000_busypushA001_2_dmg08_out4576AAFF_cgb04c_out6576AAFF.gbc` | FAIL | expected top-left glyphs 4576AAFF; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcC000_busypushA001_dmg08_out4576AA34_cgb04c_out6576AA55.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcC000_busypushC001_2_dmg08_out4522FF34_cgb04c_out6576FF34.gbc` | FAIL | expected top-left glyphs 4522FF34; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcC000_busypushC001_dmg08_out45221234_cgb04c_out6576AA34.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcC000_busypushE001_dmg08_out45221234_cgb04c_out65761234.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcC000_busypushF001_dmg08_out45221234_cgb04c_out65761234.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcC000_busypushFE01_dmg08_out65221298_cgb04c_out65761298.gbc` | FAIL | expected top-left glyphs 65221298; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcC000_busypushFEA1_dmg08_out65768700_cgb04c_out65768734.gbc::cgb` | FAIL | expected top-left glyphs 65768734; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcC000_busypushFF01_dmg08_out657600DF_cgb04c_out657612DF.gbc::cgb` | FAIL | expected top-left glyphs 657612DF; glyph pixel diff=36 |
| `gambatte/oamdma/oamdma_srcD000_wrambankchange_2_cgb04c_out4.gbc::cgb` | FAIL | expected top-left glyphs 4; glyph pixel diff=18 |
| `gambatte/oamdma/oamdma_srcDF00_busypush0001_dmg08_out4576AA34_cgb04c_out6576AA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcDF00_busypush8001_dmg08_out65221255_cgb04c_out65761255.gbc` | FAIL | expected top-left glyphs 65221255; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcDF00_busypushA001_2_dmg08_out4576AAFF_cgb04c_out6576AAFF.gbc` | FAIL | expected top-left glyphs 4576AAFF; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcDF00_busypushA001_dmg08_out4576AA34_cgb04c_out6576AA55.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcDF00_busypushC001_2_dmg08_out4522FF34_cgb04c_out6576FF34.gbc` | FAIL | expected top-left glyphs 4522FF34; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcDF00_busypushC001_dmg08_out45221234_cgb04c_out6576AA34.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcDF00_busypushE001_dmg08_out45221234_cgb04c_out65761234.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcDF00_busypushF001_dmg08_out45221234_cgb04c_out65761234.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcDF00_busypushFE01_dmg08_out65221298_cgb04c_out65761298.gbc` | FAIL | expected top-left glyphs 65221298; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcDF00_busypushFEA1_dmg08_out65768700_cgb04c_out65768734.gbc::cgb` | FAIL | expected top-left glyphs 65768734; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcDF00_busypushFF01_dmg08_out657600DF_cgb04c_out657612DF.gbc::cgb` | FAIL | expected top-left glyphs 657612DF; glyph pixel diff=36 |
| `gambatte/oamdma/oamdma_srcE000_busypop7FFF_dmg08_out657665AA_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=62 |
| `gambatte/oamdma/oamdma_srcE000_busypop9FFF_2_dmg08_out65765576_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcE000_busypop9FFF_dmg08_out65765576_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=68 |
| `gambatte/oamdma/oamdma_srcE000_busypopBFFF_2_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=84 |
| `gambatte/oamdma/oamdma_srcE000_busypopBFFF_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=104 |
| `gambatte/oamdma/oamdma_srcE000_busypopDFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc::cgb` | FAIL | expected top-left glyphs FFFF55AA; glyph pixel diff=88 |
| `gambatte/oamdma/oamdma_srcE000_busypopEFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc::cgb` | FAIL | expected top-left glyphs FFFF55AA; glyph pixel diff=88 |
| `gambatte/oamdma/oamdma_srcE000_busypopFDFF_dmg08_out657665FF_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcE000_busypopFE9F_dmg08_out6576FFFF_cgb04c_outFFFFFFFF.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFFF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcE000_busypopFEFF_dmg08_out6576FFEF_cgb04c_outFFFFFFEF.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFEF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcE000_busypopFF7F_dmg08_out6576FFAA_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcE000_busypopFFFF_dmg08_out65765576_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=68 |
| `gambatte/oamdma/oamdma_srcE000_busypush0001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcE000_busypush0001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc::cgb` | FAIL | expected top-left glyphs 55FFAA34; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcE000_busypush8001_dmg08_out65221255_cgb04c_outFFAA1255.gbc` | FAIL | expected top-left glyphs 65221255; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcE000_busypush8001_dmg08_out65221255_cgb04c_outFFAA1255.gbc::cgb` | FAIL | expected top-left glyphs FFAA1255; glyph pixel diff=60 |
| `gambatte/oamdma/oamdma_srcE000_busypushA001_2_dmg08_out4576AAFF_cgb04c_out55FFAAFF.gbc` | FAIL | expected top-left glyphs 4576AAFF; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcE000_busypushA001_2_dmg08_out4576AAFF_cgb04c_out55FFAAFF.gbc::cgb` | FAIL | expected top-left glyphs 55FFAAFF; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcE000_busypushA001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcE000_busypushA001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc::cgb` | FAIL | expected top-left glyphs 55FFAA34; glyph pixel diff=50 |
| `gambatte/oamdma/oamdma_srcE000_busypushC001_2_dmg08_out4522FF34_cgb04c_outFFAAFF55.gbc` | FAIL | expected top-left glyphs 4522FF34; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcE000_busypushC001_2_dmg08_out4522FF34_cgb04c_outFFAAFF55.gbc::cgb` | FAIL | expected top-left glyphs FFAAFF55; glyph pixel diff=82 |
| `gambatte/oamdma/oamdma_srcE000_busypushC001_dmg08_out45221234_cgb04c_outFFAA1255.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcE000_busypushC001_dmg08_out45221234_cgb04c_outFFAA1255.gbc::cgb` | FAIL | expected top-left glyphs FFAA1255; glyph pixel diff=120 |
| `gambatte/oamdma/oamdma_srcE000_busypushE001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcE000_busypushE001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc::cgb` | FAIL | expected top-left glyphs FFFFAA55; glyph pixel diff=102 |
| `gambatte/oamdma/oamdma_srcE000_busypushF001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcE000_busypushF001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc::cgb` | FAIL | expected top-left glyphs FFFFAA55; glyph pixel diff=102 |
| `gambatte/oamdma/oamdma_srcE000_busypushFE01_dmg08_out65221298_cgb04c_outFFFFAAFF.gbc` | FAIL | expected top-left glyphs 65221298; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcE000_busypushFE01_dmg08_out65221298_cgb04c_outFFFFAAFF.gbc::cgb` | FAIL | expected top-left glyphs FFFFAAFF; glyph pixel diff=106 |
| `gambatte/oamdma/oamdma_srcE000_busypushFEA1_dmg08_out65768700_cgb04c_outFFFFFF34.gbc::cgb` | FAIL | expected top-left glyphs FFFFFF34; glyph pixel diff=100 |
| `gambatte/oamdma/oamdma_srcE000_busypushFF01_dmg08_out657600DF_cgb04c_outFFFF12DF.gbc::cgb` | FAIL | expected top-left glyphs FFFF12DF; glyph pixel diff=78 |
| `gambatte/oamdma/oamdma_srcE000_busypushFF81_dmg08_out6576FF55_cgb04c_outFFFFFF55.gbc::cgb` | FAIL | expected top-left glyphs FFFFFF55; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcE000_readFE00_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/oamdma/oamdma_srcEF00_busypop7FFF_dmg08_out657665AA_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=62 |
| `gambatte/oamdma/oamdma_srcEF00_busypop9FFF_2_dmg08_out65765576_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcEF00_busypop9FFF_dmg08_out65765576_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=68 |
| `gambatte/oamdma/oamdma_srcEF00_busypopBFFF_2_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=84 |
| `gambatte/oamdma/oamdma_srcEF00_busypopBFFF_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=104 |
| `gambatte/oamdma/oamdma_srcEF00_busypopDFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc::cgb` | FAIL | expected top-left glyphs FFFF55AA; glyph pixel diff=88 |
| `gambatte/oamdma/oamdma_srcEF00_busypopEFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc::cgb` | FAIL | expected top-left glyphs FFFF55AA; glyph pixel diff=88 |
| `gambatte/oamdma/oamdma_srcEF00_busypopFDFF_dmg08_out657665FF_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcEF00_busypopFE9F_dmg08_out6576FFFF_cgb04c_outFFFFFFFF.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFFF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcEF00_busypopFEFF_dmg08_out6576FFEF_cgb04c_outFFFFFFEF.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFEF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcEF00_busypopFF7F_dmg08_out6576FFAA_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcEF00_busypopFFFF_dmg08_out65765576_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=68 |
| `gambatte/oamdma/oamdma_srcEF00_busypush0001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcEF00_busypush0001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc::cgb` | FAIL | expected top-left glyphs 55FFAA34; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcEF00_busypush8001_dmg08_out65221255_cgb04c_outFFAA1255.gbc` | FAIL | expected top-left glyphs 65221255; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcEF00_busypush8001_dmg08_out65221255_cgb04c_outFFAA1255.gbc::cgb` | FAIL | expected top-left glyphs FFAA1255; glyph pixel diff=60 |
| `gambatte/oamdma/oamdma_srcEF00_busypushA001_2_dmg08_out4576AAFF_cgb04c_out55FFAAFF.gbc` | FAIL | expected top-left glyphs 4576AAFF; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcEF00_busypushA001_2_dmg08_out4576AAFF_cgb04c_out55FFAAFF.gbc::cgb` | FAIL | expected top-left glyphs 55FFAAFF; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcEF00_busypushA001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcEF00_busypushA001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc::cgb` | FAIL | expected top-left glyphs 55FFAA34; glyph pixel diff=50 |
| `gambatte/oamdma/oamdma_srcEF00_busypushC001_2_dmg08_out4522FF34_cgb04c_outFFAAFF55.gbc` | FAIL | expected top-left glyphs 4522FF34; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcEF00_busypushC001_2_dmg08_out4522FF34_cgb04c_outFFAAFF55.gbc::cgb` | FAIL | expected top-left glyphs FFAAFF55; glyph pixel diff=82 |
| `gambatte/oamdma/oamdma_srcEF00_busypushC001_dmg08_out45221234_cgb04c_outFFAA1255.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcEF00_busypushC001_dmg08_out45221234_cgb04c_outFFAA1255.gbc::cgb` | FAIL | expected top-left glyphs FFAA1255; glyph pixel diff=120 |
| `gambatte/oamdma/oamdma_srcEF00_busypushE001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcEF00_busypushE001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc::cgb` | FAIL | expected top-left glyphs FFFFAA55; glyph pixel diff=102 |
| `gambatte/oamdma/oamdma_srcEF00_busypushF001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcEF00_busypushF001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc::cgb` | FAIL | expected top-left glyphs FFFFAA55; glyph pixel diff=102 |
| `gambatte/oamdma/oamdma_srcEF00_busypushFE01_dmg08_out65221298_cgb04c_outFFFFAAFF.gbc` | FAIL | expected top-left glyphs 65221298; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcEF00_busypushFE01_dmg08_out65221298_cgb04c_outFFFFAAFF.gbc::cgb` | FAIL | expected top-left glyphs FFFFAAFF; glyph pixel diff=106 |
| `gambatte/oamdma/oamdma_srcEF00_busypushFEA1_dmg08_out65768700_cgb04c_outFFFFFF34.gbc::cgb` | FAIL | expected top-left glyphs FFFFFF34; glyph pixel diff=100 |
| `gambatte/oamdma/oamdma_srcEF00_busypushFF01_dmg08_out657600DF_cgb04c_outFFFF12DF.gbc::cgb` | FAIL | expected top-left glyphs FFFF12DF; glyph pixel diff=78 |
| `gambatte/oamdma/oamdma_srcEF00_busypushFF81_dmg08_out6576FF55_cgb04c_outFFFFFF55.gbc::cgb` | FAIL | expected top-left glyphs FFFFFF55; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcF000_busypop7FFF_dmg08_out657665AA_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcF000_busypop9FFF_2_dmg08_out65765576_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcF000_busypop9FFF_dmg08_out65765576_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcF000_busypopBFFF_2_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=84 |
| `gambatte/oamdma/oamdma_srcF000_busypopBFFF_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=84 |
| `gambatte/oamdma/oamdma_srcF000_busypopDFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc::cgb` | FAIL | expected top-left glyphs FFFF55AA; glyph pixel diff=88 |
| `gambatte/oamdma/oamdma_srcF000_busypopEFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc::cgb` | FAIL | expected top-left glyphs FFFF55AA; glyph pixel diff=88 |
| `gambatte/oamdma/oamdma_srcF000_busypopFDFF_dmg08_out657665FF_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=46 |
| `gambatte/oamdma/oamdma_srcF000_busypopFE9F_dmg08_out6576FFFF_cgb04c_outFFFFFFFF.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFFF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcF000_busypopFEFF_dmg08_out6576FFEF_cgb04c_outFFFFFFEF.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFEF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcF000_busypopFF7F_dmg08_out6576FFAA_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcF000_busypopFFFF_dmg08_out65765576_cgb04c_outFFFF55FF.gbc::cgb` | FAIL | expected top-left glyphs FFFF55FF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcF000_busypush0001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcF000_busypush0001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc::cgb` | FAIL | expected top-left glyphs 55FFAA34; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcF000_busypush8001_dmg08_out65221255_cgb04c_outFFAA1255.gbc` | FAIL | expected top-left glyphs 65221255; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcF000_busypush8001_dmg08_out65221255_cgb04c_outFFAA1255.gbc::cgb` | FAIL | expected top-left glyphs FFAA1255; glyph pixel diff=60 |
| `gambatte/oamdma/oamdma_srcF000_busypushA001_2_dmg08_out4576AAFF_cgb04c_out55FFAAFF.gbc` | FAIL | expected top-left glyphs 4576AAFF; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcF000_busypushA001_2_dmg08_out4576AAFF_cgb04c_out55FFAAFF.gbc::cgb` | FAIL | expected top-left glyphs 55FFAAFF; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcF000_busypushA001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcF000_busypushA001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc::cgb` | FAIL | expected top-left glyphs 55FFAA34; glyph pixel diff=50 |
| `gambatte/oamdma/oamdma_srcF000_busypushC001_2_dmg08_out4522FF34_cgb04c_outFFAAFF55.gbc` | FAIL | expected top-left glyphs 4522FF34; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcF000_busypushC001_2_dmg08_out4522FF34_cgb04c_outFFAAFF55.gbc::cgb` | FAIL | expected top-left glyphs FFAAFF55; glyph pixel diff=82 |
| `gambatte/oamdma/oamdma_srcF000_busypushC001_dmg08_out45221234_cgb04c_outFFAA1255.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcF000_busypushC001_dmg08_out45221234_cgb04c_outFFAA1255.gbc::cgb` | FAIL | expected top-left glyphs FFAA1255; glyph pixel diff=120 |
| `gambatte/oamdma/oamdma_srcF000_busypushE001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcF000_busypushE001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc::cgb` | FAIL | expected top-left glyphs FFFFAA55; glyph pixel diff=102 |
| `gambatte/oamdma/oamdma_srcF000_busypushF001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcF000_busypushF001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc::cgb` | FAIL | expected top-left glyphs FFFFAA55; glyph pixel diff=102 |
| `gambatte/oamdma/oamdma_srcF000_busypushFE01_dmg08_out65221298_cgb04c_outFFFFAAFF.gbc` | FAIL | expected top-left glyphs 65221298; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcF000_busypushFE01_dmg08_out65221298_cgb04c_outFFFFAAFF.gbc::cgb` | FAIL | expected top-left glyphs FFFFAAFF; glyph pixel diff=106 |
| `gambatte/oamdma/oamdma_srcF000_busypushFEA1_dmg08_out65768700_cgb04c_outFFFFFF34.gbc::cgb` | FAIL | expected top-left glyphs FFFFFF34; glyph pixel diff=100 |
| `gambatte/oamdma/oamdma_srcF000_busypushFF01_dmg08_out657600DF_cgb04c_outFFFF12DF.gbc::cgb` | FAIL | expected top-left glyphs FFFF12DF; glyph pixel diff=78 |
| `gambatte/oamdma/oamdma_srcF000_busypushFF81_dmg08_out6576FF55_cgb04c_outFFFFFF55.gbc::cgb` | FAIL | expected top-left glyphs FFFFFF55; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcFD00_readFE00_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/oamdma/oamdma_srcFE00_busypop7FFF_dmg08_out657665AA_cgb04c_outFFFFFFAA.gbc` | FAIL | expected top-left glyphs 657665AA; glyph pixel diff=60 |
| `gambatte/oamdma/oamdma_srcFE00_busypop9FFF_2_dmg08_out65765576_cgb04c_outFFFF55FF.gbc` | FAIL | expected top-left glyphs 65765576; glyph pixel diff=66 |
| `gambatte/oamdma/oamdma_srcFE00_busypop9FFF_dmg08_out65765576_cgb04c_outFFFF55FF.gbc` | FAIL | expected top-left glyphs 65765576; glyph pixel diff=66 |
| `gambatte/oamdma/oamdma_srcFE00_busypopBFFF_2_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc` | FAIL | expected top-left glyphs 65766576; glyph pixel diff=84 |
| `gambatte/oamdma/oamdma_srcFE00_busypopBFFF_2_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_srcFE00_busypopBFFF_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc` | FAIL | expected top-left glyphs 65766576; glyph pixel diff=84 |
| `gambatte/oamdma/oamdma_srcFE00_busypopBFFF_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_srcFE00_busypopDFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc` | FAIL | expected top-left glyphs 65766576; glyph pixel diff=84 |
| `gambatte/oamdma/oamdma_srcFE00_busypopDFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc::cgb` | FAIL | expected top-left glyphs FFFF55AA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_srcFE00_busypopEFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc` | FAIL | expected top-left glyphs 65766576; glyph pixel diff=84 |
| `gambatte/oamdma/oamdma_srcFE00_busypopEFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc::cgb` | FAIL | expected top-left glyphs FFFF55AA; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_srcFE00_busypopFDFF_dmg08_out657665FF_cgb04c_outFFFF55FF.gbc` | FAIL | expected top-left glyphs 657665FF; glyph pixel diff=60 |
| `gambatte/oamdma/oamdma_srcFE00_busypopFE9F_dmg08_out6576FFFF_cgb04c_outFFFFFFFF.gbc` | FAIL | expected top-left glyphs 6576FFFF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcFE00_busypopFEFF_dmg08_out6576FFEF_cgb04c_outFFFFFFEF.gbc` | FAIL | expected top-left glyphs 6576FFEF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcFE00_busypopFF7F_dmg08_out6576FFAA_cgb04c_outFFFFFFAA.gbc` | FAIL | expected top-left glyphs 6576FFAA; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcFE00_busypopFFFF_dmg08_out65765576_cgb04c_outFFFF55FF.gbc` | FAIL | expected top-left glyphs 65765576; glyph pixel diff=66 |
| `gambatte/oamdma/oamdma_srcFE00_busypush0001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=47 |
| `gambatte/oamdma/oamdma_srcFE00_busypush0001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc::cgb` | FAIL | expected top-left glyphs 55FFAA34; glyph pixel diff=20 |
| `gambatte/oamdma/oamdma_srcFE00_busypush8001_dmg08_out65221255_cgb04c_outFFAA1255.gbc` | FAIL | expected top-left glyphs 65221255; glyph pixel diff=38 |
| `gambatte/oamdma/oamdma_srcFE00_busypush8001_dmg08_out65221255_cgb04c_outFFAA1255.gbc::cgb` | FAIL | expected top-left glyphs FFAA1255; glyph pixel diff=26 |
| `gambatte/oamdma/oamdma_srcFE00_busypushA001_2_dmg08_out4576AAFF_cgb04c_out55FFAAFF.gbc` | FAIL | expected top-left glyphs 4576AAFF; glyph pixel diff=47 |
| `gambatte/oamdma/oamdma_srcFE00_busypushA001_2_dmg08_out4576AAFF_cgb04c_out55FFAAFF.gbc::cgb` | FAIL | expected top-left glyphs 55FFAAFF; glyph pixel diff=20 |
| `gambatte/oamdma/oamdma_srcFE00_busypushA001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=47 |
| `gambatte/oamdma/oamdma_srcFE00_busypushA001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc::cgb` | FAIL | expected top-left glyphs 55FFAA34; glyph pixel diff=20 |
| `gambatte/oamdma/oamdma_srcFE00_busypushC001_2_dmg08_out4522FF34_cgb04c_outFFAAFF55.gbc` | FAIL | expected top-left glyphs 4522FF34; glyph pixel diff=43 |
| `gambatte/oamdma/oamdma_srcFE00_busypushC001_2_dmg08_out4522FF34_cgb04c_outFFAAFF55.gbc::cgb` | FAIL | expected top-left glyphs FFAAFF55; glyph pixel diff=48 |
| `gambatte/oamdma/oamdma_srcFE00_busypushC001_dmg08_out45221234_cgb04c_outFFAA1255.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=43 |
| `gambatte/oamdma/oamdma_srcFE00_busypushC001_dmg08_out45221234_cgb04c_outFFAA1255.gbc::cgb` | FAIL | expected top-left glyphs FFAA1255; glyph pixel diff=48 |
| `gambatte/oamdma/oamdma_srcFE00_busypushE001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=43 |
| `gambatte/oamdma/oamdma_srcFE00_busypushE001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc::cgb` | FAIL | expected top-left glyphs FFFFAA55; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_srcFE00_busypushF001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=43 |
| `gambatte/oamdma/oamdma_srcFE00_busypushF001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc::cgb` | FAIL | expected top-left glyphs FFFFAA55; glyph pixel diff=38 |
| `gambatte/oamdma/oamdma_srcFE00_busypushFE01_dmg08_out65221298_cgb04c_outFFFFAAFF.gbc` | FAIL | expected top-left glyphs 65221298; glyph pixel diff=64 |
| `gambatte/oamdma/oamdma_srcFE00_busypushFEA1_dmg08_out65768700_cgb04c_outFFFFFF34.gbc` | FAIL | expected top-left glyphs 65768700; glyph pixel diff=72 |
| `gambatte/oamdma/oamdma_srcFE00_busypushFEA1_dmg08_out65768700_cgb04c_outFFFFFF34.gbc::cgb` | FAIL | expected top-left glyphs FFFFFF34; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcFE00_busypushFF01_dmg08_out657600DF_cgb04c_outFFFF12DF.gbc` | FAIL | expected top-left glyphs 657600DF; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcFE00_busypushFF01_dmg08_out657600DF_cgb04c_outFFFF12DF.gbc::cgb` | FAIL | expected top-left glyphs FFFF12DF; glyph pixel diff=36 |
| `gambatte/oamdma/oamdma_srcFE00_busypushFF81_dmg08_out6576FF55_cgb04c_outFFFFFF55.gbc` | FAIL | expected top-left glyphs 6576FF55; glyph pixel diff=42 |
| `gambatte/oamdma/oamdma_srcFF00_busypopBFFF_2_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_srcFF00_busypopBFFF_dmg08_out65766576_cgb04c_outFFFFFFAA.gbc::cgb` | FAIL | expected top-left glyphs FFFFFFAA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_srcFF00_busypopDFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc::cgb` | FAIL | expected top-left glyphs FFFF55AA; glyph pixel diff=40 |
| `gambatte/oamdma/oamdma_srcFF00_busypopEFFF_dmg08_out65766576_cgb04c_outFFFF55AA.gbc::cgb` | FAIL | expected top-left glyphs FFFF55AA; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_srcFF00_busypush0001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcFF00_busypush0001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc::cgb` | FAIL | expected top-left glyphs 55FFAA34; glyph pixel diff=20 |
| `gambatte/oamdma/oamdma_srcFF00_busypush8001_dmg08_out65221255_cgb04c_outFFAA1255.gbc` | FAIL | expected top-left glyphs 65221255; glyph pixel diff=24 |
| `gambatte/oamdma/oamdma_srcFF00_busypush8001_dmg08_out65221255_cgb04c_outFFAA1255.gbc::cgb` | FAIL | expected top-left glyphs FFAA1255; glyph pixel diff=26 |
| `gambatte/oamdma/oamdma_srcFF00_busypushA001_2_dmg08_out4576AAFF_cgb04c_out55FFAAFF.gbc` | FAIL | expected top-left glyphs 4576AAFF; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcFF00_busypushA001_2_dmg08_out4576AAFF_cgb04c_out55FFAAFF.gbc::cgb` | FAIL | expected top-left glyphs 55FFAAFF; glyph pixel diff=20 |
| `gambatte/oamdma/oamdma_srcFF00_busypushA001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc` | FAIL | expected top-left glyphs 4576AA34; glyph pixel diff=15 |
| `gambatte/oamdma/oamdma_srcFF00_busypushA001_dmg08_out4576AA34_cgb04c_out55FFAA34.gbc::cgb` | FAIL | expected top-left glyphs 55FFAA34; glyph pixel diff=20 |
| `gambatte/oamdma/oamdma_srcFF00_busypushC001_2_dmg08_out4522FF34_cgb04c_outFFAAFF55.gbc` | FAIL | expected top-left glyphs 4522FF34; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcFF00_busypushC001_2_dmg08_out4522FF34_cgb04c_outFFAAFF55.gbc::cgb` | FAIL | expected top-left glyphs FFAAFF55; glyph pixel diff=48 |
| `gambatte/oamdma/oamdma_srcFF00_busypushC001_dmg08_out45221234_cgb04c_outFFAA1255.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcFF00_busypushC001_dmg08_out45221234_cgb04c_outFFAA1255.gbc::cgb` | FAIL | expected top-left glyphs FFAA1255; glyph pixel diff=48 |
| `gambatte/oamdma/oamdma_srcFF00_busypushE001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcFF00_busypushE001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc::cgb` | FAIL | expected top-left glyphs FFFFAA55; glyph pixel diff=22 |
| `gambatte/oamdma/oamdma_srcFF00_busypushF001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc` | FAIL | expected top-left glyphs 45221234; glyph pixel diff=39 |
| `gambatte/oamdma/oamdma_srcFF00_busypushF001_dmg08_out45221234_cgb04c_outFFFFAA55.gbc::cgb` | FAIL | expected top-left glyphs FFFFAA55; glyph pixel diff=38 |
| `gambatte/oamdma/oamdma_srcFF00_busypushFE01_dmg08_out65221298_cgb04c_outFFFFAAFF.gbc` | FAIL | expected top-left glyphs 65221298; glyph pixel diff=52 |
| `gambatte/oamdma/oamdma_srcFF00_busypushFEA1_dmg08_out65768700_cgb04c_outFFFFFF34.gbc::cgb` | FAIL | expected top-left glyphs FFFFFF34; glyph pixel diff=28 |
| `gambatte/oamdma/oamdma_srcFF00_busypushFF01_dmg08_out657600DF_cgb04c_outFFFF12DF.gbc::cgb` | FAIL | expected top-left glyphs FFFF12DF; glyph pixel diff=36 |
| `gambatte/oamdma/oamdma_srcFF00_readFE00_dmg08_out1_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=7 |
| `gambatte/oamdma/oamdma_srcFF00_readFE45_dmg08_out1_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=7 |
| `gambatte/oamdma/oamdmasrc80_halt_lycirq_read8000_dmg08_cgb04c_out81.gbc` | FAIL | expected top-left glyphs 81; glyph pixel diff=44 |
| `gambatte/oamdma/oamdmasrc80_halt_lycirq_read8000_dmg08_cgb04c_out81.gbc::cgb` | FAIL | expected top-left glyphs 81; glyph pixel diff=44 |
| `gambatte/oamdma/oamdmasrc80_halt_m2irq_read8000_dmg08_cgb04c_out81.gbc` | FAIL | expected top-left glyphs 81; glyph pixel diff=31 |
| `gambatte/oamdma/oamdmasrc80_halt_m2irq_read8000_dmg08_cgb04c_out81.gbc::cgb` | FAIL | expected top-left glyphs 81; glyph pixel diff=31 |
| `gambatte/oamdma/oamdmasrcC000_hdmasrc0000_cgb04c_out0A940C0D.gbc::cgb` | FAIL | expected top-left glyphs 0A940C0D; glyph pixel diff=23 |
| `gambatte/oamdma/oamdmasrcC0_speedchange_readC000_cgb04c_out11.gbc::cgb` | FAIL | expected top-left glyphs 11; glyph pixel diff=54 |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_2.gb` | FAIL | pixel diff=1584/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_2.png |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_3.gb` | FAIL | pixel diff=3014/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_3.png |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_4.gb` | FAIL | pixel diff=1584/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_4.png |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_5.gb` | FAIL | pixel diff=432/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_5.png |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_1.gbc::cgb` | FAIL | pixel diff=720/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_1.png |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_2.gbc::cgb` | FAIL | pixel diff=728/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_2.png |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_3.gbc::cgb` | FAIL | pixel diff=4592/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_3.png |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_4.gbc::cgb` | FAIL | pixel diff=1584/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_4.png |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_5.gbc::cgb` | FAIL | pixel diff=1584/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_5.png |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_6.gbc::cgb` | FAIL | pixel diff=1576/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_6.png |
| `gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_7.gbc::cgb` | FAIL | pixel diff=1576/23040 vs res/tests/gambatte/scx_during_m3/old/offset_3/scx_during_m3_ds_7.png |
| `gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_1.gb` | FAIL | pixel diff=432/23040 vs res/tests/gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_1.png |
| `gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_2.gb` | FAIL | pixel diff=1581/23040 vs res/tests/gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_2.png |
| `gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_3.gb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_3.png |
| `gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_4.gb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_4.png |
| `gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_1.gbc::cgb` | FAIL | pixel diff=1584/23040 vs res/tests/gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_1.png |
| `gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_2.gbc::cgb` | FAIL | pixel diff=2717/23040 vs res/tests/gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_2.png |
| `gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_3.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_3.png |
| `gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_4.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_4.png |
| `gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_5.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_5.png |
| `gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_6.gbc::cgb` | FAIL | pixel diff=2288/23040 vs res/tests/gambatte/scx_during_m3/old/revoffset_3/scx_during_m3_ds_6.png |
| `gambatte/scx_during_m3/old/scx_during_m3_2.gb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/scx_during_m3_2.png |
| `gambatte/scx_during_m3/old/scx_during_m3_3.gb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/scx_during_m3_3.png |
| `gambatte/scx_during_m3/old/scx_during_m3_4.gb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/scx_during_m3_4.png |
| `gambatte/scx_during_m3/old/scx_during_m3_ds_1.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/scx_during_m3_ds_1.png |
| `gambatte/scx_during_m3/old/scx_during_m3_ds_2.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scx_during_m3/old/scx_during_m3_ds_2.png |
| `gambatte/scx_during_m3/old/scx_during_m3_ds_3.gbc::cgb` | FAIL | pixel diff=2296/23040 vs res/tests/gambatte/scx_during_m3/old/scx_during_m3_ds_3.png |
| `gambatte/scx_during_m3/old/scx_during_m3_ds_4.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/scx_during_m3_ds_4.png |
| `gambatte/scx_during_m3/old/scx_during_m3_ds_5.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/old/scx_during_m3_ds_5.png |
| `gambatte/scx_during_m3/old/scx_during_m3_ds_6.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scx_during_m3/old/scx_during_m3_ds_6.png |
| `gambatte/scx_during_m3/old/scx_during_m3_ds_7.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scx_during_m3/old/scx_during_m3_ds_7.png |
| `gambatte/scx_during_m3/scx1_scx0_during_m3_1.gbc::cgb` | FAIL | pixel diff=13960/23040 vs res/tests/gambatte/scx_during_m3/scx1_scx0_during_m3_1_cgb04c.png |
| `gambatte/scx_during_m3/scx2_scx0_during_m3_1.gbc` | FAIL | pixel diff=14104/23040 vs res/tests/gambatte/scx_during_m3/scx2_scx0_during_m3_1_dmg08.png |
| `gambatte/scx_during_m3/scx2_scx0_during_m3_1.gbc::cgb` | FAIL | pixel diff=14104/23040 vs res/tests/gambatte/scx_during_m3/scx2_scx0_during_m3_1_cgb04c.png |
| `gambatte/scx_during_m3/scx2_scx1_during_m3_1.gbc::cgb` | FAIL | pixel diff=14104/23040 vs res/tests/gambatte/scx_during_m3/scx2_scx1_during_m3_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_2.gbc` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_2_dmg08.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_2.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_3.gbc` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_3_dmg08.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_3.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_4.gbc` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_4_dmg08.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_4.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_6.gbc` | FAIL | pixel diff=16/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_6_dmg08.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_6.gbc::cgb` | FAIL | pixel diff=16/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_1.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_2.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_3.gbc::cgb` | FAIL | pixel diff=2296/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_4.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_5.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_5_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_6.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_7.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scx_during_m3/scx_0060c0/scx_during_m3_ds_7_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_1.gbc` | FAIL | pixel diff=8928/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_1_dmg08.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_1.gbc::cgb` | FAIL | pixel diff=8928/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_2.gbc` | FAIL | pixel diff=9875/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_2_dmg08.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_2.gbc::cgb` | FAIL | pixel diff=9875/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_3.gbc` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_3_dmg08.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_3.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_4.gbc` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_4_dmg08.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_4.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_6.gbc` | FAIL | pixel diff=16/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_6_dmg08.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_6.gbc::cgb` | FAIL | pixel diff=16/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_1.gbc::cgb` | FAIL | pixel diff=9648/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_2.gbc::cgb` | FAIL | pixel diff=9581/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_3.gbc::cgb` | FAIL | pixel diff=2296/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_4.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_5.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_5_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_6.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_7.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scx_during_m3/scx_0063c0/scx_during_m3_ds_7_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_1.gbc` | FAIL | pixel diff=8640/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_1_dmg08.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_1.gbc::cgb` | FAIL | pixel diff=8640/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_2.gbc` | FAIL | pixel diff=9648/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_2_dmg08.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_2.gbc::cgb` | FAIL | pixel diff=9648/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_3.gbc` | FAIL | pixel diff=10594/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_3_dmg08.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_3.gbc::cgb` | FAIL | pixel diff=10594/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_4.gbc` | FAIL | pixel diff=22312/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_4_dmg08.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_4.gbc::cgb` | FAIL | pixel diff=22312/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_5.gbc` | FAIL | pixel diff=21168/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_5_dmg08.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_5.gbc::cgb` | FAIL | pixel diff=21168/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_5_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_6.gbc` | FAIL | pixel diff=21168/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_6_dmg08.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_6.gbc::cgb` | FAIL | pixel diff=21168/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_1.gbc::cgb` | FAIL | pixel diff=9072/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_2.gbc::cgb` | FAIL | pixel diff=9077/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_3.gbc::cgb` | FAIL | pixel diff=11309/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_4.gbc::cgb` | FAIL | pixel diff=22320/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_5.gbc::cgb` | FAIL | pixel diff=22320/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_5_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_6.gbc::cgb` | FAIL | pixel diff=22312/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_7.gbc::cgb` | FAIL | pixel diff=21168/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_7_cgb04c.png |
| `gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_8.gbc::cgb` | FAIL | pixel diff=21168/23040 vs res/tests/gambatte/scx_during_m3/scx_0360c0/scx_during_m3_ds_8_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_1.gbc` | FAIL | pixel diff=432/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_1_dmg08.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_1.gbc::cgb` | FAIL | pixel diff=432/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_2.gbc` | FAIL | pixel diff=2299/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_2_dmg08.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_2.gbc::cgb` | FAIL | pixel diff=2299/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_3.gbc` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_3_dmg08.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_3.gbc::cgb` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_4.gbc` | FAIL | pixel diff=3880/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_4_dmg08.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_4.gbc::cgb` | FAIL | pixel diff=3880/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_5.gbc` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_5_dmg08.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_5.gbc::cgb` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_5_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_6.gbc` | FAIL | pixel diff=3896/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_6_dmg08.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_6.gbc::cgb` | FAIL | pixel diff=3896/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_1.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_2.gbc::cgb` | FAIL | pixel diff=1147/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_3.gbc::cgb` | FAIL | pixel diff=2728/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_4.gbc::cgb` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_5.gbc::cgb` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_5_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_6.gbc::cgb` | FAIL | pixel diff=2728/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_7.gbc::cgb` | FAIL | pixel diff=2728/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_7_cgb04c.png |
| `gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_8.gbc::cgb` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0363c0/scx_during_m3_ds_8_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_1.gbc` | FAIL | pixel diff=12096/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_1_dmg08.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_1.gbc::cgb` | FAIL | pixel diff=12096/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_2.gbc` | FAIL | pixel diff=12384/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_2_dmg08.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_2.gbc::cgb` | FAIL | pixel diff=12384/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_3.gbc` | FAIL | pixel diff=12889/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_3_dmg08.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_3.gbc::cgb` | FAIL | pixel diff=12889/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_4.gbc` | FAIL | pixel diff=3880/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_4_dmg08.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_4.gbc::cgb` | FAIL | pixel diff=3880/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_5.gbc` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_5_dmg08.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_5.gbc::cgb` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_5_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_6.gbc` | FAIL | pixel diff=3896/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_6_dmg08.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_6.gbc::cgb` | FAIL | pixel diff=3896/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_1.gbc::cgb` | FAIL | pixel diff=12384/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_2.gbc::cgb` | FAIL | pixel diff=12384/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_3.gbc::cgb` | FAIL | pixel diff=13453/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_4.gbc::cgb` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_5.gbc::cgb` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_5_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_6.gbc::cgb` | FAIL | pixel diff=2728/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_7.gbc::cgb` | FAIL | pixel diff=2728/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_7_cgb04c.png |
| `gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_8.gbc::cgb` | FAIL | pixel diff=2736/23040 vs res/tests/gambatte/scx_during_m3/scx_0367c0/scx_during_m3_ds_8_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_1.gbc` | FAIL | pixel diff=17280/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_1_dmg08.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_1.gbc::cgb` | FAIL | pixel diff=17280/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_2.gbc` | FAIL | pixel diff=17856/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_2_dmg08.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_2.gbc::cgb` | FAIL | pixel diff=17568/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_3.gbc` | FAIL | pixel diff=17856/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_3_dmg08.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_3.gbc::cgb` | FAIL | pixel diff=17280/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_4.gbc` | FAIL | pixel diff=17883/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_4_dmg08.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_4.gbc::cgb` | FAIL | pixel diff=16739/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_5.gbc` | FAIL | pixel diff=21744/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_5_dmg08.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_5.gbc::cgb` | FAIL | pixel diff=21744/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_5_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_6.gbc` | FAIL | pixel diff=21744/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_6_dmg08.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_6.gbc::cgb` | FAIL | pixel diff=21744/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_1.gbc::cgb` | FAIL | pixel diff=17280/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_1_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_2.gbc::cgb` | FAIL | pixel diff=17282/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_2_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_3.gbc::cgb` | FAIL | pixel diff=18142/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_3_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_4.gbc::cgb` | FAIL | pixel diff=17280/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_4_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_5.gbc::cgb` | FAIL | pixel diff=17319/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_5_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_6.gbc::cgb` | FAIL | pixel diff=22888/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_6_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_7.gbc::cgb` | FAIL | pixel diff=21744/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_7_cgb04c.png |
| `gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_8.gbc::cgb` | FAIL | pixel diff=21744/23040 vs res/tests/gambatte/scx_during_m3/scx_0761c0/scx_during_m3_ds_8_cgb04c.png |
| `gambatte/scx_during_m3/scx_attrib_during_m3_spx0.gbc::cgb` | FAIL | pixel diff=1216/23040 vs res/tests/gambatte/scx_during_m3/scx_attrib_during_m3_spx0.png |
| `gambatte/scx_during_m3/scx_attrib_during_m3_spx1.gbc::cgb` | FAIL | pixel diff=1208/23040 vs res/tests/gambatte/scx_during_m3/scx_attrib_during_m3_spx1.png |
| `gambatte/scx_during_m3/scx_attrib_during_m3_spx1_ds.gbc::cgb` | FAIL | pixel diff=1216/23040 vs res/tests/gambatte/scx_during_m3/scx_attrib_during_m3_spx1_ds_cgb04c.png |
| `gambatte/scx_during_m3/scx_attrib_during_m3_spx2_ds.gbc::cgb` | FAIL | pixel diff=1216/23040 vs res/tests/gambatte/scx_during_m3/scx_attrib_during_m3_spx2_ds_cgb04c.png |
| `gambatte/scx_during_m3/scx_during_m3_spx0.gbc` | FAIL | pixel diff=1216/23040 vs res/tests/gambatte/scx_during_m3/scx_during_m3_spx0_dmg08.png |
| `gambatte/scx_during_m3/scx_during_m3_spx0.gbc::cgb` | FAIL | pixel diff=1216/23040 vs res/tests/gambatte/scx_during_m3/scx_during_m3_spx0_cgb04c.png |
| `gambatte/scx_during_m3/scx_during_m3_spx1.gbc` | FAIL | pixel diff=1216/23040 vs res/tests/gambatte/scx_during_m3/scx_during_m3_spx1_dmg08.png |
| `gambatte/scx_during_m3/scx_during_m3_spx1.gbc::cgb` | FAIL | pixel diff=1208/23040 vs res/tests/gambatte/scx_during_m3/scx_during_m3_spx1_cgb04c.png |
| `gambatte/scx_during_m3/scx_during_m3_spx2.gbc` | FAIL | pixel diff=1216/23040 vs res/tests/gambatte/scx_during_m3/scx_during_m3_spx2_dmg08.png |
| `gambatte/scx_during_m3/scx_during_m3_spx2.gbc::cgb` | FAIL | pixel diff=1216/23040 vs res/tests/gambatte/scx_during_m3/scx_during_m3_spx2_cgb04c.png |
| `gambatte/scx_during_m3/scx_during_m3_spx2_ds.gbc::cgb` | FAIL | pixel diff=1216/23040 vs res/tests/gambatte/scx_during_m3/scx_during_m3_spx2_ds_cgb04c.png |
| `gambatte/scx_during_m3/scx_m3_extend_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=14 |
| `gambatte/scx_during_m3/scx_m3_extend_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=14 |
| `gambatte/scx_during_m3/scx_m3_extend_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=18 |
| `gambatte/scx_during_m3/scx_m3_extend_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=18 |
| `gambatte/scx_during_m3/scx_m3_extend_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=14 |
| `gambatte/scx_during_m3/scx_m3_extend_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=18 |
| `gambatte/scy/scx3/scy_during_m3_1.gbc` | FAIL | pixel diff=3/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_1_dmg08.png |
| `gambatte/scy/scx3/scy_during_m3_1.gbc::cgb` | FAIL | pixel diff=3/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_1_cgb04c.png |
| `gambatte/scy/scx3/scy_during_m3_2.gbc` | FAIL | pixel diff=1723/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_2_dmg08.png |
| `gambatte/scy/scx3/scy_during_m3_2.gbc::cgb` | FAIL | pixel diff=715/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_2_cgb04c.png |
| `gambatte/scy/scx3/scy_during_m3_3.gbc` | FAIL | pixel diff=1160/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_3_dmg08.png |
| `gambatte/scy/scx3/scy_during_m3_3.gbc::cgb` | FAIL | pixel diff=1160/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_3_cgb04c.png |
| `gambatte/scy/scx3/scy_during_m3_4.gbc` | FAIL | pixel diff=2152/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_4_dmg08.png |
| `gambatte/scy/scx3/scy_during_m3_4.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_4_cgb04c.png |
| `gambatte/scy/scx3/scy_during_m3_5.gbc` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_5_dmg08.png |
| `gambatte/scy/scx3/scy_during_m3_5.gbc::cgb` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_5_cgb04c.png |
| `gambatte/scy/scx3/scy_during_m3_6.gbc` | FAIL | pixel diff=1016/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_6_dmg08.png |
| `gambatte/scy/scx3/scy_during_m3_6.gbc::cgb` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scx3/scy_during_m3_6_cgb04c.png |
| `gambatte/scy/scy_during_m3_2.gbc` | FAIL | pixel diff=2152/23040 vs res/tests/gambatte/scy/scy_during_m3_2_dmg08.png |
| `gambatte/scy/scy_during_m3_2.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scy_during_m3_2_cgb04c.png |
| `gambatte/scy/scy_during_m3_3.gbc` | FAIL | pixel diff=1160/23040 vs res/tests/gambatte/scy/scy_during_m3_3_dmg08.png |
| `gambatte/scy/scy_during_m3_3.gbc::cgb` | FAIL | pixel diff=1160/23040 vs res/tests/gambatte/scy/scy_during_m3_3_cgb04c.png |
| `gambatte/scy/scy_during_m3_4.gbc` | FAIL | pixel diff=2152/23040 vs res/tests/gambatte/scy/scy_during_m3_4_dmg08.png |
| `gambatte/scy/scy_during_m3_4.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scy_during_m3_4_cgb04c.png |
| `gambatte/scy/scy_during_m3_5.gbc` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scy_during_m3_5_dmg08.png |
| `gambatte/scy/scy_during_m3_5.gbc::cgb` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scy_during_m3_5_cgb04c.png |
| `gambatte/scy/scy_during_m3_6.gbc` | FAIL | pixel diff=1016/23040 vs res/tests/gambatte/scy/scy_during_m3_6_dmg08.png |
| `gambatte/scy/scy_during_m3_6.gbc::cgb` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scy_during_m3_6_cgb04c.png |
| `gambatte/scy/scy_during_m3_ds_1.gbc::cgb` | FAIL | pixel diff=1152/23040 vs res/tests/gambatte/scy/scy_during_m3_ds_1_cgb04c.png |
| `gambatte/scy/scy_during_m3_ds_2.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scy_during_m3_ds_2_cgb04c.png |
| `gambatte/scy/scy_during_m3_ds_3.gbc::cgb` | FAIL | pixel diff=2160/23040 vs res/tests/gambatte/scy/scy_during_m3_ds_3_cgb04c.png |
| `gambatte/scy/scy_during_m3_ds_4.gbc::cgb` | FAIL | pixel diff=2160/23040 vs res/tests/gambatte/scy/scy_during_m3_ds_4_cgb04c.png |
| `gambatte/scy/scy_during_m3_ds_5.gbc::cgb` | FAIL | pixel diff=1288/23040 vs res/tests/gambatte/scy/scy_during_m3_ds_5_cgb04c.png |
| `gambatte/scy/scy_during_m3_ds_6.gbc::cgb` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scy_during_m3_ds_6_cgb04c.png |
| `gambatte/scy/scy_during_m3_ds_7.gbc::cgb` | FAIL | pixel diff=1008/23040 vs res/tests/gambatte/scy/scy_during_m3_ds_7_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx08_1.gbc` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_1_dmg08.png |
| `gambatte/scy/scy_during_m3_spx08_1.gbc::cgb` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_1_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx08_2.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_2_dmg08.png |
| `gambatte/scy/scy_during_m3_spx08_2.gbc::cgb` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_2_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx08_3.gbc` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_3_dmg08.png |
| `gambatte/scy/scy_during_m3_spx08_3.gbc::cgb` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_3_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx08_4.gbc::cgb` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_4_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx08_ds_1.gbc::cgb` | FAIL | pixel diff=1128/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_ds_1_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx08_ds_2.gbc::cgb` | FAIL | pixel diff=1016/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_ds_2_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx08_ds_3.gbc::cgb` | FAIL | pixel diff=384/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_ds_3_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx08_ds_4.gbc::cgb` | FAIL | pixel diff=1280/23040 vs res/tests/gambatte/scy/scy_during_m3_spx08_ds_4_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx09_1.gbc` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_1_dmg08.png |
| `gambatte/scy/scy_during_m3_spx09_1.gbc::cgb` | FAIL | pixel diff=232/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_1_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx09_2.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_2_dmg08.png |
| `gambatte/scy/scy_during_m3_spx09_2.gbc::cgb` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_2_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx09_3.gbc` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_3_dmg08.png |
| `gambatte/scy/scy_during_m3_spx09_3.gbc::cgb` | FAIL | pixel diff=232/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_3_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx09_4.gbc::cgb` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_4_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx09_ds_1.gbc::cgb` | FAIL | pixel diff=1128/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_ds_1_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx09_ds_2.gbc::cgb` | FAIL | pixel diff=1016/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_ds_2_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx09_ds_3.gbc::cgb` | FAIL | pixel diff=384/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_ds_3_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx09_ds_4.gbc::cgb` | FAIL | pixel diff=1280/23040 vs res/tests/gambatte/scy/scy_during_m3_spx09_ds_4_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx0A_1.gbc` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0A_1_dmg08.png |
| `gambatte/scy/scy_during_m3_spx0A_1.gbc::cgb` | FAIL | pixel diff=232/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0A_1_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx0A_2.gbc` | FAIL | pixel diff=248/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0A_2_dmg08.png |
| `gambatte/scy/scy_during_m3_spx0A_2.gbc::cgb` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0A_2_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx0A_3.gbc` | FAIL | pixel diff=1144/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0A_3_dmg08.png |
| `gambatte/scy/scy_during_m3_spx0A_3.gbc::cgb` | FAIL | pixel diff=232/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0A_3_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx0A_4.gbc::cgb` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0A_4_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx0B_1.gbc` | FAIL | pixel diff=1128/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0B_1_dmg08.png |
| `gambatte/scy/scy_during_m3_spx0B_1.gbc::cgb` | FAIL | pixel diff=232/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0B_1_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx0B_2.gbc` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0B_2_dmg08.png |
| `gambatte/scy/scy_during_m3_spx0B_2.gbc::cgb` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0B_2_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx0B_3.gbc` | FAIL | pixel diff=1128/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0B_3_dmg08.png |
| `gambatte/scy/scy_during_m3_spx0B_3.gbc::cgb` | FAIL | pixel diff=232/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0B_3_cgb04c.png |
| `gambatte/scy/scy_during_m3_spx0B_4.gbc::cgb` | FAIL | pixel diff=8/23040 vs res/tests/gambatte/scy/scy_during_m3_spx0B_4_cgb04c.png |
| `gambatte/sound/ch1_duty0_pos6_to_pos7_timing_1_dmg08_cgb04c_outaudio0.gbc` | FAIL | Expected audio0; last-frame window=35114 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/sound/ch1_duty0_pos6_to_pos7_timing_1_dmg08_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35114 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/sound/ch1_duty0_pos6_to_pos7_timing_ds_1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35115 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/sound/ch1_duty0_pos6_to_pos7_timing_ds_3_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35115 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/sound/ch1_duty0_pos6_to_pos7_timing_ds_5_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35115 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/sound/ch1_duty0_to_duty3_pos3_2_dmg08_cgb04c_outaudio0.gbc` | FAIL | Expected audio0; last-frame window=35114 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/sound/ch1_duty0_to_duty3_pos3_2_dmg08_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35114 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/sound/ch1_duty0_to_duty3_pos3_dmg08_cgb04c_outaudio0.gbc` | FAIL | Expected audio0; last-frame window=35116 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/sound/ch1_duty0_to_duty3_pos3_dmg08_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35116 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/sound/ch1_init_pos_1_dmg08_outaudio0_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35116 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch1_init_pos_2_dmg08_cgb04c_outaudio1.gbc` | FAIL | Expected audio1; last-frame window=35114 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch1_init_pos_2_dmg08_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35114 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch1_init_pos_3_dmg08_cgb04c_outaudio1.gbc` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch1_init_pos_3_dmg08_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch1_init_pos_4_dmg08_outaudio1_cgb04c_outaudio0.gbc` | FAIL | Expected audio1; last-frame window=35114 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch1_init_pos_5_dmg08_outaudio1_cgb04c_outaudio0.gbc` | FAIL | Expected audio1; last-frame window=35114 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch1_init_pos_8_dmg08_outaudio0_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35114 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch1_init_reset_sweep_counter_timing_10_dmg08_outaudio0_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch1_init_reset_sweep_counter_timing_4_dmg08_outaudio1_cgb04c_outaudio0.gbc` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch1_init_reset_sweep_counter_timing_9_dmg08_outaudio1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35118 samples, constant=False (first L=-0.25, R=-0.25) |
| `gambatte/sound/ch1_init_reset_sweep_counter_timing_nr52_1_dmg08_cgb04c_out1.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/sound/ch1_init_reset_sweep_counter_timing_nr52_3_dmg08_out0_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/sound/ch2_init_env_counter_timing_2_dmg08_outaudio1_cgb04c_outaudio0.gbc` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_env_counter_timing_3_dmg08_outaudio1_cgb04c_outaudio0.gbc` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_env_counter_timing_4_dmg08_cgb04c_outaudio1.gbc` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_env_counter_timing_4_dmg08_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_reset_env_counter_timing_11_dmg08_outaudio0_cgb04c_outaudio1.gbc` | FAIL | Expected audio0; last-frame window=35118 samples, constant=False (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_reset_env_counter_timing_14_dmg08_outaudio0_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_reset_env_counter_timing_15_dmg08_outaudio1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35118 samples, constant=False (first L=0.216667, R=0.216667) |
| `gambatte/sound/ch2_init_reset_env_counter_timing_3_dmg08_cgb04c_outaudio0.gbc` | FAIL | Expected audio0; last-frame window=35118 samples, constant=False (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_reset_env_counter_timing_3_dmg08_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35118 samples, constant=False (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_reset_env_counter_timing_4_dmg08_outaudio0_cgb04c_outaudio1.gbc` | FAIL | Expected audio0; last-frame window=35118 samples, constant=False (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_reset_env_counter_timing_5_dmg08_outaudio0_cgb04c_outaudio1.gbc` | FAIL | Expected audio0; last-frame window=35118 samples, constant=False (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_reset_env_counter_timing_7_dmg08_outaudio1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35118 samples, constant=False (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_reset_length_counter_timing_6_dmg08_outaudio1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35118 samples, constant=False (first L=-0.25, R=-0.25) |
| `gambatte/sound/ch2_init_reset_length_counter_timing_7_dmg08_outaudio0_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/sound/ch2_init_reset_length_counter_timing_nr52_1_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/sound/ch2_init_reset_length_counter_timing_nr52_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/sound/ch2_init_reset_length_counter_timing_nr52_4_dmg08_out2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/sound/ch2_late_reset_nr52_2b_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/sound/ch2_late_reset_nr52_2b_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/sound/ch2_late_reset_nr52_ds_2b_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=9 |
| `gambatte/speedchange/m2int_m3stat_lcdoffds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/m2int_m3stat_scx1_lcdoffds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_ch1_duty0_pos6_to_pos7_timing_ds_1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35115 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/speedchange/speedchange2_ch1_duty0_pos6_to_pos7_timing_nop_ds_1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35115 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/speedchange/speedchange2_ch2_nr52_1b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange2_ch2_nr52_2b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange2_ch2_nr52_ds_1b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange2_ch2_nr52_ds_2b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange2_frame1_m2int_m3stat_scx3_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_lcdoff2_m2int_m3stat_scx3_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_lcdoff2_m2int_m3stat_scx4_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_lcdoff_m2int_m3stat_scx3_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_lcdoff_nop_m2int_m3stat_scx1_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_lcdoff_nop_m2int_m3stat_scx4_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_lcdoff_nopx2_m2int_m3stat_scx3_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_ly44_m3_ly_1_cgb04c_out25.gbc::cgb` | FAIL | expected top-left glyphs 25; glyph pixel diff=20 |
| `gambatte/speedchange/speedchange2_ly44_m3_m3stat_scx2_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_ly44_m3_m3stat_scx3_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_ly44_m3_nop_m3stat_scx1_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_ly44_m3_nop_m3stat_scx4_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_ly44_m3_nopx2_m3stat_scx2_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_ly44_m3_nopx2_m3stat_scx3_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_ly44_m3_stat_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_ly44_m3_stat_4_cgb04c_outC2.gbc::cgb` | FAIL | expected top-left glyphs C2; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange2_m2int_m3stat_scx3_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_nop_ch1_duty0_pos6_to_pos7_timing_ds_1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35115 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/speedchange/speedchange2_nop_lcdoff_m2int_m3stat_scx3_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_nop_lcdoff_nop_m2int_m3stat_scx1_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_nop_lcdoff_nop_m2int_m3stat_scx4_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_nop_lcdoff_nopx2_m2int_m3stat_scx3_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_nop_ly44_m3_m3stat_scx2_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_nop_ly44_m3_m3stat_scx3_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_nop_ly44_m3_nop_m3stat_scx1_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_nop_ly44_m3_nop_m3stat_scx4_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_nop_m2int_m3stat_scx1_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_nop_m2int_m3stat_scx4_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange2_tima00_1a_cgb04c_out00.gbc::cgb` | FAIL | expected top-left glyphs 00; glyph pixel diff=27 |
| `gambatte/speedchange/speedchange2_tima00_1b_cgb04c_out01.gbc::cgb` | FAIL | expected top-left glyphs 01; glyph pixel diff=26 |
| `gambatte/speedchange/speedchange3_ch1_duty0_pos6_to_pos7_timing_2_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35115 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/speedchange/speedchange3_ch1_duty0_pos6_to_pos7_timing_nop_2_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35115 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/speedchange/speedchange3_ch2_nr52_1b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange3_ch2_nr52_2b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange3_ly44_m3_m3stat_scx1_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange3_ly44_m3_m3stat_scx2_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange3_ly44_m3_nop_m3stat_scx1_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange3_ly44_m3_nop_m3stat_scx2_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange3_nop_ch1_duty0_pos6_to_pos7_timing_2_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35115 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/speedchange/speedchange3_nop_ly44_m3_m3stat_scx1_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange3_nop_ly44_m3_m3stat_scx2_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange4_ch1_duty0_pos6_to_pos7_timing_2_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/speedchange/speedchange4_ch1_duty0_pos6_to_pos7_timing_nop_2_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35118 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/speedchange/speedchange4_ch2_nr52_1b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange4_ch2_nr52_2b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange4_ly44_m3_m3stat_scx1_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=5 |
| `gambatte/speedchange/speedchange4_ly44_m3_m3stat_scx1_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange4_ly44_m3_m3stat_scx2_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=5 |
| `gambatte/speedchange/speedchange4_ly44_m3_m3stat_scx2_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange4_ly44_m3_nop_m3stat_scx3_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=5 |
| `gambatte/speedchange/speedchange4_ly44_m3_nop_m3stat_scx3_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange4_ly44_m3_nop_m3stat_scx4_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=5 |
| `gambatte/speedchange/speedchange4_ly44_m3_nop_m3stat_scx4_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange4_nop_ly44_m3_m3stat_scx1_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=5 |
| `gambatte/speedchange/speedchange4_nop_ly44_m3_m3stat_scx1_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange4_nop_ly44_m3_m3stat_scx2_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=5 |
| `gambatte/speedchange/speedchange4_nop_ly44_m3_m3stat_scx2_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange5_ch1_duty0_pos6_to_pos7_timing_2_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35115 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/speedchange/speedchange5_ch1_duty0_pos6_to_pos7_timing_nop_2_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35115 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/speedchange/speedchange5_ch2_nr52_1b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange5_ch2_nr52_2b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange5_ly44_m3_m3stat_scx1_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange5_ly44_m3_m3stat_scx2_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange5_ly44_m3_nop_m3stat_scx1_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange5_ly44_m3_nop_m3stat_scx2_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange5_nop_ch1_duty0_pos6_to_pos7_timing_2_cgb04c_outaudio1.gbc::cgb` | FAIL | Expected audio1; last-frame window=35115 samples, constant=True (first L=0.25, R=0.25) |
| `gambatte/speedchange/speedchange5_nop_ly44_m3_m3stat_scx1_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange5_nop_ly44_m3_m3stat_scx2_2_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ch1_duty0_pos6_to_pos7_timing_ds_1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35114 samples, constant=False (first L=-0.15, R=-0.15) |
| `gambatte/speedchange/speedchange_ch1_duty0_pos6_to_pos7_timing_nop_ds_1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35118 samples, constant=False (first L=-0.15, R=-0.15) |
| `gambatte/speedchange/speedchange_ch1_nr4init_duty0_pos6_to_pos7_timing_1_cgb04c_outaudio0.gbc::cgb` | FAIL | Expected audio0; last-frame window=35115 samples, constant=False (first L=-0.0166667, R=-0.0166667) |
| `gambatte/speedchange/speedchange_ch2_nr52_1b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange_ch2_nr52_2b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange_ch2_nr52_ds_1b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange_ch2_nr52_ds_2b_cgb04c_outF0.gbc::cgb` | FAIL | expected top-left glyphs F0; glyph pixel diff=9 |
| `gambatte/speedchange/speedchange_lcdoff_tima00_1_cgb04c_out80.gbc::cgb` | FAIL | expected top-left glyphs 80; glyph pixel diff=11 |
| `gambatte/speedchange/speedchange_ly44_m3_ly_cgb04c_out39.gbc::cgb` | FAIL | expected top-left glyphs 39; glyph pixel diff=23 |
| `gambatte/speedchange/speedchange_ly44_m3_m3stat_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_m3stat_scx1_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_nop_m3stat_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_nop_m3stat_scx1_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_nopx2_m3stat_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_nopx2_m3stat_scx1_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_nopx3_m3stat_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_nopx3_m3stat_scx1_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_nopx4_m3stat_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_nopx4_m3stat_scx1_1_cgb04c_outC3.gbc::cgb` | FAIL | expected top-left glyphs C3; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_stat_1_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly44_m3_stat_2_cgb04c_outC2.gbc::cgb` | FAIL | expected top-left glyphs C2; glyph pixel diff=5 |
| `gambatte/speedchange/speedchange_ly44_m3_stat_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_ly97_ly_cgb04c_out8C.gbc::cgb` | FAIL | expected top-left glyphs 8C; glyph pixel diff=23 |
| `gambatte/speedchange/speedchange_ly97_stat_cgb04c_outC0.gbc::cgb` | FAIL | expected top-left glyphs C0; glyph pixel diff=10 |
| `gambatte/speedchange/speedchange_tima00_1a_cgb04c_out80.gbc::cgb` | FAIL | expected top-left glyphs 80; glyph pixel diff=38 |
| `gambatte/speedchange/speedchange_tima00_1b_cgb04c_out81.gbc::cgb` | FAIL | expected top-left glyphs 81; glyph pixel diff=37 |
| `gambatte/speedchange/speedchange_tima00_2a_cgb04c_out81.gbc::cgb` | FAIL | expected top-left glyphs 81; glyph pixel diff=11 |
| `gambatte/speedchange/speedchange_tima00_2b_cgb04c_out82.gbc::cgb` | FAIL | expected top-left glyphs 82; glyph pixel diff=11 |
| `gambatte/sprites/10spritesPrLine_10xposA6_m0irq_2_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/sprites/10spritesPrLine_10xposA6_m0irq_2_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/sprites/10spritesPrLine_10xposA7_m0irq_2_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/sprites/10spritesPrLine_10xposA7_m0irq_2_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/sprites/10spritesPrLine_10xposA7_m3stat_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/10spritesPrLine_10xposA7_m3stat_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/10spritesPrLine_1xpos0_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesPrLine_1xpos0_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesPrLine_1xposA8_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesPrLine_1xposA8_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesPrLine_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesPrLine_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesPrLine_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_1xposa0_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_1xposa1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_1xposa2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_1xposa3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_1xposa4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_1xposa5_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_1xposa6_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_1xposa7_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_2overlap1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_2overlap2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_2overlap3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_2overlap4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_2overlap5_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_2overlap6_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_2overlap7_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_2overlap8_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_2xposa2overlap8_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_2xposa7overlap8_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/10spritesprline_3overlap8_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_1sprite8pBgCover_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_1sprite8pBgPrior_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_offset7_m3stat_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_offset7_m3stat_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_xpos01_scx4_m3stat_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_xpos01_scx4_m3stat_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_xpos07_m3stat_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_xpos07_m3stat_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_xpos09_scx4_m3stat_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/1spritesPrLine_xpos09_scx4_m3stat_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/2spritesPrLine_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/3spritesPrLine_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/3spritesPrLine_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/3spritesPrLine_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/4spritesPrLine_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/4spritesPrLine_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/4spritesPrLine_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/5spritesPrLine_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/6spritesPrLine_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/7spritesPrLine_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/7spritesPrLine_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/7spritesPrLine_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/8spritesPrLine_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/8spritesPrLine_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/8spritesPrLine_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/9spritesPrLine_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/enable/late_disable_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/enable/late_disable_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/enable/late_disable_ds_3_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_disable_1_dmg08_out0.gb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/late_disable_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange2_sp00_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange2_sp01_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange2_sp01_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange2_sp01_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange2_sp02_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange2_sp02_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange2_sp39_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange2_sp39_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange2_sp39_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_2_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_3_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_3_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_sp00_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_sp01_2_dmg08_out0_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_sp01_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_sp02_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_sp02_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_sp39_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_sp39_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_sp39_2_dmg08_out0_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/late_sizechange_sp39_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/mix_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/mix_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/mix_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_late_scx4_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space10_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space11_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space12_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space13_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space5_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space6_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space7_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space8_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr10space9_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_nr1space4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_scx1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_scx2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_scx3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_scx4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_scx4_nr1space4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_wx3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_wx4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_wx5_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_wx6_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/10spritesPrLine_wx7_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/1pos8_8pos9_wx08_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap1_offset4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap1_offset5_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap2_offset4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap3_offset4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap4_offset4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap5_offset4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap6_offset4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap7_offset4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap8_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/2overlap8_offset4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap2_scx1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap2_scx2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap2_scx3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap2_scx4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap3_scx1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap3_scx2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap3_scx3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap3_scx4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap4_scx1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap4_scx2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap4_scx3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap5_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap5_rev_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap5_scx1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap5_scx2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap5_scx3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap6_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap6_rev_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap6_scx1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap6_scx2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap6_scx3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap7_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap7_rev_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap7_scx1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap7_scx2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap7_scx3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap7_scx4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap8_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap8_scx1_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap8_scx2_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap8_scx3_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/3overlap8_scx4_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/9pos8_wx08_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/9pos8_wx09_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/9pos8_wx0A_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/9pos8_wx0B_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/9pos8_wx0C_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/9pos8_wx0D_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/space/9pos8_wx0E_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_disable_spx18_1_dmg08_out0.gb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_disable_spx19_1_dmg08_out0.gb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_disable_spx1A_1_dmg08_out0.gb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_disable_spx1B_1_dmg08_out0.gb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_enable_spx18_1_dmg08_out3.gb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_enable_spx19_1_dmg08_out3.gb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_enable_spx1A_1_dmg08_out3.gb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_enable_spx1B_1_dmg08_out3.gb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_late_disable_spx18_1_dmg08_out0.gb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_late_disable_spx19_1_dmg08_out0.gb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_late_disable_spx1A_1_dmg08_out0.gb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/sprites/sprite_late_late_disable_spx1B_1_dmg08_out0.gb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/tima/tc00_irq_late_retrigger_2_dmg08_outE4_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=18 |
| `gambatte/tima/tc00_irq_late_retrigger_3_dmg08_cgb04c_outE0.gbc` | FAIL | expected top-left glyphs E0; glyph pixel diff=18 |
| `gambatte/tima/tc00_irq_late_retrigger_3_dmg08_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=18 |
| `gambatte/tima/tc00_irq_late_retrigger_ds_2_cgb04c_outE0.gbc::cgb` | FAIL | expected top-left glyphs E0; glyph pixel diff=18 |
| `gambatte/tima/tc00_start_2_cgb04c_outF1.gbc::cgb` | FAIL | expected top-left glyphs F1; glyph pixel diff=27 |
| `gambatte/tima/tc01_late_tima_tma_2_dmg08_cgb04c_outF1.gbc` | FAIL | expected top-left glyphs F1; glyph pixel diff=22 |
| `gambatte/tima/tc01_late_tima_tma_2_dmg08_cgb04c_outF1.gbc::cgb` | FAIL | expected top-left glyphs F1; glyph pixel diff=22 |
| `gambatte/tima/tc01_late_tma_1_dmg08_cgb04c_out11.gbc` | FAIL | expected top-left glyphs 11; glyph pixel diff=22 |
| `gambatte/tima/tc01_late_tma_1_dmg08_cgb04c_out11.gbc::cgb` | FAIL | expected top-left glyphs 11; glyph pixel diff=22 |
| `gambatte/vram_m3/10spritesprline_postread_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/vram_m3/10spritesprline_postread_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/vram_m3/postread_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/vram_m3/postread_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/vram_m3/postread_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/vram_m3/postread_scx5_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/vram_m3/preread_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/vram_m3/preread_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/vram_m3/preread_lcdoffset2_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/vram_m3/prewrite_ds_1_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/vram_m3/prewrite_lcdoffset2_1_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/vram_m3/vramw_m3start_1_dmg08_cgb04c_out1.gbc` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/vram_m3/vramw_m3start_1_dmg08_cgb04c_out1.gbc::cgb` | FAIL | expected top-left glyphs 1; glyph pixel diff=27 |
| `gambatte/vramw_m3end/vramw_m3end_scx3_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=19 |
| `gambatte/vramw_m3end/vramw_m3end_scx3_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=19 |
| `gambatte/vramw_m3end/vramw_m3end_scx3_5_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/vramw_m3end/vramw_m3end_scx3_5_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_enable_afterVblank_4_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_enable_afterVblank_5_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_enable_afterVblank_5_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_scx_late_wy_FFto4_ly4_wx00_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_scx_late_wy_FFto4_ly4_wx00_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_scx_late_wy_FFto4_ly4_wx00_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_scx_late_wy_FFto4_ly4_wx20_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_scx_late_wy_FFto4_ly4_wx20_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_scx_late_wy_FFto4_ly4_wx20_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wx_late_wy_FFto2_ly2_2_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wx_late_wy_FFto2_ly2_2_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_10to0_ly1_2_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_10to0_ly1_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_10to0_ly1_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_10to1_ly1_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_10to1_ly1_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_10to1_ly1_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_1toFF_2_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_1toFF_ds_lcdoffset1_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_2toFF_2_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto0_ly0_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto0_ly0_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto0_ly0_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto0_ly2_2_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto0_ly2_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto0_ly2_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto0_ly2_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto1_ly2_2_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto1_ly2_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto1_ly2_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_scx2_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_scx2_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_scx2_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_scx3_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_scx3_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_scx3_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_scx5_2_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_scx5_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_scx5_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_scx5_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_wx00_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_wx00_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_wx00_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_wx0f_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_wx0f_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/arg/late_wy_FFto2_ly2_wx0f_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_disable_0_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_0_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_1_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx00_wx0f_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx00_wx10_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx00_wx11_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx00_wx12_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx03_wx0f_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx03_wx0f_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx03_wx10_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx03_wx10_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx03_wx11_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx03_wx11_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx03_wx12_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx03_wx12_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_early_scx03_wx12_2_dmg08_out0_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx00_wx0f_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx00_wx10_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx0f_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx0f_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx0f_2_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx10_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx10_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx10_2_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx11_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx11_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx11_2_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx12_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_late_scx03_wx12_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_scx2_1_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_disable_scx2_2_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_disable_scx2_2_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_disable_scx3_0_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_scx3_0_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_scx3_1_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_scx5_0_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_scx5_0_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_scx5_1_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_scx5_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_disable_spx10_wx0f_2_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_disable_spx10_wx0f_2_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_disable_wx0f_0_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_wx0f_0_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_disable_wx0f_1_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_enable_afterVblank_2_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_enable_afterVblank_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_enable_afterVblank_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_enable_afterVblank_ds_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_enable_afterVblank_ds_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_enable_afterVblank_lcdoffset1_2_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_reenable_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_reenable_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_reenable_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_reenable_scx2_2_dmg08_out3_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_reenable_scx3_2_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_reenable_scx3_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_reenable_scx3_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_reenable_scx5_2_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_reenable_scx5_3_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_reenable_scx5_3_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_reenable_scx5_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_reenable_wx0f_2_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_reenable_wx0f_2_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_scx_late_disable_0_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_scx_late_disable_0_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_scx_late_disable_1_dmg08_out3_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_wx_scx2_2_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_wx_scx2_2_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_wx_scx3_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_scx3_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_scx3_2_dmg08_out0_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_scx5_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_scx5_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_scx5_ds_2_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/late_wx_wx03_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_wx03_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_wx0f_1_dmg08_cgb04c_out0.gbc` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wx_wx0f_1_dmg08_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wy_ds_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wy_ds_lcdoffset1_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/late_wy_lcdoffset1_1_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=10 |
| `gambatte/window/m2int_wx03_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wx03_scx2_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wx03_scx2_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wx03_scx5_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wx07_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wx07_scx2_m3stat_1_dmg08_cgb04c_out3.gbc` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wx07_scx2_m3stat_1_dmg08_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wx07_scx5_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wx0C_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wx57_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_firstline_m3stat_2_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_m3stat_2_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_oambusyread_2_dmg08_out5_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=11 |
| `gambatte/window/m2int_wxA6_scx2_m3stat_2_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_scx2_m3stat_3_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_scx3_m3stat_2_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_scx3_m3stat_3_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_scx5_m3stat_2_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_scx5_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_spxA7_m0irq_2_dmg08_cgb04c_out2.gbc` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/window/m2int_wxA6_spxA7_m0irq_2_dmg08_cgb04c_out2.gbc::cgb` | FAIL | expected top-left glyphs 2; glyph pixel diff=9 |
| `gambatte/window/m2int_wxA6_spxA7_m3stat_2_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_spxA7_m3stat_4_dmg08_out0_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/m2int_wxA6_vrambusyread_2_dmg08_out5_cgb04c_out0.gbc::cgb` | FAIL | expected top-left glyphs 0; glyph pixel diff=11 |
| `gambatte/window/m2int_wxDefault_m3stat_ds_1_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gambatte/window/on_screen/late_wx_ds_1.gbc::cgb` | FAIL | pixel diff=160/23040 vs res/tests/gambatte/window/on_screen/late_wx_ds_1.png |
| `gambatte/window/on_screen/weon_wx18_weoff_weon_wx80.gbc` | FAIL | pixel diff=4916/23040 vs res/tests/gambatte/window/on_screen/weon_wx18_weoff_weon_wx80_dmg08.png |
| `gambatte/window/on_screen/weon_wx18_weoff_weon_wx80.gbc::cgb` | FAIL | pixel diff=4916/23040 vs res/tests/gambatte/window/on_screen/weon_wx18_weoff_weon_wx80_cgb04c.png |
| `gambatte/window/on_screen/wx17_weoff_wxA5_weon.gbc` | FAIL | pixel diff=960/23040 vs res/tests/gambatte/window/on_screen/wx17_weoff_wxA5_weon_dmg08.png |
| `gambatte/window/on_screen/wxA6_3.gbc` | FAIL | pixel diff=10780/23040 vs res/tests/gambatte/window/on_screen/wxA6_3_dmg08.png |
| `gambatte/window/on_screen/wxA6_late_we_reenable_1.gbc` | FAIL | pixel diff=14624/23040 vs res/tests/gambatte/window/on_screen/wxA6_late_we_reenable_1_dmg08.png |
| `gambatte/window/on_screen/wxA6_late_we_reenable_2.gbc` | FAIL | pixel diff=14672/23040 vs res/tests/gambatte/window/on_screen/wxA6_late_we_reenable_2_dmg08.png |
| `gambatte/window/on_screen/wxA6_late_we_reenable_3.gbc` | FAIL | pixel diff=14516/23040 vs res/tests/gambatte/window/on_screen/wxA6_late_we_reenable_3_dmg08.png |
| `gambatte/window/on_screen/wxA6_scx7.gbc` | FAIL | pixel diff=10992/23040 vs res/tests/gambatte/window/on_screen/wxA6_scx7_dmg08.png |
| `gambatte/window/on_screen/wxA6_weoff_at_xposA6.gbc` | FAIL | pixel diff=8832/23040 vs res/tests/gambatte/window/on_screen/wxA6_weoff_at_xposA6_dmg08.png |
| `gambatte/window/on_screen/wxA6_wy00.gbc` | FAIL | pixel diff=21816/23040 vs res/tests/gambatte/window/on_screen/wxA6_wy00_dmg08.png |
| `gambatte/window/on_screen/wxA6_wy01.gbc` | FAIL | pixel diff=21657/23040 vs res/tests/gambatte/window/on_screen/wxA6_wy01_dmg08.png |
| `gambatte/window/on_screen/wxA6_wy01_weoff_ly02.gbc` | FAIL | pixel diff=160/23040 vs res/tests/gambatte/window/on_screen/wxA6_wy01_weoff_ly02_dmg08.png |
| `gambatte/window/on_screen/wxA6_wy01_weoff_ly02_weon_ly60.gbc` | FAIL | pixel diff=7303/23040 vs res/tests/gambatte/window/on_screen/wxA6_wy01_weoff_ly02_weon_ly60_dmg08.png |
| `gambatte/window/on_screen/wxA6_wy01_wxA5_ly02.gbc` | FAIL | pixel diff=160/23040 vs res/tests/gambatte/window/on_screen/wxA6_wy01_wxA5_ly02_dmg08.png |
| `gambatte/window/on_screen/wxA6_wy01_wxA7_ly02.gbc` | FAIL | pixel diff=160/23040 vs res/tests/gambatte/window/on_screen/wxA6_wy01_wxA7_ly02_dmg08.png |
| `gambatte/window/on_screen/wxA6_wy8F.gbc` | FAIL | pixel diff=160/23040 vs res/tests/gambatte/window/on_screen/wxA6_wy8F_dmg08.png |
| `gambatte/window/reenable_ds_cgb04c_out3.gbc::cgb` | FAIL | expected top-left glyphs 3; glyph pixel diff=10 |
| `gbmicrotest/dma_timing_a.gb` | FAIL | FF80=80 FF81=81 FF82=FF |
| `gbmicrotest/halt_op_dupe_delay.gb` | FAIL | FF80=01 FF81=55 FF82=FF |
| `gbmicrotest/hblank_int_scx0_if_b.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx0_if_c.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx0_if_d.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx1_if_b.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx1_if_c.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx1_if_d.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx1_nops_a.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx1_nops_b.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx2_if_b.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx2_if_c.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx2_if_d.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx2_nops_a.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx2_nops_b.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx3_if_b.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx3_if_c.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx3_if_d.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx3_nops_a.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx3_nops_b.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx4.gb` | FAIL | FF80=2D FF81=2E FF82=FF |
| `gbmicrotest/hblank_int_scx4_if_b.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx4_if_c.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx4_if_d.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx4_nops_a.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx4_nops_b.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx5.gb` | FAIL | FF80=2D FF81=2E FF82=FF |
| `gbmicrotest/hblank_int_scx5_if_b.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx5_if_c.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx5_if_d.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx5_nops_a.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx5_nops_b.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx6.gb` | FAIL | FF80=2D FF81=2E FF82=FF |
| `gbmicrotest/hblank_int_scx6_if_b.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx6_if_c.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx6_if_d.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx6_nops_a.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx6_nops_b.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx7.gb` | FAIL | FF80=2D FF81=2F FF82=FF |
| `gbmicrotest/hblank_int_scx7_if_b.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx7_if_c.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx7_if_d.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx7_nops_a.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_int_scx7_nops_b.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/hblank_scx2_if_a.gb` | FAIL | FF80=E0 FF81=E2 FF82=FF |
| `gbmicrotest/int_hblank_halt_scx1.gb` | FAIL | FF80=63 FF81=62 FF82=FF |
| `gbmicrotest/int_hblank_halt_scx2.gb` | FAIL | FF80=63 FF81=62 FF82=FF |
| `gbmicrotest/int_hblank_halt_scx5.gb` | FAIL | FF80=64 FF81=63 FF82=FF |
| `gbmicrotest/int_hblank_halt_scx6.gb` | FAIL | FF80=64 FF81=63 FF82=FF |
| `gbmicrotest/int_lyc_halt.gb` | FAIL | FF80=9A FF81=99 FF82=FF |
| `gbmicrotest/int_oam_halt.gb` | FAIL | FF80=23 FF81=94 FF82=FF |
| `gbmicrotest/int_oam_incs.gb` | FAIL | FF80=91 FF81=6F FF82=FF |
| `gbmicrotest/int_oam_nops.gb` | FAIL | FF80=23 FF81=93 FF82=FF |
| `gbmicrotest/int_vblank1_halt.gb` | FAIL | FF80=43 FF81=42 FF82=FF |
| `gbmicrotest/int_vblank2_halt.gb` | FAIL | FF80=46 FF81=45 FF82=FF |
| `gbmicrotest/lcdon_halt_to_vblank_int_a.gb` | FAIL | FF80=01 FF81=00 FF82=FF |
| `gbmicrotest/lcdon_to_if_oam_a.gb` | FAIL | FF80=E2 FF81=E0 FF82=FF |
| `gbmicrotest/lcdon_to_ly1_b.gb` | FAIL | FF80=00 FF81=01 FF82=FF |
| `gbmicrotest/lcdon_to_ly2_b.gb` | FAIL | FF80=01 FF81=02 FF82=FF |
| `gbmicrotest/lcdon_to_ly3_b.gb` | FAIL | FF80=02 FF81=03 FF82=FF |
| `gbmicrotest/lcdon_to_oam_int_l0.gb` | FAIL | FF80=91 FF81=6F FF82=FF |
| `gbmicrotest/lcdon_to_oam_int_l1.gb` | FAIL | FF80=00 FF81=64 FF82=FF |
| `gbmicrotest/lcdon_to_oam_int_l2.gb` | FAIL | FF80=00 FF81=64 FF82=FF |
| `gbmicrotest/lcdon_to_oam_unlock_a.gb` | FAIL | FF80=FF FF81=27 FF82=FF |
| `gbmicrotest/lcdon_to_stat1_d.gb` | FAIL | FF80=85 FF81=84 FF82=FF |
| `gbmicrotest/lcdon_to_stat2_a.gb` | FAIL | FF80=84 FF81=80 FF82=FF |
| `gbmicrotest/lcdon_to_stat3_a.gb` | FAIL | FF80=86 FF81=84 FF82=FF |
| `gbmicrotest/line_144_oam_int_b.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/line_144_oam_int_c.gb` | FAIL | FF80=E0 FF81=FF FF82=FF |
| `gbmicrotest/line_144_oam_int_d.gb` | FAIL | FF80=E1 FF81=FF FF82=FF |
| `gbmicrotest/line_153_ly_b.gb` | FAIL | FF80=98 FF81=99 FF82=FF |
| `gbmicrotest/line_153_ly_c.gb` | FAIL | FF80=99 FF81=00 FF82=FF |
| `gbmicrotest/line_153_ly_d.gb` | FAIL | FF80=99 FF81=00 FF82=FF |
| `gbmicrotest/line_153_ly_f.gb` | FAIL | FF80=00 FF81=01 FF82=FF |
| `gbmicrotest/line_153_lyc0_stat_timing_f.gb` | FAIL | FF80=C5 FF81=C4 FF82=FF |
| `gbmicrotest/line_153_lyc0_stat_timing_m.gb` | FAIL | FF80=C4 FF81=C0 FF82=FF |
| `gbmicrotest/line_153_lyc153_stat_timing_c.gb` | FAIL | FF80=C5 FF81=C1 FF82=FF |
| `gbmicrotest/line_153_lyc153_stat_timing_e.gb` | FAIL | FF80=C1 FF81=C0 FF82=FF |
| `gbmicrotest/line_153_lyc_c.gb` | FAIL | FF80=85 FF81=81 FF82=FF |
| `gbmicrotest/line_153_lyc_int_a.gb` | FAIL | FF80=99 FF81=FF FF82=FF |
| `gbmicrotest/line_65_ly.gb` | FAIL | FF80=38 FF81=40 FF82=FF |
| `gbmicrotest/lyc1_int_halt_a.gb` | FAIL | FF80=02 FF81=01 FF82=FF |
| `gbmicrotest/lyc1_write_timing_d.gb` | FAIL | FF80=01 FF81=FF FF82=FF |
| `gbmicrotest/lyc2_int_halt_a.gb` | FAIL | FF80=02 FF81=01 FF82=FF |
| `gbmicrotest/lyc_int_halt_a.gb` | FAIL | FF80=05 FF81=04 FF82=FF |
| `gbmicrotest/oam_int_halt_a.gb` | FAIL | FF80=00 FF81=01 FF82=FF |
| `gbmicrotest/oam_int_halt_b.gb` | FAIL | FF80=00 FF81=02 FF82=FF |
| `gbmicrotest/oam_int_if_edge_b.gb` | FAIL | FF80=E0 FF81=E2 FF82=FF |
| `gbmicrotest/oam_int_if_edge_d.gb` | FAIL | FF80=E2 FF81=E0 FF82=FF |
| `gbmicrotest/oam_int_if_level_d.gb` | FAIL | FF80=E2 FF81=E0 FF82=FF |
| `gbmicrotest/oam_int_inc_sled.gb` | FAIL | FF80=00 FF81=64 FF82=FF |
| `gbmicrotest/oam_int_nops_a.gb` | FAIL | FF80=00 FF81=01 FF82=FF |
| `gbmicrotest/oam_int_nops_b.gb` | FAIL | FF80=00 FF81=02 FF82=FF |
| `gbmicrotest/oam_read_l0_a.gb` | FAIL | FF80=FF FF81=F0 FF82=FF |
| `gbmicrotest/oam_read_l1_a.gb` | FAIL | FF80=F0 FF81=FF FF82=FF |
| `gbmicrotest/oam_read_l1_f.gb` | FAIL | FF80=F0 FF81=FF FF82=FF |
| `gbmicrotest/oam_write_l0_a.gb` | FAIL | FF80=F0 FF81=91 FF82=FF |
| `gbmicrotest/oam_write_l1_c.gb` | FAIL | FF80=F0 FF81=91 FF82=FF |
| `gbmicrotest/poweron_dma_000.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_ly_000.gb` | FAIL | FF80=91 FF81=00 FF82=FF |
| `gbmicrotest/poweron_ly_119.gb` | FAIL | FF80=92 FF81=00 FF82=FF |
| `gbmicrotest/poweron_ly_120.gb` | FAIL | FF80=92 FF81=01 FF82=FF |
| `gbmicrotest/poweron_ly_233.gb` | FAIL | FF80=93 FF81=01 FF82=FF |
| `gbmicrotest/poweron_ly_234.gb` | FAIL | FF80=93 FF81=02 FF82=FF |
| `gbmicrotest/poweron_oam_006.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_oam_069.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_oam_120.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_oam_121.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_oam_183.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_oam_234.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_oam_235.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_stat_000.gb` | FAIL | FF80=81 FF81=85 FF82=FF |
| `gbmicrotest/poweron_stat_005.gb` | FAIL | FF80=81 FF81=85 FF82=FF |
| `gbmicrotest/poweron_stat_006.gb` | FAIL | FF80=81 FF81=84 FF82=FF |
| `gbmicrotest/poweron_stat_007.gb` | FAIL | FF80=81 FF81=86 FF82=FF |
| `gbmicrotest/poweron_stat_026.gb` | FAIL | FF80=81 FF81=86 FF82=FF |
| `gbmicrotest/poweron_stat_027.gb` | FAIL | FF80=81 FF81=87 FF82=FF |
| `gbmicrotest/poweron_stat_069.gb` | FAIL | FF80=81 FF81=87 FF82=FF |
| `gbmicrotest/poweron_stat_070.gb` | FAIL | FF80=81 FF81=84 FF82=FF |
| `gbmicrotest/poweron_stat_119.gb` | FAIL | FF80=81 FF81=84 FF82=FF |
| `gbmicrotest/poweron_stat_120.gb` | FAIL | FF80=81 FF81=80 FF82=FF |
| `gbmicrotest/poweron_stat_121.gb` | FAIL | FF80=81 FF81=82 FF82=FF |
| `gbmicrotest/poweron_stat_140.gb` | FAIL | FF80=81 FF81=82 FF82=FF |
| `gbmicrotest/poweron_stat_141.gb` | FAIL | FF80=81 FF81=83 FF82=FF |
| `gbmicrotest/poweron_stat_183.gb` | FAIL | FF80=81 FF81=83 FF82=FF |
| `gbmicrotest/poweron_stat_184.gb` | FAIL | FF80=81 FF81=80 FF82=FF |
| `gbmicrotest/poweron_stat_234.gb` | FAIL | FF80=81 FF81=80 FF82=FF |
| `gbmicrotest/poweron_stat_235.gb` | FAIL | FF80=81 FF81=82 FF82=FF |
| `gbmicrotest/poweron_vram_026.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_vram_069.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_vram_140.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/poweron_vram_183.gb` | FAIL | FF80=00 FF81=FF FF82=FF |
| `gbmicrotest/ppu_sprite0_scx1_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/ppu_sprite0_scx2_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/ppu_sprite0_scx3_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/ppu_sprite0_scx5_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/ppu_sprite0_scx6_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/ppu_sprite0_scx7_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/sprite4_0_a.gb` | FAIL | FF80=80 FF81=83 FF82=FF |
| `gbmicrotest/sprite4_1_a.gb` | FAIL | FF80=80 FF81=83 FF82=FF |
| `gbmicrotest/stat_write_glitch_l0_a.gb` | FAIL | FF80=E0 FF81=E2 FF82=FF |
| `gbmicrotest/stat_write_glitch_l0_b.gb` | FAIL | FF80=E0 FF81=E2 FF82=FF |
| `gbmicrotest/stat_write_glitch_l143_b.gb` | FAIL | FF80=E0 FF81=E2 FF82=FF |
| `gbmicrotest/stat_write_glitch_l143_c.gb` | FAIL | FF80=E0 FF81=E2 FF82=FF |
| `gbmicrotest/stat_write_glitch_l143_d.gb` | FAIL | FF80=E1 FF81=E3 FF82=FF |
| `gbmicrotest/stat_write_glitch_l154_a.gb` | FAIL | FF80=E0 FF81=E2 FF82=FF |
| `gbmicrotest/stat_write_glitch_l154_b.gb` | FAIL | FF80=E0 FF81=E2 FF82=FF |
| `gbmicrotest/stat_write_glitch_l154_d.gb` | FAIL | FF80=E1 FF81=E0 FF82=FF |
| `gbmicrotest/stat_write_glitch_l1_b.gb` | FAIL | FF80=E0 FF81=E2 FF82=FF |
| `gbmicrotest/stat_write_glitch_l1_c.gb` | FAIL | FF80=E0 FF81=E2 FF82=FF |
| `gbmicrotest/vblank2_int_halt_a.gb` | FAIL | FF80=02 FF81=01 FF82=FF |
| `gbmicrotest/vblank_int_halt_a.gb` | FAIL | FF80=02 FF81=01 FF82=FF |
| `gbmicrotest/vram_read_l1_b.gb` | FAIL | FF80=F0 FF81=FF FF82=FF |
| `gbmicrotest/win0_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win0_scx3_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win10_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win10_scx3_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win11_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win12_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win13_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win14_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win15_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win1_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win2_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win3_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win4_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win5_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win6_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win7_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win8_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `gbmicrotest/win9_b.gb` | FAIL | FF80=83 FF81=80 FF82=FF |
| `little-things-gb/firstwhite.gb` | FAIL | pixel diff=1225/23040 vs res/tests/little-things-gb/firstwhite-dmg-cgb.png |
| `little-things-gb/tellinglys.gb` | FAIL | pixel diff=7604/23040 vs res/tests/little-things-gb/tellinglys-dmg.png |
| `mealybug-tearoom-tests/ppu/m2_win_en_toggle.gb` | FAIL | pixel diff=9030/23040 vs res/tests/mealybug-tearoom-tests/ppu/m2_win_en_toggle_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m2_win_en_toggle.gb::cgb` | FAIL | pixel diff=17362/23040 vs res/tests/mealybug-tearoom-tests/ppu/m2_win_en_toggle_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_bgp_change.gb` | FAIL | pixel diff=2218/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_bgp_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_bgp_change.gb::cgb` | FAIL | pixel diff=22280/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_bgp_change_sprites_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_bgp_change_sprites.gb` | FAIL | pixel diff=3778/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_bgp_change_sprites_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_bgp_change_sprites.gb::cgb` | FAIL | pixel diff=23040/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_bgp_change_sprites_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_bg_en_change.gb` | FAIL | pixel diff=1629/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_bg_en_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_bg_en_change.gb::cgb` | FAIL | pixel diff=22456/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_bg_en_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_bg_en_change2.gb::cgb` | FAIL | pixel diff=22901/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_bg_en_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_bg_map_change.gb` | FAIL | pixel diff=978/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_bg_map_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_bg_map_change.gb::cgb` | FAIL | pixel diff=18520/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_bg_map_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_bg_map_change2.gb::cgb` | FAIL | pixel diff=22878/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_bg_map_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_obj_en_change.gb` | FAIL | pixel diff=146/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_obj_en_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_obj_en_change.gb::cgb` | FAIL | pixel diff=21550/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_obj_en_change_variant_cgb_d.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_obj_en_change_variant.gb` | FAIL | pixel diff=814/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_obj_en_change_variant_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_obj_en_change_variant.gb::cgb` | FAIL | pixel diff=22166/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_obj_en_change_variant_cgb_d.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_obj_size_change.gb` | FAIL | pixel diff=328/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_obj_size_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_obj_size_change.gb::cgb` | FAIL | pixel diff=22375/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_obj_size_change_scx_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_obj_size_change_scx.gb` | FAIL | pixel diff=350/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_obj_size_change_scx_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_obj_size_change_scx.gb::cgb` | FAIL | pixel diff=22990/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_obj_size_change_scx_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_change.gb` | FAIL | pixel diff=2054/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_change.gb::cgb` | FAIL | pixel diff=22088/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_change2.gb::cgb` | FAIL | pixel diff=22784/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_win_change.gb` | FAIL | pixel diff=1742/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_win_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_win_change.gb::cgb` | FAIL | pixel diff=21986/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_win_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_win_change2.gb::cgb` | FAIL | pixel diff=22990/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_win_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_win_en_change_multiple.gb` | FAIL | pixel diff=8334/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_win_en_change_multiple_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_win_en_change_multiple.gb::cgb` | FAIL | pixel diff=21744/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_win_en_change_multiple_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_win_en_change_multiple_wx.gb` | FAIL | pixel diff=4394/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_win_en_change_multiple_wx_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_win_map_change.gb` | FAIL | pixel diff=1028/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_win_map_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_win_map_change.gb::cgb` | FAIL | pixel diff=18280/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_win_map_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_lcdc_win_map_change2.gb::cgb` | FAIL | pixel diff=22993/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_lcdc_win_map_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_obp0_change.gb` | FAIL | pixel diff=432/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_obp0_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_obp0_change.gb::cgb` | FAIL | pixel diff=22750/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_obp0_change_cgb_d.png |
| `mealybug-tearoom-tests/ppu/m3_scx_high_5_bits.gb` | FAIL | pixel diff=84/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_scx_high_5_bits_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_scx_high_5_bits.gb::cgb` | FAIL | pixel diff=22592/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_scx_high_5_bits_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_scx_high_5_bits_change2.gb::cgb` | FAIL | pixel diff=22985/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_scx_high_5_bits_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_scx_low_3_bits.gb` | FAIL | pixel diff=540/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_scx_low_3_bits_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_scx_low_3_bits.gb::cgb` | FAIL | pixel diff=22500/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_scx_low_3_bits_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_scy_change.gb` | FAIL | pixel diff=10273/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_scy_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_scy_change.gb::cgb` | FAIL | pixel diff=21910/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_scy_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_scy_change2.gb::cgb` | FAIL | pixel diff=22994/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_scy_change2_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_window_timing.gb` | FAIL | pixel diff=416/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_window_timing_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_window_timing.gb::cgb` | FAIL | pixel diff=22341/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_window_timing_wx_0_cgb_d.png |
| `mealybug-tearoom-tests/ppu/m3_window_timing_wx_0.gb` | FAIL | pixel diff=936/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_window_timing_wx_0_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_window_timing_wx_0.gb::cgb` | FAIL | pixel diff=22106/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_window_timing_wx_0_cgb_d.png |
| `mealybug-tearoom-tests/ppu/m3_wx_4_change.gb` | FAIL | pixel diff=229/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_wx_4_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_wx_4_change.gb::cgb` | FAIL | pixel diff=23038/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_wx_4_change_sprites_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_wx_4_change_sprites.gb` | FAIL | pixel diff=10/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_wx_4_change_sprites_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_wx_4_change_sprites.gb::cgb` | FAIL | pixel diff=23031/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_wx_4_change_sprites_cgb_c.png |
| `mealybug-tearoom-tests/ppu/m3_wx_5_change.gb` | FAIL | pixel diff=638/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_wx_5_change_dmg_blob.png |
| `mealybug-tearoom-tests/ppu/m3_wx_6_change.gb` | FAIL | pixel diff=13799/23040 vs res/tests/mealybug-tearoom-tests/ppu/m3_wx_6_change_dmg_blob.png |
| `mooneye-test-suite/acceptance/boot_div-dmg0.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/boot_hwio-dmg0.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/boot_hwio-dmgABCmgb.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/boot_regs-dmg0.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/intr_2_0_timing.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/intr_2_0_timing.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/intr_2_mode0_timing.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/intr_2_mode0_timing.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/intr_2_mode0_timing_sprites.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/intr_2_mode0_timing_sprites.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/intr_2_mode3_timing.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/intr_2_mode3_timing.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/intr_2_oam_ok_timing.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/intr_2_oam_ok_timing.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/stat_lyc_onoff.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/acceptance/ppu/stat_lyc_onoff.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/manual-only/sprite_priority.gb::cgb` | FAIL | pixel diff=23040/23040 vs res/tests/mooneye-test-suite/manual-only/sprite_priority-cgb.png |
| `mooneye-test-suite/misc/bits/unused_hwio-C.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/misc/bits/unused_hwio-C.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/misc/boot_div-cgb0.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/misc/boot_div-cgbABCDE.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/misc/boot_hwio-C.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/misc/boot_hwio-C.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/misc/boot_regs-A.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/misc/ppu/vblank_stat_intr-C.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite/misc/ppu/vblank_stat_intr-C.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/hblank_ly_scx_timing-C.gb` | FAIL | BCDEHL=(4, 147, 66, 8, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/hblank_ly_scx_timing-C.gb::cgb` | FAIL | BCDEHL=(4, 147, 66, 8, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/hblank_ly_scx_timing_nops.gb` | FAIL | BCDEHL=(4, 149, 66, 8, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/hblank_ly_scx_timing_nops.gb::cgb` | FAIL | BCDEHL=(4, 149, 66, 8, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/hblank_ly_scx_timing_variant_nops.gb` | FAIL | BCDEHL=(10, 69, 66, 8, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/hblank_ly_scx_timing_variant_nops.gb::cgb` | FAIL | BCDEHL=(10, 69, 66, 8, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_0_timing.gb` | FAIL | BCDEHL=(72, 190, 1, 48, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_0_timing.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 48, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_scx4_timing_nops.gb` | FAIL | BCDEHL=(72, 190, 1, 60, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_scx4_timing_nops.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 60, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_scx8_timing_nops.gb` | FAIL | BCDEHL=(72, 190, 1, 60, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_scx8_timing_nops.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 60, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing.gb` | FAIL | BCDEHL=(72, 190, 1, 48, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 48, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites.gb` | FAIL | BCDEHL=(12, 201, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites.gb::cgb` | FAIL | BCDEHL=(12, 201, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites_nops.gb` | FAIL | BCDEHL=(16, 182, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites_nops.gb::cgb` | FAIL | BCDEHL=(16, 182, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites_scx1_nops.gb` | FAIL | BCDEHL=(16, 186, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites_scx1_nops.gb::cgb` | FAIL | BCDEHL=(16, 186, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites_scx2_nops.gb` | FAIL | BCDEHL=(16, 186, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites_scx2_nops.gb::cgb` | FAIL | BCDEHL=(16, 186, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites_scx3_nops.gb` | FAIL | BCDEHL=(16, 186, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites_scx3_nops.gb::cgb` | FAIL | BCDEHL=(16, 186, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites_scx4_nops.gb` | FAIL | BCDEHL=(16, 186, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode0_timing_sprites_scx4_nops.gb::cgb` | FAIL | BCDEHL=(16, 186, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode3_timing.gb` | FAIL | BCDEHL=(72, 190, 1, 48, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_mode3_timing.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 48, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_oam_ok_timing.gb` | FAIL | BCDEHL=(72, 190, 1, 48, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_oam_ok_timing.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 48, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_timing.gb` | FAIL | BCDEHL=(72, 190, 4, 253, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_2_timing.gb::cgb` | FAIL | BCDEHL=(72, 190, 4, 253, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/lcdon_mode_timing.gb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/lcdon_mode_timing.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly00_01_mode0_2.gb` | FAIL | BCDEHL=(72, 190, 1, 252, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly00_01_mode0_2.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 252, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly00_mode1_2-C.gb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly00_mode1_2-C.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly00_mode2_3.gb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly00_mode2_3.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly00_mode3_0.gb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly00_mode3_0.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly143_144_145.gb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly143_144_145.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly143_144_152_153.gb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly143_144_152_153.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly143_144_mode0_1.gb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly143_144_mode0_1.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly143_144_mode3_0.gb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly143_144_mode3_0.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc-C.gb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc-C.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_0-C.gb` | FAIL | BCDEHL=(72, 190, 5, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_0-C.gb::cgb` | FAIL | BCDEHL=(72, 190, 5, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_0_write-C.gb` | FAIL | BCDEHL=(72, 190, 2, 252, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_0_write-C.gb::cgb` | FAIL | BCDEHL=(72, 190, 2, 252, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_144-C.gb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_144-C.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_153-C.gb` | FAIL | BCDEHL=(72, 190, 5, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_153-C.gb::cgb` | FAIL | BCDEHL=(72, 190, 5, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_153_write-C.gb` | FAIL | BCDEHL=(72, 190, 2, 252, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_153_write-C.gb::cgb` | FAIL | BCDEHL=(72, 190, 2, 252, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_write-C.gb` | FAIL | BCDEHL=(72, 190, 2, 253, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_write-C.gb::cgb` | FAIL | BCDEHL=(72, 190, 2, 253, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_new_frame-C.gb` | FAIL | BCDEHL=(72, 190, 2, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_new_frame-C.gb::cgb` | FAIL | BCDEHL=(72, 190, 2, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/stat_write_if-C.gb` | FAIL | BCDEHL=(8, 211, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/stat_write_if-C.gb::cgb` | FAIL | BCDEHL=(8, 211, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/vblank_if_timing.gb` | FAIL | BCDEHL=(72, 190, 1, 124, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/vblank_if_timing.gb::cgb` | FAIL | BCDEHL=(72, 190, 1, 124, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/manual-only/sprite_priority.gb` | FAIL | pixel diff=335/23040 vs res/tests/mooneye-test-suite-wilbertpol/manual-only/sprite_priority-dmg.png |
| `mooneye-test-suite-wilbertpol/manual-only/sprite_priority.gb::cgb` | FAIL | pixel diff=22819/23040 vs res/tests/mooneye-test-suite-wilbertpol/manual-only/sprite_priority-cgb.png |
| `mooneye-test-suite-wilbertpol/misc/bits/unused_hwio-C.gb` | FAIL | BCDEHL=(1, 128, 66, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/misc/bits/unused_hwio-C.gb::cgb` | FAIL | BCDEHL=(1, 128, 66, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/misc/boot_hwio-C.gb` | FAIL | BCDEHL=(207, 203, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/misc/boot_hwio-C.gb::cgb` | FAIL | BCDEHL=(207, 203, 66, 240, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/misc/boot_regs-A.gb::cgb` | FAIL | BCDEHL=(72, 190, 2, 255, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/misc/gpu/vblank_stat_intr-C.gb` | FAIL | BCDEHL=(72, 190, 2, 60, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/misc/gpu/vblank_stat_intr-C.gb::cgb` | FAIL | BCDEHL=(72, 190, 2, 60, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/utils/dump_boot_hwio.gb` | FAIL | BCDEHL=(255, 12, 255, 128, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `mooneye-test-suite-wilbertpol/utils/dump_boot_hwio.gb::cgb` | FAIL | BCDEHL=(255, 12, 255, 128, 255, 64); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_align.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_align_cpu.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_duty.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_duty_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_extra_length_clocking-cgb0B.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_freq_change.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_freq_change_timing-A.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_freq_change_timing-cgb0BC.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_freq_change_timing-cgbDE.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_nrx2_glitch.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_nrx2_speed_change.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_restart.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_restart_nrx2_glitch.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_stop_div.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_stop_restart.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_sweep.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_sweep_restart.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_sweep_restart_2.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_volume.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_1/channel_1_volume_div.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_align.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_align_cpu.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_duty.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_duty_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_extra_length_clocking-cgb0B.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_freq_change.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_nrx2_glitch.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_nrx2_speed_change.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_restart.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_restart_nrx2_glitch.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_stop_div.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_stop_restart.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_volume.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_2/channel_2_volume_div.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_and_glitch.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_extra_length_clocking-cgb0.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_extra_length_clocking-cgbB.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_first_sample.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_freq_change_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_restart_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_restart_during_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_restart_stop_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_shift_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_shift_skip_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_3/channel_3_wave_ram_sync.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_4/channel_4_align.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_4/channel_4_delay.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_4/channel_4_equivalent_frequencies.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_4/channel_4_extra_length_clocking-cgb0B.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_4/channel_4_freq_change.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_4/channel_4_frequency_alignment.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_4/channel_4_lfsr_7_15.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_4/channel_4_lfsr_restart.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/channel_4/channel_4_lfsr_restart_fast.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/div_trigger_volume_10.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/div_write_trigger_10.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/div_write_trigger_10.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/apu/div_write_trigger_volume_10.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/dma/gbc_dma_cont.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/dma/gdma_addr_mask.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/dma/gdma_addr_mask.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/dma/hdma_lcd_off.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/dma/hdma_lcd_off.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/dma/hdma_mode0.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/dma/hdma_mode0.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/ppu/blocking_bgpi_increase.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/sgb/command_mlt_req.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/sgb/command_mlt_req.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/sgb/command_mlt_req_1_incrementing.gb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `same-suite/sgb/command_mlt_req_1_incrementing.gb::cgb` | FAIL | BCDEHL=(66, 66, 66, 66, 66, 66); expected=(3, 5, 8, 13, 21, 34) |
| `scribbltests/statcount/statcount-auto.gb` | FAIL | pixel diff=991/23040 vs res/tests/scribbltests/statcount/statcount_auto-cgb-dmg.png |
| `scribbltests/statcount/statcount.gb` | FAIL | pixel diff=609/23040 vs res/tests/scribbltests/statcount/statcount_auto-cgb-dmg.png |
| `strikethrough/strikethrough.gb` | FAIL | pixel diff=7/23040 vs res/tests/strikethrough/strikethrough-dmg.png |

## Unsupported and unverifiable executions

These executions target a different hardware model or do not expose a stable,
machine-readable result through the current output interfaces. They are kept
separate from objective failures.

| Suite | Result | Count | Reason |
|---|---|---:|---|
| age-test-roms | UNSUPPORTED_MODEL | 33 | Test name targets CGB, CGB compatibility mode, or double speed |
| blargg | UNSUPPORTED_MODEL | 14 | Bundled documentation identifies this as CGB-specific |
| dmg-acid2 | UNSUPPORTED_MODEL | 1 | CGB compatibility-mode colours come from the boot ROM's title-checksum palette, which GameLad does not emulate |
| gambatte | UNVERIFIABLE_NO_ORACLE | 52 | Upstream Gambatte runner defines no expected output for this ROM name |
| gbmicrotest | UNVERIFIABLE_NO_ORACLE | 31 | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| mealybug-tearoom-tests | UNSUPPORTED_MODEL | 7 | Only CGB expected screenshots are bundled |
| mealybug-tearoom-tests | UNSUPPORTED_MODEL | 2 | Test name targets CGB |
| mealybug-tearoom-tests | UNVERIFIABLE_NO_ORACLE | 1 | RTC utility has no upstream reference screenshot |
| mealybug-tearoom-tests | UNVERIFIABLE_NO_ORACLE | 1 | Upstream suite ships no reference image for any model |
| mooneye-test-suite | UNSUPPORTED_MODEL | 14 | Mooneye model suffix does not include DMG |
| mooneye-test-suite | UNSUPPORTED_MODEL | 7 | Test name targets MGB, SGB, or CGB |
| mooneye-test-suite | UNVERIFIABLE_NO_ORACLE | 2 | Boot ROM glitch/extraction utility is not meaningful in a post-boot run |
| mooneye-test-suite-wilbertpol | UNSUPPORTED_MODEL | 20 | Mooneye model suffix does not include DMG |
| mooneye-test-suite-wilbertpol | UNSUPPORTED_MODEL | 5 | Test name targets MGB, SGB, or CGB |
| mooneye-test-suite-wilbertpol | UNVERIFIABLE_NO_ORACLE | 6 | Logic-analysis ROM emits a hardware trace; no pass/fail oracle |
| same-suite | UNSUPPORTED_MODEL | 8 | Test name targets CGB or AGB behavior |
| same-suite | UNSUPPORTED_MODEL | 58 | Upstream test manifest runs this PCM-register APU test on CGB |
| scribbltests | UNVERIFIABLE_NO_ORACLE | 1 | Interactive WX/WY debugging utility; no reference screenshot or pass criterion |
| scribbltests | UNVERIFIABLE_NO_ORACLE | 1 | Upstream describes this as a WIP demo and ships no reference screenshot |

<details>
<summary>Full unsupported/unverifiable execution list</summary>

| ROM / case | Result | Evidence |
|---|---|---|
| `age-test-roms/lcd-align-ly/lcd-align-ly-cgbBC.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/lcd-align-ly/lcd-align-ly-cgbE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/ly/ly-cgbE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/ly/ly-ncmBC.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/ly/ly-ncmE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/m3-bg-lcdc/m3-bg-lcdc-ds.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/m3-bg-lcdc/m3-bg-lcdc-nocgb.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/m3-bg-scx/m3-bg-scx-ds.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/m3-bg-scx/m3-bg-scx-nocgb.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/oam/oam-read-cgbE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/oam/oam-read-ncmBC.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/oam/oam-read-ncmE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/oam/oam-write-cgbBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/oam/oam-write-ncmBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/speed-switch/caution/spsw-interrupts-cgbBC.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/speed-switch/caution/spsw-interrupts-cgbE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/speed-switch/spsw-ch2-lc-delay-cgbBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/speed-switch/spsw-div-cgbBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/speed-switch/spsw-mode0-cgbBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/speed-switch/spsw-stop-prefetch-cgbBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/speed-switch/spsw-tima-cgbBC.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/speed-switch/spsw-tima-cgbE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/stat-interrupt/stat-int-ncmBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/stat-mode-sprites/stat-mode-sprites-ds-cgbBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/stat-mode-window/stat-mode-window-cgbBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/stat-mode-window/stat-mode-window-ds-cgbBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/stat-mode-window/stat-mode-window-ncmBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/stat-mode/stat-mode-cgbE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/stat-mode/stat-mode-ds-cgbBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/stat-mode/stat-mode-ncmBC.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/stat-mode/stat-mode-ncmE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/vram/vram-read-cgbBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `age-test-roms/vram/vram-read-ncmBCE.gb` | UNSUPPORTED_MODEL | Test name targets CGB, CGB compatibility mode, or double speed |
| `blargg/cgb_sound/cgb_sound.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/01-registers.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/02-len ctr.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/03-trigger.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/04-sweep.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/05-sweep details.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/06-overflow on trigger.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/07-len sweep period sync.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/08-len ctr during power.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/09-wave read while on.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/10-wave trigger while on.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/11-regs after power.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/cgb_sound/rom_singles/12-wave.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `blargg/interrupt_time/interrupt_time.gb` | UNSUPPORTED_MODEL | Bundled documentation identifies this as CGB-specific |
| `dmg-acid2/dmg-acid2.gb::cgb` | UNSUPPORTED_MODEL | CGB compatibility-mode colours come from the boot ROM's title-checksum palette, which GameLad does not emulate |
| `gambatte/bgtiledata/bgtiledata_spx08_ds_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/bgtiledata/bgtiledata_spx08_ds_2.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/cgb_bgp_dumper.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/cgb_objp_dumper.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/fexx_ffxx_dumper.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/fexx_read_reset_set_dumper.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/halt/ime_noie_nolcdirq_readstat_dmg08_cgb_blank.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/halt/noime_noie_nolcdirq_readstat_dmg08_cgb_blank.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/ioregs_reset_dumper.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/jpadirq_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/jpadirq_2.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/m0enable/lycdisable_ff45_ds_2_xout0.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdma_src80_oambusy_dumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdma_srcC0_oambusy_dumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrc8000_gdmasrcC000_2xgdmalen09_oamdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrc8000_gdmasrcC000_2xgdmalen09_vramdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrc0000_gdmalen04_oamdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrc0000_gdmalen04_oamdumper_ds_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrc0000_gdmalen13_oamdumper_ds_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrc0000_gdmalen13_vramdumper_ds_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_2xgdmalen09_oamdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_2xgdmalen09_vramdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_gdmalen09_oamdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_gdmalen09_vramdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_gdmalen13_oamdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_gdmalen13_oamdumper_2.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_gdmalen13_oamdumper_ds_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_gdmalen13_oamdumper_ds_2.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_gdmalen13_oamdumper_ds_3.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_gdmalen13_vramdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC000_gdmalen13_vramdumper_ds_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC0F0_gdmalen13_oamdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/oamdma/oamdmasrcC000_gdmasrcC0F0_gdmalen13_vramdumper_1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sound/ch1_duty0_to_duty3_pos3_1_dmg08_cgb_xoutaudio1.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/11spritesPrLine_10xposA8.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_group_image_1.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_group_image_2.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_group_image_3.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_group_image_4.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_group_image_5.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_group_image_6.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_group_image_7.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_group_image_8.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_group_image_9.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_scx5_1.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_scx5_2.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_sp00x18_1.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/late_disable_sp00x18_2.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sprites/sprite_late_enable_spx19_2_dmg08_xout0.gb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/sram.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/vram_dumper.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gambatte/wram_dumper.gbc::cgb` | UNVERIFIABLE_NO_ORACLE | Upstream Gambatte runner defines no expected output for this ROM name |
| `gbmicrotest/000-oam_lock.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/000-write_to_x8000.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/001-vram_unlocked.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/002-vram_locked.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/004-tima_boot_phase.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/004-tima_cycle_timer.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/007-lcd_on_stat.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/400-dma.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/500-scx-timing.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/800-ppu-latch-scx.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/801-ppu-latch-scy.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/802-ppu-latch-tileselect.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/803-ppu-latch-bgdisplay.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/audio_testbench.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/cpu_bus_1.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/dma_basic.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/flood_vram.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/lcdon_write_timing.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/ly_while_lcd_off.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/minimal.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/mode2_stat_int_to_oam_unlock.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/oam_sprite_trashing.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/poweron.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/ppu_scx_vs_bgp.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/ppu_sprite_testbench.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/ppu_spritex_vs_scx.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/ppu_win_vs_wx.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/ppu_wx_early.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/temp.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/toggle_lcdc.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `gbmicrotest/wave_write_to_0xC003.gb` | UNVERIFIABLE_NO_ORACLE | GBMicrotest legacy/visual utility; ROM contains no FF82 verdict write |
| `mealybug-tearoom-tests/dma/hdma_during_halt-C.gb` | UNSUPPORTED_MODEL | Test name targets CGB |
| `mealybug-tearoom-tests/dma/hdma_timing-C.gb` | UNSUPPORTED_MODEL | Test name targets CGB |
| `mealybug-tearoom-tests/mbc/mbc3_rtc.gb` | UNVERIFIABLE_NO_ORACLE | RTC utility has no upstream reference screenshot |
| `mealybug-tearoom-tests/ppu/m3_lcdc_bg_en_change2.gb` | UNSUPPORTED_MODEL | Only CGB expected screenshots are bundled |
| `mealybug-tearoom-tests/ppu/m3_lcdc_bg_map_change2.gb` | UNSUPPORTED_MODEL | Only CGB expected screenshots are bundled |
| `mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_change2.gb` | UNSUPPORTED_MODEL | Only CGB expected screenshots are bundled |
| `mealybug-tearoom-tests/ppu/m3_lcdc_tile_sel_win_change2.gb` | UNSUPPORTED_MODEL | Only CGB expected screenshots are bundled |
| `mealybug-tearoom-tests/ppu/m3_lcdc_win_map_change2.gb` | UNSUPPORTED_MODEL | Only CGB expected screenshots are bundled |
| `mealybug-tearoom-tests/ppu/m3_scx_high_5_bits_change2.gb` | UNSUPPORTED_MODEL | Only CGB expected screenshots are bundled |
| `mealybug-tearoom-tests/ppu/m3_scy_change2.gb` | UNSUPPORTED_MODEL | Only CGB expected screenshots are bundled |
| `mealybug-tearoom-tests/ppu/win_without_bg.gb` | UNVERIFIABLE_NO_ORACLE | Upstream suite ships no reference image for any model |
| `mooneye-test-suite/acceptance/bits/unused_hwio-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/boot_div-S.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/boot_div2-S.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/boot_hwio-S.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/boot_regs-mgb.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite/acceptance/boot_regs-sgb.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite/acceptance/boot_regs-sgb2.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite/acceptance/di_timing-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/halt_ime1_timing2-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/oam_dma/sources-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/ppu/hblank_ly_scx_timing-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/ppu/intr_1_2_timing-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/ppu/lcdon_timing-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/ppu/lcdon_write_timing-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/acceptance/ppu/vblank_stat_intr-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/madness/mgb_oam_dma_halt_sprites.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite/misc/boot_div-A.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/misc/boot_div-cgb0.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite/misc/boot_div-cgbABCDE.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite/misc/boot_regs-A.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite/misc/boot_regs-cgb.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite/utils/bootrom_dumper.gb` | UNVERIFIABLE_NO_ORACLE | Boot ROM glitch/extraction utility is not meaningful in a post-boot run |
| `mooneye-test-suite/utils/bootrom_dumper.gb::cgb` | UNVERIFIABLE_NO_ORACLE | Boot ROM glitch/extraction utility is not meaningful in a post-boot run |
| `mooneye-test-suite-wilbertpol/acceptance/bits/unused_hwio-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/boot_hwio-G.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/di_timing-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/hblank_ly_scx_timing-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/intr_1_2_timing-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly00_mode0_2-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly00_mode1_0-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_0-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_0_write-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_144-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_153-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_153_write-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_lyc_write-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/ly_new_frame-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/stat_write_if-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/gpu/vblank_stat_intr-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/acceptance/halt_ime1_timing2-GS.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/logic-analysis/external-bus/read_timing/read_timing.gb` | UNVERIFIABLE_NO_ORACLE | Logic-analysis ROM emits a hardware trace; no pass/fail oracle |
| `mooneye-test-suite-wilbertpol/logic-analysis/external-bus/read_timing/read_timing.gb::cgb` | UNVERIFIABLE_NO_ORACLE | Logic-analysis ROM emits a hardware trace; no pass/fail oracle |
| `mooneye-test-suite-wilbertpol/logic-analysis/external-bus/write_timing/write_timing.gb` | UNVERIFIABLE_NO_ORACLE | Logic-analysis ROM emits a hardware trace; no pass/fail oracle |
| `mooneye-test-suite-wilbertpol/logic-analysis/external-bus/write_timing/write_timing.gb::cgb` | UNVERIFIABLE_NO_ORACLE | Logic-analysis ROM emits a hardware trace; no pass/fail oracle |
| `mooneye-test-suite-wilbertpol/logic-analysis/ppu/simple_scx/simple_scx.gb` | UNVERIFIABLE_NO_ORACLE | Logic-analysis ROM emits a hardware trace; no pass/fail oracle |
| `mooneye-test-suite-wilbertpol/logic-analysis/ppu/simple_scx/simple_scx.gb::cgb` | UNVERIFIABLE_NO_ORACLE | Logic-analysis ROM emits a hardware trace; no pass/fail oracle |
| `mooneye-test-suite-wilbertpol/madness/mgb_oam_dma_halt_sprites.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite-wilbertpol/misc/boot_hwio-S.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/misc/boot_regs-A.gb` | UNSUPPORTED_MODEL | Mooneye model suffix does not include DMG |
| `mooneye-test-suite-wilbertpol/misc/boot_regs-cgb.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite-wilbertpol/misc/boot_regs-mgb.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite-wilbertpol/misc/boot_regs-sgb.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `mooneye-test-suite-wilbertpol/misc/boot_regs-sgb2.gb` | UNSUPPORTED_MODEL | Test name targets MGB, SGB, or CGB |
| `same-suite/apu/channel_1/channel_1_align.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_align_cpu.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_duty.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_duty_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_extra_length_clocking-cgb0B.gb` | UNSUPPORTED_MODEL | Test name targets CGB or AGB behavior |
| `same-suite/apu/channel_1/channel_1_freq_change.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_freq_change_timing-A.gb` | UNSUPPORTED_MODEL | Test name targets CGB or AGB behavior |
| `same-suite/apu/channel_1/channel_1_freq_change_timing-cgb0BC.gb` | UNSUPPORTED_MODEL | Test name targets CGB or AGB behavior |
| `same-suite/apu/channel_1/channel_1_freq_change_timing-cgbDE.gb` | UNSUPPORTED_MODEL | Test name targets CGB or AGB behavior |
| `same-suite/apu/channel_1/channel_1_nrx2_glitch.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_nrx2_speed_change.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_restart.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_restart_nrx2_glitch.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_stop_div.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_stop_restart.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_sweep.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_sweep_restart.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_sweep_restart_2.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_volume.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_1/channel_1_volume_div.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_align.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_align_cpu.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_duty.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_duty_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_extra_length_clocking-cgb0B.gb` | UNSUPPORTED_MODEL | Test name targets CGB or AGB behavior |
| `same-suite/apu/channel_2/channel_2_freq_change.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_nrx2_glitch.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_nrx2_speed_change.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_restart.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_restart_nrx2_glitch.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_stop_div.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_stop_restart.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_volume.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_2/channel_2_volume_div.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_and_glitch.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_extra_length_clocking-cgb0.gb` | UNSUPPORTED_MODEL | Test name targets CGB or AGB behavior |
| `same-suite/apu/channel_3/channel_3_extra_length_clocking-cgbB.gb` | UNSUPPORTED_MODEL | Test name targets CGB or AGB behavior |
| `same-suite/apu/channel_3/channel_3_first_sample.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_freq_change_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_restart_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_restart_during_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_restart_stop_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_shift_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_shift_skip_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_stop_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_stop_div.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_3/channel_3_wave_ram_sync.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_align.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_delay.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_equivalent_frequencies.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_extra_length_clocking-cgb0B.gb` | UNSUPPORTED_MODEL | Test name targets CGB or AGB behavior |
| `same-suite/apu/channel_4/channel_4_freq_change.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_frequency_alignment.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_lfsr.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_lfsr15.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_lfsr_15_7.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_lfsr_7_15.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_lfsr_restart.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_lfsr_restart_fast.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/channel_4/channel_4_volume_div.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/div_trigger_volume_10.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/div_write_trigger_volume.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `same-suite/apu/div_write_trigger_volume_10.gb` | UNSUPPORTED_MODEL | Upstream test manifest runs this PCM-register APU test on CGB |
| `scribbltests/fairylake/fairylake.gb` | UNVERIFIABLE_NO_ORACLE | Upstream describes this as a WIP demo and ships no reference screenshot |
| `scribbltests/winpos/winpos.gb` | UNVERIFIABLE_NO_ORACLE | Interactive WX/WY debugging utility; no reference screenshot or pass criterion |

</details>

## Interpretation

The original audit's broad CPU timing, interrupt/HALT, timer, serial, MBC/RTC,
DMA, PPU/FIFO, APU, and CGB omissions have been addressed. Remaining objective
failures are concentrated in finer hardware traces: sub-machine-cycle
PPU/STAT/fetch timing, CGB speed-switch and HDMA boundaries, model-specific
OAM-DMA arbitration, exact APU edge behavior, and revision-specific cartridge
or boot behavior. `GAMEBOY_FIX_SUMMARY.md` records the implemented changes,
reviewed tradeoffs, and known limits.

This corpus contains tests for multiple hardware revisions and several
utilities that intentionally have no binary verdict. Unsupported and
no-oracle executions are therefore reported separately and are never counted
as passes or failures.

## Coverage integrity

- Unique discovered ROMs: 4528
- Unique ROMs represented in results: 4528
- Additional subtest executions: 2152
- Missing paths: 0
- Repeated-run status differences: 0
