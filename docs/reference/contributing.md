# Contributing

CL02 documentation should stay source-faithful and reproducible.

## Documentation Rules

- Prefer repository paths, screenshots, measured behavior, and firmware source over inferred behavior.
- Keep device claims tied to the public hardware labels, firmware files, README text, or source code.
- Do not add speculative neuroscience or stimulation-safety claims.
- Do not replace real hardware or UI screenshots with generated device imagery.
- When adding setup guidance, include the exact firmware image, installer, PCB revision, and parameter names affected.

## Useful Checks

Before submitting documentation changes:

1. Run `mkdocs build --strict --verbose`.
2. Check links to repository paths.
3. Review desktop and mobile rendering.
4. Confirm screenshots are source assets under `docs/images`, not generated `site` output.
5. Leave unrelated firmware, Windows build output, and binary artifacts untouched unless the change explicitly targets them.
