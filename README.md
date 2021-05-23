# Level Stats Server

A mod which sends your level stats to a client.

## Overlays
You can get some overlays [here](https://computerelite.github.io/tools/Level_Stats_Quest_Overlay/)

## For developers
Example JSON which you get from `http://[IP]:3501`

```json
{
    "levelName": "256*256",
    "energy": 0.40999993681907654,
    "combo": 0,
    "percentage": 0.3498721122741699,
    "songAuthor": "Seiryu x t+pazolite",
    "difficulty": 4,
    "score": 684,
    "timePlayed": 5,
    "totalTime": 241,
    "id": "custom_level_CEA6EA6A38D25D20BC6A0F548BE2962835C54377",
    "levelSub": "",
    "mapper": "Rise & SieR",
    "rank": "D",
    "bpm": 256,
    "njs": 23
}
```
### Difficulty
This can be a value from 0 to 4 (Easy, Normal, Hard, Expert, Expert +).

### energy
The energy you have left 0.0 to 1.0

### timePlayed and totalTime
both are in seconds

### percentage
Your accuracy from 0.0 to 1.0

Rest should explain itself

Here's an example js file which fetches data and edits an html https://computerelite.github.io/tools/Level_Stats_Quest_Overlay/pulling.js

## Credits
* [Laurie](https://github.com/Lauriethefish) made most of the code since this is a copy of his [Discord rich presence mod](https://github.com/Lauriethefish/Quest-Discord-Presence)
* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario) - [vscode-bsqm](https://github.com/raftario/vscode-bsqm) and [this template](https://github.com/raftario/bmbf-mod-template)
