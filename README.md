# Gira-OTM-Adapter

ESPHome-basierte Firmware fuer die Adapter-Platine an Gira DualQ Rauchmeldern.

Zwei-teilige Dokumentation im Ordner `docs/`:

- **`docs/protokoll/`** — Universelles Wissen ueber die Adapter-Platine und
  das serielle Gira-Protokoll. Wer eine eigene Firmware, ein anderes Framework
  oder einen anderen Anwendungsfall bauen moechte, findet hier die Referenz.
- **`docs/projekt/`** — Unser konkretes ESPHome-Setup. Wer unsere Firmware
  reproduzieren oder verwenden moechte, findet hier die Anleitungen.

## Verzeichnisse

- `components/gira_rauchmelder/` — ESPHome External Component (Protokoll-Logik in C++)
- `packages/` — ESPHome-Hardware-Template (feste PCB-Belegung)
- `devices/` — Konkrete Melder-YAMLs (duenn, nur Parameter)
- `firmware/` — Optional: fertige Binaries

## Setup

1. `secrets.yaml.example` nach `secrets.yaml` kopieren und ausfuellen
2. (weitere Schritte kommen — siehe `docs/projekt/inbetriebnahme.md`)

## Lizenz

MIT. Siehe `LICENSE`.
