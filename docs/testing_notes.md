# Gameboy ROM Testing Notes

## Program Counter Values

### Intruction Timing

| file           | PC Value  | Notes                                                    |
|----------------|-----------|----------------------------------------------------------|
| timer.s        | 49855     | End of timer delay in `init_timer`, right before tests   |
|                | 49886     | Right before return from `start_timer`                   |
| instr_timing.s | 49926     | Call to start timer during `test_timer`                  |
|                | 49951     | return from `stop_timer` in `test_timer`                 |
