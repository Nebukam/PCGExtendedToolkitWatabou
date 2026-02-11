![UE 5.7](https://img.shields.io/badge/UE-5.7-darkgreen) ![5.6](https://img.shields.io/badge/5.6-darkgreen)

<p align="center">
  <img src="https://github.com/Nebukam/PCGExElementsWatabou/blob/main/Resources/Watabou_Logo.png" alt="PCGEx Logo">
</p>

<h1 align="center">PCGEx + Watabou</h1>

<p align="center">
  <strong>Import procedural maps from Watabou's generators into Unreal Engine's PCG framework</strong><br>
  Cities, villages, neighborhoods, and dungeons — from JSON to PCG data in seconds.
</p>

<p align="center">
  <a href="https://pcgex.gitbook.io/pcgex/watabou">Documentation</a> •
  <a href="https://discord.gg/mde2vC5gbE">Discord</a> •
  <a href="https://www.fab.com/listings/---">FAB</a>
</p>

---

## What is PCGEx + Watabou?

This is a **companion plugin for [PCGEx](https://github.com/Nebukam/PCGExtendedToolkit)** that converts JSON exports from [Watabou's procedural generators](https://watabou.itch.io/) into PCG & PCGEx data you can use directly in Unreal Engine.

![Image](https://github.com/user-attachments/assets/b9cd1aa8-9f61-49b6-8428-5082f60bc295)

### Supported Generators

- [Medieval Fantasy City Generator](https://watabou.itch.io/medieval-fantasy-city-generator)
- [Neighborhood Generator](https://watabou.itch.io/neighbourhood)
- [Village Generator](https://watabou.itch.io/village-generator)
- [One Page Dungeon](https://watabou.itch.io/one-page-dungeon)

### Experimental Status

This plugin is **experimental**. Watabou's JSON data structures may change over time. The code is designed to support multiple data versions, but if something breaks, [open an issue](https://github.com/Nebukam/PCGExElementsWatabou/issues) and I'll look into it.

---

## Requirements

- **Unreal Engine 5.6+**
- **[PCGExtendedToolkit](https://github.com/Nebukam/PCGExtendedToolkit)** — Core PCGEx plugin (free, MIT licensed)

---

## Installation

### From FAB
Get the latest release from the **[FAB Marketplace](https://www.fab.com/listings/---)**.

### From Source
1. Clone this repository into your project's `Plugins/` folder
2. Ensure **PCGExtendedToolkit** is also installed
3. Regenerate project files and build

---

## Getting Started

1. Go to any of the supported Watabou generators listed above
2. Right-click and export to JSON
3. Rename the file extension from `.json` to `.wataboujson` — this is key for the auto-import to kick in
4. Drag and drop the file into Unreal — this generates a new Data Asset. Save it.
5. In a PCG Graph, use the `Load Watabou Data` node and pick your data asset

> The content of the JSON export depends on what is displayed in the generator. Some generators expose different data types such as districts — play around with them!

---

## License

**MIT License** — Free for personal and commercial use. See [LICENSE](LICENSE) for full terms.

---

## Support

- **[Discord](https://discord.gg/mde2vC5gbE)** — Community support and discussion
- **[Documentation](https://pcgex.gitbook.io/pcgex)** — Guides and tutorials
- **[Patreon](https://www.patreon.com/c/pcgex)** — Support PCGEx development
