# Umbenennungs-Mapping

Die urspruenglichen Wiki-Seitentitel enthalten `:` (im GitHub-Wiki als
Kategorie-Trennzeichen genutzt). NTFS auf Windows verbietet `:` in
Dateinamen. Beim Import wurden die entsprechenden Files nach dem Muster
`-:-` → `_-_` umbenannt.

Diese Tabelle dient als Referenz, wenn spaeter Wiki-interne Links
(`[[Hardware-:-Grundlagen]]` etc.) aufgeloest werden muessen.

| Original-Name (GitHub-Wiki) | Import-Name (in diesem Ordner) |
|---|---|
| `Hardware-:-Grundlagen.md` | `Hardware_-_Grundlagen.md` |
| `Hardware-:-Platine-(Überprüfung).md` | `Hardware_-_Platine-(Überprüfung).md` |
| `Hardware-:-Platine.md` | `Hardware_-_Platine.md` |
| `Hardware-:-Zusätzliche-Sensoren.md` | `Hardware_-_Zusätzliche-Sensoren.md` |
| `Home.md` | `Home.md` (unveraendert) |
| `MQTT-:-Grundlagen-und-Erweiterungen.md` | `MQTT_-_Grundlagen-und-Erweiterungen.md` |
| `Software-:-Erstinbetriebnahme.md` | `Software_-_Erstinbetriebnahme.md` |
| `Sonderfunktionen-:-Melder-und-Alarmierungsgruppen.md` | `Sonderfunktionen_-_Melder-und-Alarmierungsgruppen.md` |
| `_Sidebar.md` | `_Sidebar.md` (unveraendert) |

Quelle: `git clone https://github.com/Sefina-DS/Gira-OTM-Adapter.wiki.git`
(gespiegelt nach `E:\- Claude Code -\_WIKI-SPIEGEL\`, liegt ausserhalb
dieses Repos).
