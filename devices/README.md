# devices/

Dieser Ordner enthaelt Melder-YAMLs. **Konkrete Melder-Instanzen werden nicht
ins Repo committed** — sie sind pro Installation individuell und liegen
lokal in deinem ESPHome-Config-Verzeichnis.

## Neuen Melder anlegen

1. `EXAMPLE.yaml` in deinen ESPHome-Config-Ordner kopieren
2. Sprechenden Namen vergeben:
   - Datei: `Gira-RM_<Standort>.yaml` (z.B. `Gira-RM_HG_EG_Flur.yaml`)
   - `esphome.name` in der YAML: `gira-rm-<standort>` (lowercase, Bindestriche)
   - `friendly_name`: `"Gira-RM <Standort>"` (Klartext)
3. In `substitutions:` die Melder-spezifischen Werte anpassen
   (Meldegruppe, Alarmierungsgruppen etc.)
4. `secrets.yaml` muss `wifi_ssid`, `wifi_password`, `api_key`, `ota_password`
   enthalten (siehe `../secrets.yaml.example`)
5. Im ESPHome-Dashboard flashen

## Warum nicht im Repo?

- Sprechende Namen ohne Namens-Kollisionen mit anderen Installationen
- Meldergruppen, Montageorte usw. sind private Details, die keinen
  Consumer interessieren
- Aenderungen an einzelnen Meldern sind selten — das Wiederverwendbare
  (Component, Package, Doku) liegt im Repo, das aendert sich haeufiger

## Dateien in diesem Ordner, die IM Repo landen

- `EXAMPLE.yaml` — die Vorlage
- `README.md` — diese Datei
