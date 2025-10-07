# TODO: Fix format specifier warnings in saloha

## saloha.c ✅
- Line 41: %d -> %ld for slot
- Line 48: %d -> %ld for slot
- Line 67: %d -> %ld for nstns
- Line 68: %9d -> %9ld for nstns
- Line 96: %9d -> %9ld for nslots
- Line 101: %9d -> %9ld for start_stats
- Line 185: %d -> %ld for slot

## stats.c
- Line 77: %d %d %d -> %ld %f %ld for slot, m, samples
- Line 136: %6d -> %6ld for v[i]
- Line 150: %d -> %ld for samples
- Line 151: %d %d -> %ld %ld for min, max
- Line 152-153: %d %d -> %ld %ld for jitter, percentile
- Line 163: %d -> %ld for slot
- Line 167: %d -> %ld for slot
- Line 172: %d -> %ld for slot
- Line 176: %d -> %ld for slot
- Line 181: %d -> %ld for slot
- Line 185: %d -> %ld for slot
- Line 189: %d -> %ld for slot
- Line 193: %d -> %ld for slot
- Line 198: %d -> %ld for slot
- Line 202: %d -> %ld for slot
- Line 206: %d -> %ld for slot
- Line 211: %d -> %ld for slot
- Line 215: %d -> %ld for slot
- Line 219: %d -> %ld for slot
- Line 225: %d -> %ld for slot
- Line 229: %d -> %ld for slot
- Line 233: %d -> %ld for slot
- Line 239: %d -> %ld for slot
- Line 243: %d -> %ld for slot
- Line 247: %d -> %ld for slot
- Line 323: %d -> %ld for slot

## stochastic.c
- Line 59: %d -> %ld for slot
- Line 63: %4d -> %4ld for slot
- Line 125: %d -> %ld for slot

## protocol.c
- Line 69: %d -> %ld for slot
- Line 71: %d -> %ld for slot
- Line 73: Add arguments slot, pk.sarv_time, pk.iservtime
- Line 189: %d -> %ld for slot
- Line 196: %d %d %d %d %d %d %d -> %ld %ld %ld %ld %ld %ld %ld for slot, s, gload, snt, lng
- Line 201: %d %d %d -> %ld %ld %ld for slot, s, gload
- Line 202: %d -> %ld for tpk

## Recompile and verify
